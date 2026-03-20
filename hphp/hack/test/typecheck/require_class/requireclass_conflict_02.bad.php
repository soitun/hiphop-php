<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


final class C {}

trait T {
  require class C;
  require extends C;
}
