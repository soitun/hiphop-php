(*
 * Copyright (c) 2018, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Aast

let handler =
  object
    inherit Tast_visitor.handler_base

    method! at_expr env =
      function
      | (_, _, Obj_get (_, (_, p, This), _, _)) ->
        let Equal = Tast_env.eq_typing_env in
        Typing_error_utils.add_typing_error
          ~env
          Typing_error.(
            primary
            @@ Primary.Nonsense_member_selection { pos = p; kind = "$this" })
      | (_, _, Obj_get (_, (_, p, Lplaceholder _), _, _)) ->
        let Equal = Tast_env.eq_typing_env in
        Typing_error_utils.add_typing_error
          ~env
          Typing_error.(
            primary
            @@ Primary.Nonsense_member_selection { pos = p; kind = "$_" })
      | _ -> ()
  end
