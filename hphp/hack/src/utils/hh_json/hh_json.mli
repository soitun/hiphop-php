(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Legacy Hh_json type, only used by generated Glean code.
    New code should use Yojson.Safe.t directly. *)

type json =
  | JSON_Object of (string * json) list
  | JSON_Array of json list
  | JSON_String of string
  | JSON_Number of string
  | JSON_Bool of bool
  | JSON_Null

val to_yojson : json -> Yojson.Safe.t
