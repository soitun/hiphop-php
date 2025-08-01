(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude
open Common
open Typing_defs
module Reason = Typing_reason
module Env = Typing_env
module Log = Typing_log
module Phase = Typing_phase
module TySet = Typing_set
module Cls = Folded_class
module MakeType = Typing_make_type

(* A guiding principle when expanding a type access C::T is that if C <: D and
   we know that D::T = X (represented by an Exact result below), then C::T is
   also X. So Exact is propagated down the <: relation, see `update_class_name`
   below where this behavior is encoded. *)

(** When the access code is going to return a `Tgeneric "Foo::Bar"`,
  we use the following type to tell the difference between legit
  accesses from known concrete types (`this`, or expression-
  dependent types), and unsound accesses from arbitrary generic
  variables. *)
type root_kind =
  | ConcreteClass
  | GenericType

type context = {
  id: pos_id;  (** The T in the type access C::T *)
  ety_env: expand_env;
      (** The expand environment as passed in by Phase.localize *)
  generics_seen: TySet.t;
      (** The set of visited generics used to avoid infinite loops during expansion.
        These are all some `Tgeneric (name, tyargs)`. *)
  allow_abstract: bool;
      (** Whether or not an abstract type constant is allowed as the result. In the
        future, this boolean should disappear and abstract type constants should
        appear only in the class where they are defined. *)
  abstract_as_tyvar_at_pos: Pos.t option;
      (** If set, abstract type constants will be expanded as type variables. This
        is a hack which should naturally go away when the semantics of abstract
        type constants is cleaned up. *)
  base: locl_ty option;
      (** The origin of the extension. For example if TC is a generic parameter
        subject to the constraint TC as C and we would like to expand TC::T we
        will expand C::T with base set to `Some (Tgeneric "TC")` (and root set
        to C). If it is None the base is exactly the current root. *)
  root_kind: root_kind;  (** See documentation for the [root_kind] type. *)
}

(** The result of an expansion
  - Missing err means that the type constant is not present, with error function
    to be called if we need to report this
  - Exact ty means that the expansion results precisely in 'ty'
  - Abstract (n0, [n1, n2, n3], lower_bound, upper_bound) means that the result is a
    generic with name n0::T such that:
    n0::T as n1::T as n2::T as n3::T as upper_bound super lower_bound *)
type result =
  | Missing of Typing_error.t option
  | Exact of locl_ty
  | Abstract of abstract

and abstract = {
  name: string;
  names: string list;
  lower_bounds: TySet.t;
  upper_bounds: TySet.t;
}

let make_reason env id root r =
  Reason.typeconst (r, id, lazy (Typing_print.error env root), get_reason root)

(* FIXME: It is bogus to use strings here and put them in Tgeneric; one
   possible problem is when a type parameter has a name which conflicts
   with a class name *)
let tp_name class_name id = class_name ^ "::" ^ snd id

(** A smart constructor for Abstract that also checks if the type we are
  creating is known to be equal to some other type *)
let abstract_or_exact env id ({ name; _ } as abstr) =
  let tp_name = tp_name name id in
  if not (TySet.is_empty (Env.get_equal_bounds env tp_name)) then
    (* If the resulting abstract type is exactly equal to something,
       mark the result as exact.
       For example, if we have the following
       abstract class Box {
         abstract const type T;
       }
       function addFiveToValue<T1 as Box>(T1 $x) : int where T1::T = int {
           return $x->get() + 5;
       }
       Here, $x->get() has type expr#1::T as T1::T (as Box::T).
       But T1::T is exactly equal to int, so $x->get() no longer needs
       to be expression dependent. Thus, $x->get() typechecks. *)
    Exact (MakeType.generic Reason.none tp_name)
  else
    Abstract abstr

(** In the case of an abstract type constant with a bound which refines the
    constant to be equal to a constant in the class from which we are projecting,
    localization will proceed and determine that we have a cycle:

    interface I {
      abstract const type T as I with { type T = I::T; };
                                                 ^^^^
      public function next(): this::T;
    }

    In this example if we were to recursively localize `I::T` we would encounter
    a cycle. To avoid this, we have to make a substitution for the result of
    localization.

    Since any type constant triggering this case is abstract, we know that the
    receiver (`this`) will be an 'expression dependent' type and we only need
    to rewrite in this case.

    We therefore know that resulting type is the mangled generic we use to
    represent accesses on expression dependent types.

    This function transforms a decl_ty looking for occurrences of an access
    of the [type_const_name] on [TThis] and replaces them with a mangled generic
    obtained by appending the [type_const_name] to the string representation of
    the expression dependent type we are accessing through. *)
let eliminate_recursive_access ty type_const_name this_ty =
  match get_node this_ty with
  | Tdependent (DTexpr expr_id, _) ->
    let this_name = Expression_id.display expr_id in
    let on_ty ty ~ctx =
      match deref ty with
      | (reason, Taccess (root_ty, (_, name)))
        when String.equal name type_const_name -> begin
        match get_node root_ty with
        | Tthis ->
          let ty =
            mk
              ( reason,
                Tgeneric (Format.sprintf "%s::%s" this_name type_const_name) )
          in
          (ctx, `Stop ty)
        | _ -> (ctx, `Continue ty)
      end
      | _ -> (ctx, `Continue ty)
    and on_rc_bound rc_bound ~ctx = (ctx, `Continue rc_bound) in
    Typing_defs_core.transform_top_down_decl_ty ty ~on_ty ~on_rc_bound ~ctx:()
  | _ -> ty

(** [create_root_from_type_constant ctx env root class_name class_]
  looks up a type constant in a class and returns a `result`. More precisely, it looks up
  [ctx.id] in [class_]. [class_name] is the class name for [class_] and [root]
  is a `Tclass (class_name, ...)`.

  A type constant has both a constraint type and assigned type. Which one we choose depends if
  the current root is the base (origin) of the expansion (in which case [ctx.base] is `None`),
  or if it is an upper bound of the base (in which case [ctx.base] is `Some`). *)
let create_root_from_type_constant ctx env root (_class_pos, class_name) class_
    : _ * result =
  let { id = (id_pos, type_const_name) as id; _ } = ctx in
  match Env.get_typeconst env class_ type_const_name with
  | None ->
    ( (env, None, []),
      Missing
        (Option.map
           ctx.ety_env.on_error
           ~f:
             Typing_error.(
               fun on_error ->
                 apply_reasons ~on_error
                 @@ Secondary.Smember_not_found
                      {
                        pos = id_pos;
                        kind = `class_typeconst;
                        class_name;
                        class_pos = Cls.pos class_;
                        member_name = type_const_name;
                        hint = None;
                      })) )
  | Some typeconst ->
    (match
       Typing_defs.add_type_expansion_check_cycles
         ctx.ety_env
         {
           Type_expansions.name =
             Type_expansions.Expandable.Type_constant
               { receiver_name = class_name; type_const_name };
           use_pos = id_pos;
           def_pos = None;
         }
     with
    | Error cycle ->
      (* This is a cycle through a type constant that we are using *)
      ((env, None, [cycle]), Missing None)
    | Ok ety_env ->
      let ctx = { ctx with ety_env } in
      let ety_env =
        let drop_exact ty =
          (* Legacy behavior is to preserve exactness only on `this` and not
             through `this::T` *)
          map_ty ty ~f:(function
              | Tclass (cid, _, tyl) -> Tclass (cid, nonexact, tyl)
              | ty -> ty)
        in
        {
          ctx.ety_env with
          this_ty = drop_exact (Option.value ctx.base ~default:root);
        }
      in
      let abstract_or_exact env ~lower_bounds ~upper_bounds =
        let ty_err_opt =
          if not ctx.allow_abstract then
            Option.map
              ctx.ety_env.on_error
              ~f:
                Typing_error.(
                  fun on_error ->
                    apply_reasons ~on_error
                    @@ Secondary.Abstract_tconst_not_allowed
                         {
                           pos = id_pos;
                           decl_pos = fst typeconst.ttc_name;
                           tconst_name = type_const_name;
                         })
          else
            None
        in
        (* TODO(T59448452): this treatment of abstract type constants is unsound *)
        ( abstract_or_exact
            env
            id
            { name = class_name; names = []; lower_bounds; upper_bounds },
          ty_err_opt )
      in
      (* Don't report errors in expanded definition or constraint.
       * These will have been reported at the definition site already. *)
      let ety_env = { ety_env with on_error = None } in
      (match typeconst.ttc_kind with
      (* Concrete type constants *)
      | TCConcrete { tc_type = ty } ->
        let ((env, ty_err_opt, cycles), ty) =
          Phase.localize_rec ~ety_env env ty
        in
        let ty = map_reason ty ~f:(make_reason env id root) in
        ((env, ty_err_opt, cycles), Exact ty)
      (* Abstract type constants *)
      | TCAbstract
          { atc_as_constraint = upper; atc_super_constraint = lower; _ } ->
        let upper =
          Option.map upper ~f:(fun ty ->
              eliminate_recursive_access ty type_const_name ctx.ety_env.this_ty)
        and lower =
          Option.map lower ~f:(fun ty ->
              eliminate_recursive_access ty type_const_name ctx.ety_env.this_ty)
        in
        let to_tys env : decl_ty option -> _ * TySet.t = function
          | Some ty ->
            let ((env, err, cycles), ty) = Phase.localize_rec ~ety_env env ty in
            ((env, err, cycles), TySet.singleton ty)
          | None -> ((env, None, []), TySet.empty)
        in
        let ((env, e1, cycles_lower), lower_bounds) = to_tys env lower in
        let ((env, e2, cycles_upper), upper_bounds) = to_tys env upper in
        let (res, e3) = abstract_or_exact env ~lower_bounds ~upper_bounds in
        let ty_err_opt =
          Typing_error.multiple_opt @@ List.filter_map ~f:Fn.id [e1; e2; e3]
        in
        ((env, ty_err_opt, cycles_lower @ cycles_upper), res)))

(* Cheap intersection operation. Do not call Typing_intersection.intersect
 * as this calls into subtype which in turn calls into expand_with_env below
 *)
let intersect ty1 ty2 =
  if equal_locl_ty ty1 ty2 then
    ty1
  else
    MakeType.intersection (get_reason ty1) [ty1; ty2]

(* Cheap union operation. Do not call Typing_union.union
 * as this calls into subtype which in turn calls into expand_with_env below
 *)
let union ty1 ty2 =
  if equal_locl_ty ty1 ty2 then
    ty1
  else
    MakeType.union (get_reason ty1) [ty1; ty2]

(* Given the results of projecting a type constant from types t1 , ... , tn,
 * determine the result of projecting a type constant from type (t1 | ... | tn).
 * If the type constant is missing from any of the disjuncts, then it's treated
 * as missing from the union.
 *)
let rec union_results err rl =
  match rl with
  | [] -> Missing err
  | [r] -> r
  | r1 :: rl ->
    let r2 = union_results err rl in

    (* Union is defined iff both are defined concretely *)
    (match (r1, r2) with
    | (Missing err, _)
    | (_, Missing err) ->
      Missing err
    (* In essence, this says (C | D)::TP = (C::TP) | (D::TP) *)
    | (Exact ty1, Exact ty2) -> Exact (union ty1 ty2)
    (* We don't support projecting through any other kind of union *)
    | _ -> Missing err)

(* Given the results of projecting a type constant from types t1, ..., tn,
 * determine the result of projecting a type constant from type (t1 & ... & tn).
 *)
let rec intersect_results err rl =
  match rl with
  (* Essentially, this is `mixed`. *)
  | [] -> Missing err
  | [r] -> r
  | r1 :: rs ->
    let r2 = intersect_results err rs in
    (match (r1, r2) with
    | (Missing _, r)
    | (r, Missing _) ->
      r
    (* In essence, we're saying (C & D)::TP = (C::TP) & (D::TP) *)
    | (Exact ty1, Exact ty2) -> Exact (intersect ty1 ty2)
    (* Here, we merge the bounds on abstract type constants. Effectively this is intersection. *)
    | ( Abstract { name; names; lower_bounds = lowerl1; upper_bounds = upperl1 },
        Abstract { lower_bounds = lowerl2; upper_bounds = upperl2; _ } ) ->
      Abstract
        {
          name;
          names;
          lower_bounds = TySet.union lowerl1 lowerl2;
          upper_bounds = TySet.union upperl1 upperl2;
        }
    (* Exact type overrides abstract type: the bound on abstract type will be checked
       * against the exact type at implementation site. *)
    | (Abstract _, Exact ty)
    | (Exact ty, Abstract _) ->
      Exact ty)

let rec type_of_result ~ignore_errors ctx env root res =
  let type_with_bound env as_tyvar name ~lower_bounds ~upper_bounds =
    match as_tyvar with
    | Some tyvar_pos ->
      let (env, tvar) = Env.fresh_type_invariant env tyvar_pos in
      Log.log_new_tvar_for_tconst_access env tyvar_pos tvar name ctx.id;
      (env, tvar)
    | None ->
      let generic_name = tp_name name ctx.id in
      let reason = make_reason env ctx.id root Reason.none in
      let ty = MakeType.generic reason generic_name in
      let env =
        TySet.fold
          (fun bnd env ->
            (* TODO(T59317869): play well with flow sensitivity *)
            Env.add_upper_bound_global env generic_name bnd)
          upper_bounds
          env
      in
      let env =
        TySet.fold
          (fun bnd env ->
            (* TODO(T59317869): play well with flow sensitivity *)
            Env.add_lower_bound_global env generic_name bnd)
          lower_bounds
          env
      in
      (env, ty)
  in
  match res with
  | Exact ty -> ((env, None), ty)
  | Abstract { name; names = name' :: namel; lower_bounds; upper_bounds } ->
    let res' =
      Abstract { name = name'; names = namel; lower_bounds; upper_bounds }
    in
    let ((env, ty_err_opt), ty) =
      type_of_result ~ignore_errors ctx env root res'
    in
    let (env, ty) =
      type_with_bound
        env
        None
        name
        ~lower_bounds
        ~upper_bounds:(TySet.singleton ty)
    in
    ((env, ty_err_opt), ty)
  | Abstract { name; names = []; lower_bounds; upper_bounds } ->
    let (env, ty) =
      type_with_bound
        env
        ctx.abstract_as_tyvar_at_pos
        name
        ~lower_bounds
        ~upper_bounds
    in
    ((env, None), ty)
  | Missing err ->
    let ty_err_opt =
      if not ignore_errors then
        err
      else
        None
    in
    (* TODO akenn: this is a source of unsoundness *)
    let ty =
      Typing_utils.mk_tany env (Pos_or_decl.unsafe_to_raw_pos (get_pos root))
    in
    ((env, ty_err_opt), ty)

let update_class_name env id new_name = function
  | (Exact _ | Missing _) as res -> res
  | Abstract ({ name; names; _ } as abstr) ->
    abstract_or_exact
      env
      id
      { abstr with name = new_name; names = name :: names }

let is_opaque_internal ty =
  Reason.Predicates.is_opaque_type_from_module (get_reason ty)

let rec expand ctx env root =
  let (env, root) = Env.expand_type env root in
  let err_opt =
    let (pos, tconst) = ctx.id in
    let ty_name = lazy (Typing_print.error env root) in
    let reason =
      Typing_error.Secondary.Non_object_member
        {
          pos;
          ctxt = `read;
          kind = `class_typeconst;
          member_name = tconst;
          ty_name;
          decl_pos = get_pos root;
        }
    in
    Option.map ctx.ety_env.on_error ~f:(fun on_error ->
        Typing_error.apply_reasons ~on_error reason)
  in
  if Typing_utils.is_tyvar_error env root then
    ((env, None, []), Missing None)
  else
    match get_node root with
    | Tany _ -> ((env, None, []), Exact root)
    (* We allow inheritance of members from internal classes and traits into public ones,
       even if their signatures include internal types, but when localizing such internal
       types we pretend that a Tclass is a Tnewtype i.e. is opaque.
       Here we deal with projection from a newtype that was generated in such cases e.g.
           internal class Base { const type TC = int; public static function foo(self::TC $_):void { } }
           class Derived extends Base { }
           function bar():void { Derived::foo("A"); }
        Generate another fake newtype by mangling the name using the projection syntax.
    *)
    | Tnewtype (name, _tyl, bound) when is_opaque_internal root ->
      ( (env, None, []),
        Exact
          (mk (get_reason root, Tnewtype (name ^ "::" ^ snd ctx.id, [], bound)))
      )
    | Tnewtype (name, _, ty) ->
      let ctx =
        let base = Some (Option.value ctx.base ~default:root) in
        let allow_abstract = true in
        { ctx with base; allow_abstract }
      in
      let ((env, ty_err_opt, cycles), res) = expand ctx env ty in
      let name = Printf.sprintf "<cls#%s>" name in
      ((env, ty_err_opt, cycles), update_class_name env ctx.id name res)
    | Tclass (cid, Nonexact cr, targs)
      when Class_refinement.has_refined_const ctx.id cr -> begin
      match Class_refinement.get_refined_const ctx.id cr with
      | Some { rc_bound = TRexact ty; _ } -> ((env, None, []), Exact ty)
      | Some { rc_bound = TRloose { tr_lower; tr_upper }; _ } ->
        let alt_root = mk (get_reason root, Tclass (cid, nonexact, targs)) in
        let ((env, ty_err_opt, cycles_alt_root), result) =
          expand ctx env alt_root
        in
        (match result with
        | Exact _
        | Missing _ ->
          ((env, ty_err_opt, cycles_alt_root), result)
        | Abstract abstr ->
          (* Flag an error if the root is not a concrete class type. *)
          let ty_err_opt' =
            match ctx.root_kind with
            | ConcreteClass -> None
            | GenericType ->
              let pos = Reason.to_pos (get_reason root) in
              Option.map ctx.ety_env.on_error ~f:(fun on_error ->
                  Typing_error.apply_reasons
                    ~on_error
                    (Typing_error.Secondary.Inexact_tconst_access (pos, ctx.id)))
          in
          let ty_err_opt = Option.first_some ty_err_opt' ty_err_opt in
          let add_bounds tyset bnds =
            List.fold bnds ~init:tyset ~f:(fun tyset t -> TySet.add t tyset)
          in
          let abstr =
            {
              abstr with
              lower_bounds = add_bounds abstr.lower_bounds tr_lower;
              upper_bounds = add_bounds abstr.upper_bounds tr_upper;
            }
          in
          ((env, ty_err_opt, cycles_alt_root), Abstract abstr))
      | None -> (* unreachable *) ((env, None, []), Missing None)
    end
    | Tclass (cls, _, _) -> begin
      Env.get_class env (snd cls)
      |> Decl_entry.map_or
           ~default:((env, None, []), Missing None)
           ~f:(fun ci ->
             (* Hack: `self` in a trait is mistakenly replaced by the trait instead
                of the class using the trait, so if a trait is the root, it is
                likely because originally there was `self::T` written.
                TODO(T54081153): fix `self` in traits and clean this up *)
             let allow_abstract =
               Ast_defs.is_c_trait (Cls.kind ci) || ctx.allow_abstract
             in
             let ctx = { ctx with allow_abstract } in
             create_root_from_type_constant ctx env root cls ci)
    end
    | Tgeneric s ->
      let ctx =
        {
          ctx with
          generics_seen = TySet.add root ctx.generics_seen;
          base = Some (Option.value ctx.base ~default:root);
          allow_abstract = true;
          abstract_as_tyvar_at_pos = None;
          root_kind =
            (let this_name = Naming_special_names.SpecialIdents.this in
             (* The `this` type stands for a concrete class. *)
             if String.equal s this_name then
               ConcreteClass
             else
               ctx.root_kind);
        }
      in
      let is_supportdyn_mixed t =
        let (is_supportdyn, _, t) = Typing_utils.strip_supportdyn env t in
        is_supportdyn && Typing_utils.is_mixed env t
      in
      (* Ignore seen bounds to avoid infinite loops *)
      let upper_bounds =
        let ub = Env.get_upper_bounds env s in
        TySet.filter
          (fun ty ->
            (not (is_supportdyn_mixed ty))
            && (* Also ignore ~T if we've seen T *)
            not
              (TySet.mem
                 (snd (Typing_dynamic_utils.strip_dynamic env ty))
                 ctx.generics_seen))
          ub
        |> TySet.elements
      in
      (* TODO: should we be taking the intersection of the errors? *)
      let ((env, ty_err_opt, cycles_upper_bounds), resl) =
        Phase.list_map_env_err_cycles
          env
          upper_bounds
          ~f:(expand ctx)
          ~combine_ty_errs:Typing_error.multiple_opt
      in
      let res = intersect_results err_opt resl in
      ( (env, ty_err_opt, cycles_upper_bounds),
        update_class_name env ctx.id s res )
    | Tdependent (dep_ty, ty) ->
      let ctx =
        let root_kind = ConcreteClass in
        let base = Some (Option.value ctx.base ~default:root) in
        let allow_abstract = true in
        let abstract_as_tyvar_at_pos = None in
        { ctx with base; allow_abstract; abstract_as_tyvar_at_pos; root_kind }
      in
      let ((env, ty_err_opt, cycles), res) = expand ctx env ty in
      ( (env, ty_err_opt, cycles),
        update_class_name env ctx.id (DependentKind.to_string dep_ty) res )
    | Tintersection tyl ->
      (* Terrible hack (compatible with previous behaviour) that first attempts to project off the
       * non-type-variable conjunects. If this doesn't succeed, then try the type variable
       * conjunects, which will cause type-const constraints to be added to the type variables.
       *)
      let (tyl_vars, tyl_nonvars) =
        List.partition_tf tyl ~f:(fun t ->
            let (_env, t) = Env.expand_type env t in
            is_tyvar t)
      in
      (* TODO: should we be taking the intersection of the errors? *)
      let ((env, e1, cycles_tyl_nonvars), resl) =
        Phase.list_map_env_err_cycles
          env
          tyl_nonvars
          ~f:(fun env root ->
            let ety_env = { ctx.ety_env with this_ty = root } in
            let ctx = { ctx with ety_env } in
            expand ctx env root)
          ~combine_ty_errs:Typing_error.multiple_opt
      in
      let result = intersect_results err_opt resl in
      let ((env, e2, cycles_tyl_vars), res) =
        match result with
        | Missing _ ->
          (* TODO: should we be taking the intersection of the errors? *)
          let ((env, e2, cycles_tyl_vars), resl) =
            Phase.list_map_env_err_cycles
              env
              tyl_vars
              ~f:(fun env root ->
                let ety_env = { ctx.ety_env with this_ty = root } in
                let ctx = { ctx with ety_env } in
                expand ctx env root)
              ~combine_ty_errs:Typing_error.multiple_opt
          in
          (* TODO: should we be taking the intersection of the errors? *)
          ((env, e2, cycles_tyl_vars), intersect_results err_opt resl)
        | _ -> ((env, None, []), result)
      in
      let ty_err_opt =
        Option.merge e1 e2 ~f:(fun e1 e2 -> Typing_error.multiple [e1; e2])
      in
      ((env, ty_err_opt, cycles_tyl_vars @ cycles_tyl_nonvars), res)
    | Tunion tyl ->
      let ((env, ty_err_opt, cycles), resl) =
        Phase.list_map_env_err_cycles
          env
          tyl
          ~f:(fun env root ->
            let ety_env = { ctx.ety_env with this_ty = root } in
            let ctx = { ctx with ety_env } in
            expand ctx env root)
          ~combine_ty_errs:Typing_error.union_opt
      in
      let result = union_results err_opt resl in
      ((env, ty_err_opt, cycles), result)
    | Tvar n ->
      let ((env, err, cycles), ty) =
        Typing_subtype_tconst.get_tyvar_type_const
          env
          n
          ctx.id
          ~on_error:ctx.ety_env.on_error
      in
      ((env, err, cycles), Exact ty)
    | Taccess _
    | Tnonnull
    | Tprim _
    | Tshape _
    | Ttuple _
    | Tvec_or_dict _
    | Tfun _
    | Tdynamic
    | Toption _
    | Tlabel _
    | Tneg _
    | Tclass_ptr _ ->
      ((env, None, []), Missing err_opt)

(** Expands a type constant access like A::T to its definition. *)
let expand_with_env
    (ety_env : expand_env)
    env
    ?(ignore_errors = false)
    ?(as_tyvar_with_cnstr = None)
    root
    (id : pos_id)
    ~allow_abstract_tconst =
  let ((env, ty_err_opt, cycles), ty) =
    let ctx =
      {
        id;
        ety_env;
        base = None;
        generics_seen = TySet.empty;
        allow_abstract = allow_abstract_tconst;
        abstract_as_tyvar_at_pos = as_tyvar_with_cnstr;
        root_kind =
          GenericType
          (* Worst-case assumption; may be refined during [expand]. *);
      }
    in
    let ((env, e1, cycles), res) = expand ctx env root in
    let ((env, e2), ty) = type_of_result ~ignore_errors ctx env root res in
    let ty_err_opt = Option.merge e1 e2 ~f:Typing_error.both in
    ((env, ty_err_opt, cycles), ty)
  in
  let (env, ty) = Log.log_type_access ~level:1 root id (env, ty) in
  ((env, ty_err_opt, cycles), ty)

(* This is called with non-nested type accesses e.g. this::T1::T2 is
 * represented by (this, [T1; T2])
 *)
let referenced_typeconsts env ety_env (root, ids) =
  let ((env, e1), root) = Phase.localize ~ety_env env root in
  let (((_env, ty_errs), _ty), res) =
    List.fold
      ids
      ~init:(((env, Option.to_list e1), root), [])
      ~f:(fun (((env, ty_errs), root), acc) (pos, tconst) ->
        let (env, tyl) =
          Typing_utils.get_concrete_supertypes ~abstract_enum:true env root
        in
        let acc =
          List.fold tyl ~init:acc ~f:(fun acc ty ->
              let (env, ty) = Env.expand_type env ty in
              match get_node ty with
              | Tclass ((_, class_name), _, _) ->
                let ( >>= ) = Option.( >>= ) in
                Option.value
                  ~default:acc
                  ( Env.get_class env class_name |> Decl_entry.to_option
                  >>= fun class_ ->
                    Env.get_typeconst env class_ tconst >>= fun typeconst ->
                    Some ((typeconst.Typing_defs.ttc_origin, tconst, pos) :: acc)
                  )
              | _ -> acc)
        in
        let ((env, ty_err_opt, _cycles), root) =
          expand_with_env
            ety_env
            env
            ~as_tyvar_with_cnstr:None
            root
            (Pos_or_decl.of_raw_pos pos, tconst)
            ~allow_abstract_tconst:true
        in
        let ty_errs =
          Option.value_map
            ~default:ty_errs
            ~f:(fun e -> e :: ty_errs)
            ty_err_opt
        in
        (((env, ty_errs), root), acc))
  in
  (res, Typing_error.multiple_opt ty_errs)

(*****************************************************************************)
(* Exporting *)
(*****************************************************************************)

let () = Typing_utils.expand_typeconst_ref := expand_with_env
