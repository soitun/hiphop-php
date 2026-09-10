(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
open Hh_prelude

let usage =
  {|
Codemod the entire repo to add missing `__NeedsConcrete` attributes (including generated files) in order to get stats.
Best invoked with ./run.sh
Currently useful for "what-if" analysis (getting numbers).
|}

type args = {
  root: string;
  errors_file: string;
}

let parse_args_exn () : args =
  let errors_file = ref None in
  let root = ref None in
  let () =
    Arg.parse
      [
        ( "--errors",
          Arg.String (fun s -> errors_file := Some s),
          "File containing errors to codemod, produce by hh --re --json" );
        ("--root", Arg.String (fun s -> root := Some s), "Where's your WWW?");
      ]
      (fun _ -> ())
      usage
  in
  let errors_file = Option.value_exn ~message:usage !errors_file in
  let root = Option.value_exn ~message:usage !root in
  let () = Printf.printf "Codemodding %s from %s\n" root errors_file in
  { root; errors_file }

let () =
  let { root; errors_file } = parse_args_exn () in
  let () = Relative_path.set_path_prefix Relative_path.Root (Path.make root) in
  let () = Printf.printf "Streaming warnings from JSON\n%!" in
  let {
    Codemod_sa_warning.warnings;
    input_diagnostics;
    matching_diagnostics;
    codemoddable_diagnostics;
    unique_targets;
  } =
    match Codemod_sa_warning.parse_warnings_json_file errors_file with
    | Ok result -> result
    | Error message ->
      Printf.eprintf "%s\n%!" message;
      exit 2
  in
  let work_count = Relative_path.Map.cardinal warnings in
  Printf.printf
    "input_diagnostics=%d matching_diagnostics=%d codemoddable_diagnostics=%d unique_targets=%d\n%!"
    input_diagnostics
    matching_diagnostics
    codemoddable_diagnostics
    unique_targets;
  Printf.printf "Rewriting %d files sequentially\n%!" work_count;
  let on_path path warnings_for_path =
    let file = Relative_path.to_absolute path in
    let code = Disk.cat file in
    let (contents, edits) =
      Codemod_sa_rewrite.rewrite path warnings_for_path code
    in
    if edits > 0 then Disk.write_file ~file ~contents;
    edits
  in
  let rewrites =
    Relative_path.Map.fold
      warnings
      ~init:0
      ~f:(fun path warnings_for_path rewrites ->
        rewrites + on_path path warnings_for_path)
  in
  Printf.printf
    "SAFE_ABSTRACT_SUMMARY\tinput_diagnostics=%d\tmatching_diagnostics=%d\tcodemoddable_diagnostics=%d\tunique_targets=%d\tfiles=%d\trewrites=%d\n%!"
    input_diagnostics
    matching_diagnostics
    codemoddable_diagnostics
    unique_targets
    work_count
    rewrites;
  if codemoddable_diagnostics > 0 && rewrites = 0 then begin
    Printf.eprintf
      "Found %d codemoddable diagnostics but made no rewrites\n%!"
      codemoddable_diagnostics;
    exit 2
  end
