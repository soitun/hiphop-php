(*
 * Copyright (c) 2018, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

type env

type t = env

exception Not_in_class

(*
* All+only functions in Typing_env that are safe in tasts checks
* should hereby be available as Tast_env.get_class, Tast_env.get_typedef, etc.
*
* See comment in typing_env.mli Expose_to_tast_env on when+how to add things
*)
include
  module type of Typing_env.Expose_to_tast_env
    (* forget that Tast_env.env = Typing_env_types.env *)
    with type env := env

type class_or_typedef_result =
  | ClassResult of Decl_provider.class_decl
  | TypedefResult of Typing_defs.typedef_type

(** Return a string representation of the given type using Hack-like syntax. *)
val print_ty : env -> Typing_defs.locl_ty -> string

val print_decl_ty : ?msg:bool -> env -> Typing_defs.decl_ty -> string

val print_error_ty :
  ?ignore_dynamic:bool -> env -> Typing_defs.locl_ty -> string

val print_hint : env -> Aast.hint -> string

(** Return a string representation of the given type using Hack-like syntax,
    formatted with limited width and line breaks, including additional
    information from the {!SymbolOccurrence.t} and (if provided)
    {!SymbolDefinition.t}. *)
val print_ty_with_identity :
  env ->
  Typing_defs.locl_ty ->
  'b SymbolOccurrence.t ->
  'b SymbolDefinition.t option ->
  string

val print_decl_ty_with_identity :
  env ->
  Typing_defs.decl_ty ->
  'b SymbolOccurrence.t ->
  'b SymbolDefinition.t option ->
  string

(** Return a JSON representation of the given type. *)
val ty_to_json :
  env -> ?show_like_ty:bool -> Typing_defs.locl_ty -> Hh_json.json

(** Convert a JSON representation of a type back into a locl-phase type. *)
val json_to_locl_ty :
  ?keytrace:Hh_json.Access.keytrace ->
  Provider_context.t ->
  Hh_json.json ->
  (Typing_defs.locl_ty, Typing_defs.deserialization_error) result

(** Return the type of the enclosing class definition.
    When not in a class definition, raise {!Not_in_class}. *)
val get_self_ty_exn : env -> Tast.ty

val get_class_or_typedef :
  env -> Decl_provider.type_key -> class_or_typedef_result Decl_entry.t

val outside_expr_tree : env -> env

(** Return the {!Provider_context.t} with which this TAST was checked. *)
val get_ctx : env -> Provider_context.t

val get_file : env -> Relative_path.t

val get_deps_mode : env -> Typing_deps_mode.t

(* Return the {!Relative_path.t} of the file the env is from *)

(** Expand a type variable ({!Typing_defs.Tvar}) to the type it refers to. *)
val expand_type : env -> Tast.ty -> env * Tast.ty

(** Eliminate type variables ({!Typing_defs.Tvar}) in the given type by
    recursively replacing them with the type they refer to. *)
val fully_expand : env -> Tast.ty -> Tast.ty

(** Strip ~ from type *)
val strip_dynamic : env -> Tast.ty -> Tast.ty

type is_disjoint_result =
  | NonDisjoint  (** Types are possibly overlapping *)
  | Disjoint  (** Types are definitely non-overlapping *)
  | DisjointIgnoringDynamic of Tast.ty * Tast.ty
      (** Types overlap in dynamic, but as like-stripped types are disjoint *)

(** Are types disjoint? If is_dynamic_call is true, then
    we are checking a dynamic call through a like type, so
    result cannot be precise (i.e. won't return Disjoint) *)
val is_disjoint :
  is_dynamic_call:bool -> env -> Tast.ty -> Tast.ty -> is_disjoint_result

(** Strip supportdyn from type, return whether it was there or not *)
val strip_supportdyn : env -> Tast.ty -> bool * Tast.ty

val get_underlying_function_type :
  env -> Tast.ty -> (Typing_reason.t * Tast.ty Typing_defs.fun_type) option

(** Types that can have methods called on them. Usually a class but
    also includes dynamic types *)
type receiver_identifier =
  | RIclass of string * Tast.ty list
  | RIdynamic
  | RIerr
  | RIany

(** Given some class type or unresolved union of class types, return the
    identifiers of all receivers the type may represent. *)
val get_receiver_ids : env -> Tast.ty -> receiver_identifier list

(** Given some class type or unresolved union of class types, return the
    identifiers of all classes the type may represent. *)
val get_class_ids : env -> Tast.ty -> string list

(** For an `EnumClass\Label<T, _>` returns `T`. *)
val get_label_receiver_ty : env -> Tast.ty -> env * Tast.ty

(** Intersect type with nonnull *)
val intersect_with_nonnull : env -> Pos_or_decl.t -> Tast.ty -> env * Tast.ty

(** Get the "as" constraints from an abstract type or generic parameter, or
    return the type itself if there is no "as" constraint. In the case of a
    generic parameter whose "as" constraint is another generic parameter, repeat
    the process until a type is reached that is not a generic parameter. Don't
    loop on cycles. (For example, function foo<Tu as Tv, Tv as Tu>(...))
    The abstract_enum flag controls whether arraykey bound enums are considered
    abstract, or as arraykey bound.
     *)
val get_concrete_supertypes :
  ?expand_supportdyn:bool ->
  abstract_enum:bool ->
  env ->
  Tast.ty ->
  env * Tast.ty list

(** Return {true} if the given {Decl_provider.class_decl} (referred to by the given
    {class_id_}, if provided) allows the current class (the one returned by
    {!get_self}) to access its members with the given {visibility}. *)
val is_visible :
  is_method:bool ->
  env ->
  Typing_defs.ce_visibility * bool ->
  Nast.class_id_ option ->
  Decl_provider.class_decl ->
  bool

(** Assert that the types of values involved in a strict (non-)equality
    comparison are compatible; e.g., that the types are not statically
    known to be disjoint, in which case the comparison will always return
    true or false. *)
val assert_nontrivial :
  Pos.t -> Ast_defs.bop -> env -> Tast.ty -> Tast.ty -> unit

(** Return the declaration-phase type the given hint represents. *)
val hint_to_ty : env -> Aast.hint -> Typing_defs.decl_ty

val localize :
  env -> Typing_defs.expand_env -> Typing_defs.decl_ty -> env * Tast.ty

val localize_hint_for_refinement : env -> Aast.hint -> env * Tast.ty

(** Return [Result.Ok ()] if the current hint is supported in the new
  constraint-based refinement logic. Otherwise returns [Result.Err string],
  where [string] is the reason why the hint isn't supported. *)
val supports_new_refinement : env -> Aast.hint -> (unit, string) Result.t

(** Transforms a declaration phase type ({!Typing_defs.decl_ty})
    into a localized type ({!Typing_defs.locl_ty} = {!Tast.ty}).
    Performs no substitutions of generics and initializes the late static bound
    type ({!Typing_defs.Tthis}) to the current class type (the type returned by
    {!get_self}).

    This is mostly provided as legacy support for {!AutocompleteService}, and
    should not be considered a general mechanism for transforming a {decl_ty} to
    a {!Tast.ty}.

    {!quiet} silences certain errors because those errors have already fired
    and/or are not appropriate at the time we call localize.
    *)
val localize_no_subst :
  env -> ignore_errors:bool -> Typing_defs.decl_ty -> env * Tast.ty

(** Get the upper bounds of the type parameter with the given name.
  FIXME: This function cannot return correct bounds at this time, because
  during TAST checks, the Next continuation in the typing environment (which stores
  information about type parameters) is gone.
 *)
val get_upper_bounds : env -> string -> Type_parameter_env.tparam_bounds

(** Get the reification of the type parameter with the given name. *)
val get_reified : env -> string -> Aast.reify_kind

(** Get whether the type parameter supports testing with is/as. *)
val get_enforceable : env -> string -> bool

(** Indicates whether the type parameter with the given name is <<__Newable>>. *)
val get_newable : env -> string -> bool

val fresh_type : env -> Pos.t -> env * Typing_defs.locl_ty

(** Return whether the type parameter with the given name was implicity created
    as part of an `instanceof`, `is`, or `as` expression (instead of being
    explicitly declared in code by the user). *)
val is_fresh_generic_parameter : string -> bool

(** Assert that one type is a subtype of another, resolving unbound type
    variables in both types (if any), with {!env} reflecting the new state of
    these type variables. Produce an error if they cannot be subtypes. *)
val assert_subtype :
  Pos.t ->
  Typing_reason.ureason ->
  env ->
  Tast.ty ->
  Tast.ty ->
  Typing_error.Callback.t ->
  env

(** Return {true} when the first type is a subtype of the second type
    regardless of the values of unbound type variables in both types (if any). *)
val is_sub_type : env -> Tast.ty -> Tast.ty -> bool

val is_dynamic_aware_sub_type : env -> Tast.ty -> Tast.ty -> bool

(** Return {true} when the first type can be considered a subtype of the second
    type after resolving unbound type variables in both types (if any). *)
val can_subtype : env -> Tast.ty -> Tast.ty -> bool

(** Return {true} when the first type is a subtype of the second type. There is
    no type T such that for all T', T <: T' and T' <: T (which is the case for Tany
    and Terr in `can_subtype`) *)
val is_sub_type_for_union : env -> Tast.ty -> Tast.ty -> bool

(** Simplify unions in a type. *)
val simplify_unions : env -> Tast.ty -> env * Tast.ty

(** Simplify intersections in a type. *)
val simplify_intersections : env -> Tast.ty -> env * Tast.ty

val as_bounds_to_non_intersection_type :
  env -> Tast.decl_ty list -> Tast.ty option

(** Union a list of types. *)
val union_list : env -> Typing_reason.t -> Tast.ty list -> env * Tast.ty

(** Returns (class_name, tconst_name, tconst_reference_position) for each type
    constant referenced in the type access path. *)
val referenced_typeconsts :
  env -> Aast.hint -> Aast.sid list -> (string * string * Pos.t) list

(** Return an {!env} for which {!is_static} will return {true}.
    If you are using {!Tast_visitor}, you should have no need of this. *)
val set_static : env -> env

(** Return an {!env} for which {!val_kind} is set to the second argument. *)
val set_val_kind : env -> Typing_defs.val_kind -> env

(** Returns the val_kind of the typing environment *)
val get_val_kind : env -> Typing_defs.val_kind

(** Returns an {!env} for which {!inside_constructor} is set to {true}.
    If you are using {!Tast_visitor}, you should have no need of this. *)
val set_inside_constructor : env -> env

(** Returns whether or not the typing environment is inside the
    constructor of a class *)
val get_inside_constructor : env -> bool

(** Returns a {!Decl_env.env} *)
val get_decl_env : env -> Decl_env.env

(** Construct an empty {!env}. Unlikely to be the best choice; prefer using
    {!Tast_visitor} or constructing an {!env} from a {!Tast.def}. *)
val empty : Provider_context.t -> env

(** Construct an {!env} from a toplevel definition. *)
val def_env : Provider_context.t -> Tast.def -> env

(** Construct an {!env} from a method definition and the {!env} of the context
    it appears in. *)
val restore_method_env : env -> Tast.method_ -> env

(** Construct an {!env} from a lambda definition and the {!env} of the context
    it appears in. *)
val restore_fun_env : env -> Tast.fun_ -> env

val typing_env_as_tast_env : Typing_env_types.env -> env

val tast_env_as_typing_env : env -> Typing_env_types.env

(** Verify that an XHP body expression is legal. *)
val is_xhp_child : env -> Pos.t -> Tast.ty -> bool * Typing_error.t option

val is_enum : env -> Decl_provider.type_key -> bool

val set_allow_wildcards : env -> env

val get_allow_wildcards : env -> bool

val fun_has_implicit_return : env -> bool

val fun_has_readonly : env -> bool

(** Check that the position is in the current decl and if it is, resolve
    it with the current file. *)
val fill_in_pos_filename_if_in_current_decl :
  env -> Pos_or_decl.t -> Pos.t option

(** Check if the environment is for a definition in a while that is a builtin. *)
val is_hhi : env -> bool

(** See {!Tast.check_status} to understand what this function returns from the
    environment. *)
val get_check_status : env -> Tast.check_status

val get_current_decl_and_file : env -> Pos_or_decl.ctx

val derive_instantiation :
  env ->
  Typing_defs.decl_ty ->
  Typing_defs.locl_ty ->
  env * Derive_type_instantiation.Instantiation.t

val add_typing_error : Typing_error.t -> env:env -> unit

val add_warning : env -> ('x, 'a) Typing_warning.t -> unit
