<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait T {
  require class C;

  public function foo(): void {
    self::bar();
  }
}

final class C {
  use T;

  public static function bar(): void {}
}
