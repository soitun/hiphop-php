(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

type clock =
  | Watchman of Watchman.clock
  | Eden of Edenfs_watcher.clock
[@@deriving show, eq]
