<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait MyTrait {
  require class C;
}

final class C<T> {
  use MyTrait;
}
