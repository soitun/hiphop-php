(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Asserter
open Asserter.Int_asserter

let test () =
  (* empty *)
  assert_equals 0 (Big_list.length Big_list.empty) "empty length";
  assert_list_equals [] (Big_list.as_list Big_list.empty) "empty list";

  (* create *)
  let t = Big_list.create [1; 2; 3] in
  assert_equals 3 (Big_list.length t) "create length";
  assert_list_equals [1; 2; 3] (Big_list.as_list t) "create list";

  (* cons *)
  let u = Big_list.cons 0 t in
  assert_equals 4 (Big_list.length u) "cons length";
  assert_list_equals [0; 1; 2; 3] (Big_list.as_list u) "cons list";

  (* is_empty *)
  Bool_asserter.assert_equals false (Big_list.is_empty t) "is_empty t";
  Bool_asserter.assert_equals true (Big_list.is_empty Big_list.empty) "is_empty";

  (* decons *)
  begin
    match Big_list.decons t with
    | None -> failwith "decons t expected some"
    | Some (hd, tl) ->
      assert_equals 1 hd "decons t hd";
      assert_equals 2 (Big_list.length tl) "decons t tl length";
      assert_list_equals [2; 3] (Big_list.as_list tl) "decons t tl list"
  end;
  begin
    match Big_list.decons Big_list.empty with
    | None -> ()
    | Some _ -> failwith "decons empty expected none"
  end;

  (* filter *)
  let is_even i = i mod 2 = 0 in
  let u = Big_list.filter t ~f:is_even in
  assert_equals 1 (Big_list.length u) "filter t length";
  assert_list_equals [2] (Big_list.as_list u) "filter t list";
  let u = Big_list.filter Big_list.empty ~f:is_even in
  assert_equals 0 (Big_list.length u) "filter empty length";
  assert_list_equals [] (Big_list.as_list u) "filter empty list";

  (* map *)
  let inc i = i + 1 in
  let u = Big_list.map t ~f:inc in
  assert_equals 3 (Big_list.length u) "map t length";
  assert_list_equals [2; 3; 4] (Big_list.as_list u) "map t list";
  let u = Big_list.map Big_list.empty ~f:inc in
  assert_equals 0 (Big_list.length u) "map empty length";
  assert_list_equals [] (Big_list.as_list u) "map empty list";

  (* append *)
  let u = Big_list.append [0; 9] t in
  assert_equals 5 (Big_list.length u) "append t length";
  assert_list_equals [0; 9; 1; 2; 3] (Big_list.as_list u) "append t list";

  (* rev_append *)
  let u = Big_list.rev_append [0; 9] t in
  assert_equals 5 (Big_list.length u) "rev_append t length";
  assert_list_equals [9; 0; 1; 2; 3] (Big_list.as_list u) "rev_append t list";

  (* rev *)
  let u = Big_list.rev t in
  assert_equals 3 (Big_list.length u) "rev t length";
  assert_list_equals [3; 2; 1] (Big_list.as_list u) "rev t list";

  (* split_n *)
  let (split, rest) = Big_list.split_n t 0 in
  assert_list_equals [] split "split t 0 split";
  assert_equals 3 (Big_list.length rest) "split t 0 rest length";
  assert_list_equals [1; 2; 3] (Big_list.as_list rest) "split t 0 rest list";
  let (split, rest) = Big_list.split_n t 1 in
  assert_list_equals [1] split "split t 1 split";
  assert_equals 2 (Big_list.length rest) "split t 1 rest length";
  assert_list_equals [2; 3] (Big_list.as_list rest) "split t 1 rest list";
  let (split, rest) = Big_list.split_n t 3 in
  assert_list_equals [1; 2; 3] split "split t 3 split";
  assert_equals 0 (Big_list.length rest) "split t 3 rest length";
  assert_list_equals [] (Big_list.as_list rest) "split t 3 rest list";
  let (split, rest) = Big_list.split_n t 4 in
  assert_list_equals [1; 2; 3] split "split t 4 split";
  assert_equals 0 (Big_list.length rest) "split t 4 rest length";
  assert_list_equals [] (Big_list.as_list rest) "split t 4 rest list";
  (* split_n empty *)
  let (split, rest) = Big_list.split_n Big_list.empty 0 in
  assert_list_equals [] split "split empty 0 split";
  assert_equals 0 (Big_list.length rest) "split empty 0 rest length";
  assert_list_equals [] (Big_list.as_list rest) "split empty 0 rest list";
  let (split, rest) = Big_list.split_n Big_list.empty 1 in
  assert_list_equals [] split "split empty 1 split";
  assert_equals 0 (Big_list.length rest) "split empty 1 rest length";
  assert_list_equals [] (Big_list.as_list rest) "split empty 1 rest list";

  true

let () = Unit_test.run_all [("biglist", test)]
