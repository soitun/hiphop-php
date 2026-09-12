(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val server_exists : string -> bool

val connect_once :
  tracker:Connection_tracker.t ->
  timeout:int ->
  terminate_monitor_on_version_mismatch:bool ->
  Path.t ->
  Monitor_rpc.handoff_options ->
  ( Stdlib.in_channel * out_channel * Server_command_types.server_specific_files,
    Monitor_utils.connection_error )
  result

val connect_and_shut_down :
  tracker:Connection_tracker.t ->
  Path.t ->
  (Monitor_utils.shutdown_result, Monitor_utils.connection_error) result
