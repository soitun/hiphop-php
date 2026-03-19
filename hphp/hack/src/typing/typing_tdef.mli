(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** The result of looking up and expanding a single typedef.
    [Expanded] contains the raw (unsubstituted) body as a [decl_ty],
    [Opaque] contains the upper-bound constraint.
    Both include the type parameters needed by the caller for substitution. *)
type expansion =
  | Expanded of {
      tparams: Typing_defs.decl_tparam list;
      body: Typing_defs.decl_ty;
      cstr: Typing_defs.decl_ty;
    }
  | Opaque of {
      tparams: Typing_defs.decl_tparam list;
      cstr: Typing_defs.decl_ty;
    }
  | Cyclic of { td_type_assignment: Typing_defs.typedef_type_assignment }

type expansion_result = {
  ety_env: Typing_defs.expand_env;
  cycles: Type_expansions.cycle_reporter list;
  expansion: expansion;
}

(** [expand_typedef ety_env env r name] looks up the type alias [name]
    and returns a decl-level expansion result. For visible typedefs, the
    result contains the raw (unsubstituted) body; for opaque ones, the
    upper-bound constraint. The caller handles substitution and localization.

    Expansion behavior is controlled through [ety_env.visibility_behavior]:
    - [Always_expand_newtype] forces expansion of all type aliases
    - [Expand_visible_newtype_only] only expands visible aliases (default) *)
val expand_typedef :
  Typing_defs.expand_env ->
  Typing_env_types.env ->
  Typing_reason.t ->
  string ->
  expansion_result

(** Recursively expand all typedef layers at the decl level.
    Internally sets [Always_expand_newtype] and calls [expand_typedef]
    in a loop, performing decl-level substitution at each step,
    until a non-typedef [Tapply] (i.e., a class) or a
    non-[Tapply] type is reached. *)
val force_expand_decl_typedef :
  Typing_defs.expand_env ->
  Typing_env_types.env ->
  Typing_defs.decl_ty ->
  Typing_defs.expand_env * Typing_defs.decl_ty
