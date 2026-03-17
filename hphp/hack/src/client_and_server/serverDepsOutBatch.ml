open Hh_prelude
open ServerDepsUtil

let build_json_def def =
  `Assoc
    [
      ( "kind",
        `String (SymbolDefinition.string_of_kind def.SymbolDefinition.kind) );
      ("name", `String (SymbolDefinition.full_name def));
      ( "position",
        Pos.to_absolute def.SymbolDefinition.pos |> Pos.multiline_json );
    ]

let rec build_json_entry
    ~(ctx : Provider_context.t)
    ~(entry : Provider_context.entry)
    ~(total_occ_list : Relative_path.t SymbolOccurrence.t list)
    ~(get_def :
       Relative_path.t SymbolOccurrence.t ->
       Relative_path.t SymbolDefinition.t option)
    (occ : Relative_path.t SymbolOccurrence.t) : Yojson.Safe.t =
  let open SymbolOccurrence in
  let def_opt = get_def occ in
  let depends_json =
    match def_opt with
    | None -> `String "None"
    | Some def ->
      if Option.is_none occ.is_declaration then
        build_json_def def
      else
        let body_list = body_symbols ~ctx ~entry total_occ_list occ def in
        `List
          (List.map
             body_list
             ~f:(build_json_entry ~ctx ~entry ~total_occ_list ~get_def))
  in
  `Assoc
    [
      ("kind", `String (kind_to_string occ.type_));
      ("name", `String occ.name);
      ( "declaration",
        match occ.is_declaration with
        | None -> `Null
        | Some p -> Pos.to_absolute p |> Pos.multiline_json );
      ("position", Pos.to_absolute occ.pos |> Pos.multiline_json);
      ("depends_on", depends_json);
    ]

let interesting_occ (occ : Relative_path.t SymbolOccurrence.t) : bool =
  let open SymbolOccurrence in
  match occ.type_ with
  | Keyword _
  | LocalVar
  | BuiltInType _
  | BestEffortArgument _ ->
    false
  | _ -> true

let go_json :
    Provider_context.t -> (string * int * int) list -> Yojson.Safe.t list =
 fun server_ctx pos_list ->
  let json_of_symbols acc_ctx_in (file, line, column) =
    let (acc_ctx_out, entry, _, get_def) = get_def_setup acc_ctx_in file in
    let total_occ_list =
      IdentifySymbolService.all_symbols_ctx ~ctx:acc_ctx_out ~entry
      |> List.filter ~f:interesting_occ
    in
    let symbols = List.filter total_occ_list ~f:(is_target line column) in
    let json =
      `List
        (List.map
           symbols
           ~f:
             (build_json_entry ~ctx:acc_ctx_out ~entry ~total_occ_list ~get_def))
    in
    (acc_ctx_out, json)
  in

  let (_, json_list) =
    List.fold_map pos_list ~init:server_ctx ~f:json_of_symbols
  in
  json_list

let go (ctx : Provider_context.t) (pos_list : (string * int * int) list) :
    string list =
  let jsons = go_json ctx pos_list in
  List.map jsons ~f:Hh_json_helpers.Out.pretty_to_string
