(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Heavy commands should be run from www and provide a root.
  They can benefit from logging and config parsing *)
type heavy_command =
  | CCheck of Client_env.client_check_env
  | CStart of Client_start.env
  | CStop of Client_stop.env
  | CRestart of Client_start.env
  | CLsp of Client_lsp.args
  | CSavedStateProjectMetadata of Client_env.client_check_env
  | CDownloadSavedState of Client_download_saved_state.env
  | CRage of ClientRage.env

(** Light commands should not use any of the following
  - Hh_logger, HackEventLogger
  - configs
  - root or relative paths *)
type light_command = CDecompressZhhdg of Client_decompress_zhhdg.env

type command =
  | With_config of heavy_command
  | Without_config of light_command

type command_keyword =
  | CKCheck
  | CKStart
  | CKStop
  | CKRestart
  | CKNone
  | CKLsp
  | CKSavedStateProjectMetadata
  | CKDownloadSavedState
  | CKRage
  | CKDecompressZhhdg

let get_custom_telemetry_data command =
  match command with
  | CCheck { Client_env.custom_telemetry_data; _ }
  | CStart { Client_start.custom_telemetry_data; _ }
  | CRestart { Client_start.custom_telemetry_data; _ } ->
    custom_telemetry_data
  | CStop _
  | CLsp _
  | CSavedStateProjectMetadata _
  | CDownloadSavedState _
  | CRage _ ->
    []

let command_name = function
  | CKCheck -> "check"
  | CKStart -> "start"
  | CKStop -> "stop"
  | CKRestart -> "restart"
  | CKLsp -> "lsp"
  | CKSavedStateProjectMetadata -> "saved-state-project-metadata"
  | CKDownloadSavedState -> "download-saved-state"
  | CKRage -> "rage"
  | CKDecompressZhhdg -> "decompress-zhhdg"
  | CKNone -> ""

let name_camel_case_heavy = function
  | CCheck _ -> "Check"
  | CStart _ -> "Start"
  | CStop _ -> "Stop"
  | CRestart _ -> "Restart"
  | CLsp _ -> "Lsp"
  | CSavedStateProjectMetadata _ -> "SavedStateProjectMetadata"
  | CDownloadSavedState _ -> "DownloadSavedState"
  | CRage _ -> "Rage"

let name_camel_case_light = function
  | CDecompressZhhdg _ -> "DecompressZhhdg"
