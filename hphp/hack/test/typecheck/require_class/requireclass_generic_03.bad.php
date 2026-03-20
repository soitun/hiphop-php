<?hh
// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.


trait MyTrait {
  require class C<int>;
}

final class C<T> {
}
