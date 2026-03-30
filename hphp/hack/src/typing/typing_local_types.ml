(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
open Typing_defs

type local = {
  ty: locl_ty;  (** The type of the local *)
  defined: bool;
      (** True if the variable is definitely defined. False if it might not be. *)
  pos: Pos.t;  (** The position at which the variable got its type. *)
  eid: Expression_id.t;
      (** Along with a type, each local variable has a expression id associated with
          it. This is used when generating expression dependent types for the 'this'
          type. The idea is that if two local variables have the same expression_id
          then they refer to the same late bound type, and thus have compatible
          'this' types. *)
  macro_splice_vars: (Pos.t * locl_ty) Local_id.Map.t option;
      (** If the variable is bound to a expression tree splice that has nested expression
          trees with free variables, this records the types of those variables along with
          the position they occurred in. *)
}
[@@deriving show]

type t = local Local_id.Map.t

let empty = Local_id.Map.empty

let add_to_local_types id local m = Local_id.Map.add ?combine:None id local m
