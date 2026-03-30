(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

type repo_transition =
  | State_enter of Hg.Rev.t
  | State_leave of Hg.Rev.t
  | Changed_merge_base of Hg.Rev.t
[@@deriving show]

type t

(** Non-blocking. The refs are updated whenever the file watcher informs us about entering or
   leaving hg.update and hg.transaction states *)
val get_change :
  t ->
  is_in_hg_update_state:bool ref ->
  is_in_hg_transaction_state:bool ref ->
  repo_transition option

(** If false (and no other changes happen),
    the next call of `get_change` returns `None` *)
val has_more_messages : t -> bool

val init : watchman_debug_logging:bool -> Path.t -> t option
