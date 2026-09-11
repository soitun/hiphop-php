(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude

let main (env : Client_start.env) : Exit_status.t Lwt.t =
  HackEventLogger.client_restart
    ~data:
      (Config_file.Utils.parse_hhconfig_and_hh_conf_to_json
         ~root:env.Client_start.root
         ~server_local_config_path:ServerLocalConfigLoad.system_config_path);
  if
    Monitor_connection.server_exists
      (ServerFiles.lock_file env.Client_start.root)
  then
    Client_stop.kill_server env.Client_start.root env.Client_start.from
  else
    Printf.eprintf
      "Warning: no server to restart for %s\n%!"
      (Path.to_string env.Client_start.root);
  Client_start.start_server env;
  Lwt.return Exit_status.No_error
