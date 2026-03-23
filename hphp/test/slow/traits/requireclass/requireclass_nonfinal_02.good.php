<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

class C {}

trait T {
  require class C;
}

<<__EntryPoint>>
function main(): void {
  echo "done\n";
}
