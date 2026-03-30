(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude

type repo_transition =
  | State_enter of Hg.Rev.t
  | State_leave of Hg.Rev.t
  | Changed_merge_base of Hg.Rev.t * Watchman.clock
[@@deriving show]

type t = Watchman of Watchman.watchman_instance ref

let get_change_watchman
    watchman_ref ~is_in_hg_update_state ~is_in_hg_transaction_state =
  let (watchman, change) = Watchman.get_changes !watchman_ref in
  watchman_ref := watchman;
  match change with
  | Watchman.Watchman_unavailable
  | Watchman.Watchman_synchronous _ ->
    None
  | Watchman.Watchman_pushed (Watchman.Changed_merge_base (rev, _files, clock))
    ->
    let () = Hh_logger.log "Changed_merge_base: %s" (Hg.Rev.to_string rev) in
    Some (Changed_merge_base (rev, clock))
  | Watchman.Watchman_pushed (Watchman.State_enter (state, json))
    when String.equal state "hg.update" ->
    is_in_hg_update_state := true;
    Option.(
      json >>= Watchman_utils.rev_in_state_change >>= fun hg_rev ->
      Hh_logger.log "State_enter: %s" (Hg.Rev.to_string hg_rev);
      Some (State_enter hg_rev))
  | Watchman.Watchman_pushed (Watchman.State_leave (state, json))
    when String.equal state "hg.update" ->
    is_in_hg_update_state := false;
    Option.(
      json >>= Watchman_utils.rev_in_state_change >>= fun hg_rev ->
      Hh_logger.log "State_leave: %s" (Hg.Rev.to_string hg_rev);
      Some (State_leave hg_rev))
  | Watchman.Watchman_pushed (Watchman.State_enter (state, _))
    when String.equal state "hg.transaction" ->
    is_in_hg_transaction_state := true;
    None
  | Watchman.Watchman_pushed (Watchman.State_leave (state, _))
    when String.equal state "hg.transaction" ->
    is_in_hg_transaction_state := false;
    None
  | Watchman.Watchman_pushed (Watchman.Files_changed _)
  | Watchman.Watchman_pushed (Watchman.State_enter _)
  | Watchman.Watchman_pushed (Watchman.State_leave _) ->
    None

let get_change t ~is_in_hg_update_state ~is_in_hg_transaction_state =
  let (Watchman watchman_ref) = t in
  get_change_watchman
    watchman_ref
    ~is_in_hg_update_state
    ~is_in_hg_transaction_state

let has_more_messages t =
  let (Watchman watchman_ref) = t in
  match Watchman.get_reader !watchman_ref with
  | None -> false
  | Some reader -> Buffered_line_reader.is_readable reader

let init_watchman ~watchman_debug_logging root =
  let watchman =
    (* The informant is only interested in hg state changes and Changed_merge_base notifications,
       but not actual files.
       Therefore, we use an expression term for our subscription that matches no files. *)
    let expression_terms =
      [Hh_json_helpers.AdhocJsonHelpers.strlist ["false"]]
    in
    Watchman.init
      {
        Watchman.subscribe_mode = Some Watchman.Scm_aware;
        init_timeout = Watchman.Explicit_timeout 30.;
        expression_terms;
        debug_logging = watchman_debug_logging;
        (* Should also take an arg *)
        sockname = None;
        subscription_prefix = "hh_informant_watcher";
        roots = [root];
      }
      ()
  in
  match watchman with
  | Some watchman_env ->
    Some (Watchman (ref (Watchman.Watchman_alive watchman_env)))
  | None -> None

let init = init_watchman
