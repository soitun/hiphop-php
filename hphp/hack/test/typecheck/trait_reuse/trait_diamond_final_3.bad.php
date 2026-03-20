<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

trait T {
  final public static function f(): void {}
}

class ParentClass {
  use T;
}

class MidClass extends ParentClass {}

trait T1 { use T; }

class ChildClass extends MidClass {
  use T1;
}
