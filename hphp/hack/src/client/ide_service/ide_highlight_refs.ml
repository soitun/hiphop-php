(*
 * Copyright (c) 2016, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude

let get_target symbol =
  Symbol_occurrence.(
    let module Types = Server_command_types.Find_refs in
    let module SO = Symbol_occurrence in
    Find_refs_service.(
      match symbol.type_ with
      | SO.Class _ -> Some (IClass symbol.name)
      | SO.Function -> Some (IFunction symbol.name)
      | SO.Method (SO.ClassName class_name, member_name) ->
        Some (IMember (Subclasses_of class_name, Types.Method member_name))
      | SO.Property (SO.ClassName class_name, member_name)
      | SO.XhpLiteralAttr (class_name, member_name) ->
        Some (IMember (Subclasses_of class_name, Types.Property member_name))
      | SO.ClassConst (SO.ClassName class_name, member_name) ->
        Some (IMember (Subclasses_of class_name, Types.Class_const member_name))
      | SO.Typeconst (class_name, member_name) ->
        Some (IMember (Subclasses_of class_name, Types.Typeconst member_name))
      | SO.GConst -> Some (IGConst symbol.name)
      | _ -> None))

let highlight_symbol ctx entry pos symbol =
  let res =
    match get_target symbol with
    | Some target ->
      let results = Find_refs_service.find_refs_ctx ~ctx ~entry ~target in
      List.rev (List.map results ~f:(fun { name = _; pos } -> pos))
    | None
      when Symbol_occurrence.equal_kind
             symbol.Symbol_occurrence.type_
             Symbol_occurrence.LocalVar ->
      Server_find_locals.go ~ctx ~entry pos
    | None -> []
  in
  List.map res ~f:Ide_api_types.pos_to_range

let compare_starts r1 r2 =
  File_content.Position.compare r1.Ide_api_types.st r2.Ide_api_types.st

let go_quarantined
    ~(ctx : Provider_context.t) ~(entry : Provider_context.entry) pos :
    Ide_api_types.range list =
  let symbol_to_highlight =
    Identify_symbol_service.go_quarantined
      ~ctx
      ~entry
      pos
      ~use_declaration_spans:false
  in
  let results =
    List.fold symbol_to_highlight ~init:[] ~f:(fun acc s ->
        let stuff = highlight_symbol ctx entry pos s in
        List.append stuff acc)
  in
  let results = List.dedup_and_sort ~compare:compare_starts results in
  results
