(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Load configuration flags with the following effective precedence:
  - /etc/hh.conf
  - JustKnobs
  - SandboxExperiment
  - ExperimentsConfig
  - [overrides]

  [overrides] are also applied initially so dynamic configuration sources can
  observe them, then reapplied at the end to retain final precedence.

  If not [silent], then prints what it's doing to stderr. *)
val load :
  silent:bool ->
  current_version:Config_file_version.version ->
  current_rolled_out_flag_idx:int ->
  deactivate_saved_state_rollout:bool ->
  from:string ->
  overrides:Config_file_common.t ->
  Server_local_config.t

(** As [load], additionally applying [apply_dynamic_overrides] after JustKnobs
    and before SandboxExperiment. The callback observes the initially applied
    [overrides], which are reapplied afterward. *)
val load_with_dynamic_overrides :
  apply_dynamic_overrides:
    (silent:bool -> Config_file_common.t -> Config_file_common.t) ->
  silent:bool ->
  current_version:Config_file_version.version ->
  current_rolled_out_flag_idx:int ->
  deactivate_saved_state_rollout:bool ->
  from:string ->
  overrides:Config_file_common.t ->
  Server_local_config.t

(** Load ServerLocalConfig from already-parsed config contents.
    This is intended for testing, bypassing file reads and overrides. *)
val load_from_config :
  silent:bool ->
  current_version:Config_file_version.version ->
  current_rolled_out_flag_idx:int ->
  deactivate_saved_state_rollout:bool ->
  Config_file_common.t ->
  Server_local_config.t

module For_test : sig
  val apply_overrides_in_order :
    silent:bool ->
    config:Config_file_common.t ->
    overrides:Config_file_common.t ->
    apply_justknobs_overrides:(Config_file_common.t -> Config_file_common.t) ->
    apply_dynamic_overrides:(Config_file_common.t -> Config_file_common.t) ->
    apply_sandbox_experiment_overrides:
      (Config_file_common.t -> Config_file_common.t) ->
    apply_experiments_config_overrides:
      (Config_file_common.t -> string * Config_file_common.t) ->
    string * Config_file_common.t
end

val to_rollout_flags : Server_local_config.t -> Hack_event_logger.rollout_flags

val system_config_path : string

val default : Server_local_config.t
