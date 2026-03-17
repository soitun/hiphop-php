(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
val notebook_to_hack :
  notebook_number:string ->
  header:string ->
  Yojson.Safe.t ->
  (string, string) Result.t
