(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Expand a [locl_ty], smashing typedef abstraction and collecting a trail
    of where the typedefs come from. Peels [Tnewtype] layers one at a time,
    expanding at the decl level and localizing at each step.

    /!\ This only does something if passed a [Tnewtype]. *)
val force_expand_typedef :
  ety_env:Typing_defs.expand_env ->
  Typing_env_types.env ->
  Typing_defs.locl_ty ->
  (Typing_env_types.env * Typing_error.t option)
  * Typing_defs.locl_ty
  * Pos_or_decl.t list
