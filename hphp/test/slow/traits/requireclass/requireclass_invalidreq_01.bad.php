<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

trait T2 {}

trait T {
  require class T2;
}

<<__EntryPoint>>
function main(): void {
  echo "done\n";
}
