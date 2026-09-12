(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val go_ide :
  filename_for_logging:string ->
  content:string ->
  action:Server_format_types.ide_action ->
  options:Lsp.DocumentFormatting.formattingOptions ->
  Server_format_types.ide_result
