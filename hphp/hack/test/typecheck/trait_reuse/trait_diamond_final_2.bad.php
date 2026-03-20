<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.

trait T {
  final public static function f(): void {}
}

class ParentClass {
  use T;
}

<<__EnableMethodTraitDiamond>>
class ChildClass extends ParentClass {
  use T;
}
