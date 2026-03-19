(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude
open Typing_defs
open Utils
module Reason = Typing_reason
module Env = Typing_env
module Subst = Decl_subst
module MakeType = Typing_make_type

(** The result of looking up and expanding a single typedef.
    [Expanded] contains the raw (unsubstituted) body as a [decl_ty],
    [Opaque] contains the upper-bound constraint.
    Both include the type parameters needed by the caller for substitution. *)
type expansion =
  | Expanded of {
      tparams: decl_tparam list;
      body: decl_ty;
      cstr: decl_ty;
    }
  | Opaque of {
      tparams: decl_tparam list;
      cstr: decl_ty;
    }
  | Cyclic of { td_type_assignment: typedef_type_assignment }

type expansion_result = {
  ety_env: expand_env;
  cycles: Type_expansions.cycle_reporter list;
  expansion: expansion;
}

(** Compute the upper bound constraint, defaulting to [mixed]. *)
let get_typedef_constraint td =
  match td.td_as_constraint with
  | None -> MakeType.mixed (Reason.implicit_upper_bound (td.td_pos, "?nonnull"))
  | Some cstr -> cstr

(** Decide whether to expand a typedef based on visibility, and return the
    raw body or [None] for opaque. Does not perform substitution or cycle
    checking. *)
let resolve_typedef_body
    ~visibility_behavior env (x : string) (td : typedef_type) =
  if Env.should_expand_type_alias ~visibility_behavior env ~name:x td then
    let body =
      match td.td_type_assignment with
      | SimpleTypeDef (_vis, td_type) -> td_type
      | CaseType (variant, variants) ->
        Typing_utils.get_case_type_variants_as_type variant variants
    in
    Some body
  else
    None

(** [expand_typedef ety_env env r name] looks up the type alias [name]
    and returns a decl-level expansion result. For visible typedefs, the
    result contains the raw (unsubstituted) body; for opaque ones, the
    upper-bound constraint. The caller handles substitution and localization.

    Expansion behavior is controlled through [ety_env.visibility_behavior]:
    - [Always_expand_newtype] forces expansion of all type aliases
    - [Expand_visible_newtype_only] only expands visible aliases (default) *)
let expand_typedef ety_env env r (x : string) =
  let td = unsafe_opt @@ Decl_entry.to_option (Env.get_typedef env x) in
  match
    add_type_expansion_check_cycles
      ety_env
      {
        Type_expansions.name = Type_expansions.Expandable.Type_alias x;
        use_pos = Reason.to_pos r;
        def_pos = Some td.td_pos;
      }
  with
  | Error cycle ->
    {
      ety_env;
      cycles = [cycle];
      expansion = Cyclic { td_type_assignment = td.td_type_assignment };
    }
  | Ok ety_env ->
    let tparams = td.td_tparams in
    (match
       resolve_typedef_body
         ~visibility_behavior:ety_env.visibility_behavior
         env
         x
         td
     with
    | Some body ->
      let cstr = get_typedef_constraint td in
      { ety_env; cycles = []; expansion = Expanded { tparams; body; cstr } }
    | None ->
      let cstr = get_typedef_constraint td in
      { ety_env; cycles = []; expansion = Opaque { tparams; cstr } })

(** Recursively expand all typedef layers at the decl level.
    Internally sets [Always_expand_newtype] and calls [expand_typedef]
    in a loop, performing decl-level substitution at each step,
    until a non-typedef is reached. *)
let force_expand_decl_typedef ety_env env (ty : decl_ty) =
  let ety_env = { ety_env with visibility_behavior = Always_expand_newtype } in
  let rec aux ety_env ty =
    match get_node ty with
    | Tapply ((_, name), args) -> begin
      match Decl_entry.to_option (Env.get_typedef env name) with
      | Some _ ->
        let result = expand_typedef ety_env env (get_reason ty) name in
        begin
          match result.expansion with
          | Expanded { tparams; body; cstr = _ } ->
            let subst = Subst.make_decl tparams args in
            let body = Decl_instantiate.instantiate subst body in
            aux result.ety_env body
          | Opaque _
          | Cyclic _ ->
            (result.ety_env, ty)
        end
      | None -> (ety_env, ty)
    end
    | _ -> (ety_env, ty)
  in
  aux ety_env ty
