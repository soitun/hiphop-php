<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


abstract class C {
  use T;
}

trait T {
  require class C;
}
