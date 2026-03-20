<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait T {
  require class C;
}

final class C  {
  use T;

  public function foo(): void { C::bar(); }
}
