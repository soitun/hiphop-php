<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

const bool BOOLCNS = false;

class Cls {
  const INTCNS = 123;
}

const vec<mixed> ARRAY1 = vec[];
const vec<mixed> ARRAY2 = vec[1, 2, 3, 4];
const vec<mixed> ARRAY3 = vec['a', 'b', 'c', 'd'];
const vec<mixed> ARRAY4 = vec[1, vec[false, null], vec[true, 'abc'], 1.23, vec[]];
const vec<mixed> ARRAY5 = vec[vec[], dict[], keyset[]];
const vec<mixed> ARRAY6 = vec[vec[1, 2], dict['abc' => true], keyset['a', 100, 'b']];
const vec<mixed> ARRAY7 = ARRAY1;
const vec<mixed> ARRAY8 = vec[ARRAY2, ARRAY2];
const vec<mixed> ARRAY9 = vec[BOOLCNS ? ARRAY2 : ARRAY3];
const vec<mixed> ARRAY10 = vec[Cls::INTCNS];

const vec<mixed> VEC1 = vec[];
const vec<mixed> VEC2 = vec[1, 2, 3, 4];
const vec<mixed> VEC3 = vec['a', 'b', 'c', 'd'];
const vec<mixed> VEC4 = vec[1, vec[false, null], vec[true, 'abc'], 1.23, vec[]];
const vec<mixed> VEC5 = vec[vec[], dict[], keyset[]];
const vec<mixed> VEC6 = vec[vec[1, 2], dict['abc' => true], keyset['a', 100, 'b']];
const vec<mixed> VEC7 = VEC1;
const vec<mixed> VEC8 = vec[VEC2, VEC2];
const vec<mixed> VEC9 = vec[BOOLCNS ? VEC2 : VEC3];
const vec<mixed> VEC10 = vec[Cls::INTCNS];

const dict<arraykey, mixed> DICT1 = dict[];
const dict<arraykey, mixed> DICT2 = dict[100 => 1, 200 => 2, 300 => 3, 400 => 4];
const dict<arraykey, mixed> DICT3 = dict['key1' => 'a', 'key2' => 'b', 'key3' => 'c', 'key4' => 'd'];
const dict<arraykey, mixed> DICT4 = dict['key1' => 500, 'key2' => 800];
const dict<arraykey, mixed> DICT5 = dict[100 => 'abc', 200 => 'def'];
const dict<arraykey, mixed> DICT6 = dict['100' => 'abc', '200' => 'def', 100 => 'ghi', 200 => 'jkl'];
const dict<arraykey, mixed> DICT7 = dict[0 => 1,
                   1 => dict['a' => false, 5 => null],
                   2 => dict[10 => true, 'z' => 'abc'],
                   3 => 1.23,
                   4 => dict[]];
const dict<arraykey, mixed> DICT8 = dict['100' => 5, 100 => 'abc', 1 => dict[123 => 'abc', '123' => 10]];
const dict<arraykey, mixed> DICT9 = dict[100 => vec[], 200 => vec[], 300 => keyset[]];
const dict<arraykey, mixed> DICT10 = dict[100 => vec[1, 2], 200 => vec['abc'], 300 => keyset['a', 100, 'b']];
const dict<arraykey, mixed> DICT11 = DICT1;
const dict<arraykey, mixed> DICT12 = dict[123 => DICT2, 456 => DICT2];
const dict<arraykey, mixed> DICT13 = dict[100 => BOOLCNS ? DICT2 : DICT3];
const dict<arraykey, mixed> DICT14 = dict['abc' => Cls::INTCNS];

const keyset<arraykey> KEYSET1 = keyset[];
const keyset<arraykey> KEYSET2 = keyset[1, 2, 3, 4];
const keyset<arraykey> KEYSET3 = keyset['a', 'b', 'c'];
const keyset<arraykey> KEYSET4 = keyset[1, '1', 2, '2'];
const keyset<arraykey> KEYSET5 = KEYSET1;
const keyset<arraykey> KEYSET6 = keyset[BOOLCNS ? 'a' : 1];
const keyset<arraykey> KEYSET7 = keyset[Cls::INTCNS];

<<__EntryPoint>> function main(): void {
var_dump(ARRAY1);
var_dump(ARRAY2);
var_dump(ARRAY3);
var_dump(ARRAY4);
var_dump(ARRAY5);
var_dump(ARRAY6);
var_dump(ARRAY7);
var_dump(ARRAY8);
var_dump(ARRAY9);
var_dump(ARRAY10);

var_dump(VEC1);
var_dump(VEC2);
var_dump(VEC3);
var_dump(VEC4);
var_dump(VEC5);
var_dump(VEC6);
var_dump(VEC7);
var_dump(VEC8);
var_dump(VEC9);
var_dump(VEC10);

var_dump(DICT1);
var_dump(DICT2);
var_dump(DICT3);
var_dump(DICT4);
var_dump(DICT5);
var_dump(DICT6);
var_dump(DICT7);
var_dump(DICT8);
var_dump(DICT9);
var_dump(DICT10);
var_dump(DICT11);
var_dump(DICT12);
var_dump(DICT13);
var_dump(DICT14);

var_dump(KEYSET1);
var_dump(KEYSET2);
var_dump(KEYSET3);
var_dump(KEYSET4);
var_dump(KEYSET5);
var_dump(KEYSET6);
var_dump(KEYSET7);
}
