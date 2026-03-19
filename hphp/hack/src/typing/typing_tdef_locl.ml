(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Locl-level typedef expansion: bridges [Typing_tdef] (decl-level expansion)
    and [Typing_phase] (localization) without requiring either to depend on the
    other. *)

open Hh_prelude
open Typing_defs
module Env = Typing_env
module SN = Naming_special_names
module Subst = Decl_subst
module Phase = Typing_phase

(** Expand a [locl_ty], smashing typedef abstraction and collecting a trail
    of where the typedefs come from. Peels [Tnewtype] layers one at a time,
    expanding at the decl level and localizing at each step.

    /!\ This only does something if passed a [Tnewtype]. *)
let force_expand_typedef ~ety_env env (t : locl_ty) =
  let rec aux e1 ety_env env t =
    let default () =
      ((env, e1), t, Type_expansions.def_positions ety_env.type_expansions)
    in
    match deref t with
    | (_, Tnewtype (x, _, _)) when String.equal SN.Classes.cEnumClassLabel x ->
      (* Labels are Resources at runtime, so we don't want to force them
       * to string. MemberOf on the other hand are "typed alias" on the
       * underlying type so it's ok to force them. So we only special case
       * Labels here *)
      default ()
    | (r, Tnewtype (x, argl, _))
      when not (Env.is_enum env x || Env.is_enum_class env x) ->
      let ety_env_expand =
        { ety_env with visibility_behavior = Always_expand_newtype }
      in
      let Typing_tdef.{ ety_env = result_ety_env; cycles = _; expansion } =
        Typing_tdef.expand_typedef ety_env_expand env r x
      in
      begin
        match expansion with
        | Typing_tdef.Expanded { tparams; body; cstr = _ } ->
          let substs = Subst.make_locl tparams argl in
          (* Use default visibility behavior for body localization, NOT
             Always_expand_newtype. This ensures opaque newtypes in the body
             remain as Tnewtype, allowing the outer force_expand loop to
             handle them one at a time and accumulate the expansion trail. *)
          let ety_env_inner =
            {
              result_ety_env with
              substs;
              under_type_constructor = false;
              visibility_behavior = default_visibility_behaviour;
            }
          in
          let ((env, e2), ty) =
            Phase.localize ~ety_env:ety_env_inner env body
          in
          let ty = with_reason ty r in
          let err = Option.merge e1 e2 ~f:Typing_error.both in
          aux err result_ety_env env ty
        | Typing_tdef.Opaque _
        | Typing_tdef.Cyclic _ ->
          default ()
      end
    | _ -> default ()
  in
  aux None ety_env env t
