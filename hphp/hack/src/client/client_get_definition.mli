(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val print_readable :
  ?short_pos:bool ->
  ('a Symbol_occurrence.t * string Symbol_definition.t option) list ->
  unit

val go :
  (string Symbol_occurrence.t * string Symbol_definition.t option) list ->
  bool ->
  unit
