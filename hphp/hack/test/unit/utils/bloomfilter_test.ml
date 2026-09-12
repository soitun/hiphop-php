(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude
open OUnit2

let test_zero_length_bloomfilter _ =
  let filter = Bloom_filter.create ~capacity:0 in
  let h = Bloom_filter.hash "" in
  assert_equal
    ~msg:"A zero length bloom filter should fail membership tests"
    false
    (Bloom_filter.mem filter h);

  Bloom_filter.add filter h;
  assert_equal
    ~msg:
      "A zero length bloom filter should fail membership tests after adding a member"
    false
    (Bloom_filter.mem filter h)

let test_filter_with_capacity capacity _ =
  let filter = Bloom_filter.create ~capacity in
  let h1 = Bloom_filter.hash "1" in
  let h2 = Bloom_filter.hash "2" in
  let h3 = Bloom_filter.hash "3" in
  let h4 = Bloom_filter.hash "4" in
  let h5 = Bloom_filter.hash "5" in

  assert_equal
    ~msg:"A bloom filter starts as empty"
    false
    (Bloom_filter.mem filter h1);

  Bloom_filter.add filter h1;
  assert_bool
    "After adding a member it should be present"
    (Bloom_filter.mem filter h1);

  Bloom_filter.add filter h1;
  assert_bool
    "Adding to a bloom filter should be idempotent"
    (Bloom_filter.mem filter h1);

  List.iter ~f:(Bloom_filter.add filter) [h2; h3; h4];
  assert_bool
    "After adding all the members they should all be present"
    (List.for_all ~f:(Bloom_filter.mem filter) [h1; h2; h3; h4]);

  assert_equal
    ~msg:"Most likely it should be false if not added"
    false
    (Bloom_filter.mem filter h5)

let () =
  "bloomfilter"
  >::: [
         "test_zero_length_bloomfilter" >:: test_zero_length_bloomfilter;
         "test_small_capacity_bloomfilter" >:: test_filter_with_capacity 4;
         "test_large_capacity_bloomfilter" >:: test_filter_with_capacity 100;
         "test_huge_capacity_bloomfilter" >:: test_filter_with_capacity 5000;
       ]
  |> run_test_tt_main
