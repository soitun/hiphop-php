<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait T {
  require class C;

  public function foo(): int { return C::$x; }
}

final class C  {
  use T;
}
