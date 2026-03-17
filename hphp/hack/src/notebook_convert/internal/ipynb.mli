(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
open Hh_prelude

type cell =
  | Non_hack of {
      cell_type: string;
      contents: string;
      cell_bento_metadata: Yojson.Safe.t option;
          (** Corresponds to the cell-level "metadata" field in ipynb format.  *)
    }
  | Hack of {
      contents: string;
      cell_bento_metadata: Yojson.Safe.t option;
          (** Corresponds to the cell-level "metadata" field in ipynb format.  *)
    }

type t = {
  cells: cell list;
  kernelspec: Yojson.Safe.t;
}

(** Expects JSON matching schema:
* https://github.com/jupyter/nbformat/blob/main/nbformat/v4/nbformat.v4.schema.json
*)
val ipynb_of_json : Yojson.Safe.t -> (t, string) result

val ipynb_to_json : t -> Yojson.Safe.t

val ipynb_of_chunks :
  Notebook_chunk.t list ->
  kernelspec:Yojson.Safe.t ->
  (t, Notebook_convert_error.t) result
