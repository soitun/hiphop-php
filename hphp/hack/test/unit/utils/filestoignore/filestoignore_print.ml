open Hh_prelude

(**
This binary is part of a test to allow eye-balling the conversion from
FilesToIgnore.server_watch_spec to FilesToIgnore.watchman_server_expression_terms.

The actual conversion is performed by
FilesToIgnore.watchman_expression_term_of_spec.

This binary just exists to allow us to use test/verify.py to compare the actual
conversion result (as JSON) against the expected ones: To this end, it ignores
its command line arguments and just pretty-prints
FilesToIgnore.watchman_server_expression_terms.
*)

let main () =
  let pretty =
    Yojson.Safe.pretty_to_string
      (`List FilesToIgnore.watchman_server_expression_terms)
  in
  Printf.printf "%s" pretty

let _ = main ()
