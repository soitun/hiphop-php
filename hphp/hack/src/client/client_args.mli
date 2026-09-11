(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val parse_args : from_default:string -> Client_command.command

val root : Client_command.heavy_command -> Path.t

val from : Client_command.heavy_command -> string

val is_interactive : Client_command.heavy_command -> bool

val config : Client_command.heavy_command -> (string * string) list option

val dump_config : Client_command.heavy_command -> bool
