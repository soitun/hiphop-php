(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
type outline = string Symbol_definition.t list

val summarize_property :
  string -> ('a, 'b) Aast.class_var -> Relative_path.t Symbol_definition.t

val summarize_class_const :
  string -> ('a, 'b) Aast.class_const -> Relative_path.t Symbol_definition.t

val summarize_class_typeconst :
  string ->
  ('a, 'b) Aast.class_typeconst_def ->
  Relative_path.t Symbol_definition.t

val summarize_method :
  string -> ('a, 'b) Aast.method_ -> Relative_path.t Symbol_definition.t

val summarize_class :
  ('a, 'b) Aast.class_ ->
  no_children:bool ->
  Relative_path.t Symbol_definition.t

val summarize_typedef :
  ('a, 'b) Aast.typedef -> Relative_path.t Symbol_definition.t

val summarize_fun : ('a, 'b) Aast.fun_def -> Relative_path.t Symbol_definition.t

val summarize_gconst :
  ('a, 'b) Aast.gconst -> Relative_path.t Symbol_definition.t

val summarize_local : string -> 'a Pos.pos -> 'a Symbol_definition.t

val summarize_module_def :
  ('a, 'b) Aast.module_def -> Relative_path.t Symbol_definition.t

val outline : Parser_options.t -> string -> string Symbol_definition.t list

val outline_entry_no_comments :
  popt:Parser_options.t ->
  entry:Provider_context.entry ->
  string Symbol_definition.t list

val print_def : ?short_pos:bool -> string -> string Symbol_definition.t -> unit

val print : ?short_pos:bool -> string Symbol_definition.t list -> unit
