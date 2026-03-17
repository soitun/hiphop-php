(** State_enter and State_leave events contains a JSON blob specifying
 * the revision we are moving to. This gets it. *)
let rev_in_state_change (json : Yojson.Safe.t) : Hg.Rev.t option =
  Hh_json_helpers.Access.(
    return json >>= get_string "rev" |> function
    | Error _ ->
      let () =
        Hh_logger.log
          "Watchman_utils failed to get rev in json: %s"
          (* unsorted: watchman debug logging, not snapshot-tested *)
          (Yojson.Safe.to_string json)
      in
      None
    | Ok (v, _) -> Some (Hg.Rev.of_string v))

let merge_in_state_change (json : Yojson.Safe.t) =
  Hh_json_helpers.Access.(
    return json >>= get_bool "merge" |> function
    | Error _ ->
      let () =
        Hh_logger.log
          "Watchman_utils failed to get merge in json: %s"
          (* unsorted: watchman debug logging, not snapshot-tested *)
          (Yojson.Safe.to_string json)
      in
      None
    | Ok (v, _) -> Some v)
