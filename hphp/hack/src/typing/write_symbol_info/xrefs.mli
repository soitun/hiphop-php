(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hack

module PosMap : WrappedMap_sig.S with type key = Pos.t

(** maps a target fact id to the json representation of the corresponding fact,
   and the positions of symbol that reference it *)
type fact_map = (XRefTarget.t * Pos.t list) Fact_id.Map.t

type target_info = {
  target: XRefTarget.t;
  receiver_type: Declaration.t option;
  fact_id: Fact_id.t; (* the fact_id of the target *)
}

(** maps a position to various info about the target *)
type pos_map = target_info list PosMap.t

(** records if any of the symbol occurrences of a given target fact_id has
  * SO.affects_prod_build = true *)
type used_in_prod_build_map = bool Fact_id.Map.t

(* Maps of XRefs, constructed for a given file *)
type t = private {
  fact_map: fact_map;
  pos_map: pos_map;
  used_in_prod_build: used_in_prod_build_map;
}

(* Updates both maps. It is expected that for a given fact_id, all json are equal. *)
val add : t -> Fact_id.t -> Pos.t -> target_info -> t

(** given a target identified by its fact_id, update the used_in_prod_build_map with
  * the affects_prod_build information of a symbol occurrence of the target
  * Remark that a target, used_in_prod_build can be false only if all its symbol occurrences
  * have affetcs_prod_build = false
  *)
val mark_target_used_in_prod_build : t -> Fact_id.t -> bool -> t

val empty : t
