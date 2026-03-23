<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

trait T {
  require class C;
}

class C {}

class D {
  use T;
}

<<__EntryPoint>>
function main(): void {
  new D();
  echo "done\n";
}
