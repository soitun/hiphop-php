(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val apply_patches : ServerRenameTypes.patch list -> unit

val patches_to_json_string : ServerRenameTypes.patch list -> string

val print_patches_json : ServerRenameTypes.patch list -> unit

val go :
  (unit -> Client_connect.conn Lwt.t) ->
  desc:string ->
  Client_env.client_check_env ->
  Client_env.rename_mode ->
  before:string ->
  after:string ->
  unit Lwt.t

val go_ide_from_patches : ServerRenameTypes.patch list -> bool -> unit
