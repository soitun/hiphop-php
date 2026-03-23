<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

class C { use T1, T2; }

trait T1 { require class C; }
trait T2 { require extends C; }

<<__EntryPoint>>
function main(): void {
  echo "done\n";
}
