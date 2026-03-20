<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait T {
  require class C;

  public static function foo(): void { C::bar(); }
}

final class C  {
  use T;

  public static function bar(): void { C::foo(); }
}
