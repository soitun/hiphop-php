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

module List = Base.List

type json =
  | JSON_Object of (string * json) list
  | JSON_Array of json list
  | JSON_String of string
  | JSON_Number of string
  | JSON_Bool of bool
  | JSON_Null

let rec to_yojson (t : json) : Yojson.Safe.t =
  match t with
  | JSON_Null -> `Null
  | JSON_String s -> `String s
  | JSON_Number s ->
    (match Stdlib.int_of_string_opt s with
    | Some i -> `Int i
    | None ->
      (match Float.of_string_opt s with
      | Some d -> `Float d
      | None -> `Intlit s))
  | JSON_Bool b -> `Bool b
  | JSON_Array l -> `List (List.map l ~f:to_yojson)
  | JSON_Object l -> `Assoc (List.map l ~f:(fun (k, v) -> (k, to_yojson v)))
