(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val identify_symbol_response_to_json :
  (string SymbolOccurrence.t * string SymbolDefinition.t option) list ->
  Yojson.Safe.t

val print_json : Yojson.Safe.t -> unit

val tast_holes_response_to_json :
  print_file:bool ->
  (string * string * string * string * Pos.t) list ->
  Yojson.Safe.t

val outline_response_to_json : string SymbolDefinition.t list -> Yojson.Safe.t

val highlight_references_response_to_json :
  Ide_api_types.range list -> Yojson.Safe.t

val infer_type_error_response_to_json :
  string option * string option * string option * string option -> Yojson.Safe.t

val infer_type_response_to_json : string option * string option -> Yojson.Safe.t
