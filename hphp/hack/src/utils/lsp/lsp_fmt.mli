(*
 * Copyright (c) 2019, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val parse_id : Yojson.Safe.t -> Lsp.lsp_id

val parse_id_opt : Yojson.Safe.t option -> Lsp.lsp_id option

val print_id : Lsp.lsp_id -> Yojson.Safe.t

val id_to_string : Lsp.lsp_id -> string

val parse_position : Yojson.Safe.t option -> Lsp.position

val print_position : Lsp.position -> Yojson.Safe.t

val print_range : Lsp.range -> Yojson.Safe.t

val print_location : Lsp.Location.t -> Yojson.Safe.t

val print_locations : Lsp.Location.t list -> Yojson.Safe.t

val print_definition_location : Lsp.DefinitionLocation.t -> Yojson.Safe.t

val print_definition_locations : Lsp.DefinitionLocation.t list -> Yojson.Safe.t

val parse_range_exn : Yojson.Safe.t option -> Lsp.range

val parse_range_opt : Yojson.Safe.t option -> Lsp.range option

val parse_textDocumentIdentifier :
  Yojson.Safe.t option -> Lsp.TextDocumentIdentifier.t

val parse_versionedTextDocumentIdentifier :
  Yojson.Safe.t option -> Lsp.VersionedTextDocumentIdentifier.t

val parse_textDocumentItem : Yojson.Safe.t option -> Lsp.TextDocumentItem.t

val print_textDocumentItem : Lsp.TextDocumentItem.t -> Yojson.Safe.t

val print_markedItem : Lsp.markedString -> Yojson.Safe.t

val parse_textDocumentPositionParams :
  Yojson.Safe.t option -> Lsp.TextDocumentPositionParams.t

val parse_textEdit : Yojson.Safe.t option -> Lsp.TextEdit.t option

val print_textEdit : Lsp.TextEdit.t -> Yojson.Safe.t

val print_textEdits : Lsp.TextEdit.t list -> Yojson.Safe.t

val print_command : Lsp.Command.t -> Yojson.Safe.t

val parse_command : Yojson.Safe.t option -> Lsp.Command.t

val parse_formattingOptions :
  Yojson.Safe.t option -> Lsp.DocumentFormatting.formattingOptions

val print_symbolInformation : Lsp.SymbolInformation.t -> Yojson.Safe.t

val print_shutdown : unit -> Yojson.Safe.t

val parse_cancelRequest : Yojson.Safe.t option -> Lsp.CancelRequest.params

val print_cancelRequest : Lsp.CancelRequest.params -> Yojson.Safe.t

val print_rage : Lsp.RageFB.result -> Yojson.Safe.t

val parse_didOpen : Yojson.Safe.t option -> Lsp.DidOpen.params

val print_didOpen : Lsp.DidOpen.params -> Yojson.Safe.t

val parse_didClose : Yojson.Safe.t option -> Lsp.DidClose.params

val parse_didSave : Yojson.Safe.t option -> Lsp.DidSave.params

val parse_didChange : Yojson.Safe.t option -> Lsp.DidChange.params

val parse_signatureHelp : Yojson.Safe.t option -> Lsp.SignatureHelp.params

val print_signatureHelp : Lsp.SignatureHelp.result -> Yojson.Safe.t

val parse_documentRename : Yojson.Safe.t option -> Lsp.Rename.params

val print_diagnostics : Lsp.PublishDiagnostics.params -> Yojson.Safe.t

val print_codeActionResult :
  Lsp.CodeAction.result -> Lsp.CodeActionRequest.params -> Yojson.Safe.t

val print_codeActionResolveResult :
  Lsp.CodeActionResolve.result -> Yojson.Safe.t

val print_logMessage : Lsp.MessageType.t -> string -> Yojson.Safe.t

val print_showMessage : Lsp.MessageType.t -> string -> Yojson.Safe.t

val print_showMessageRequest :
  Lsp.ShowMessageRequest.showMessageRequestParams -> Yojson.Safe.t

val parse_result_showMessageRequest :
  Yojson.Safe.t option -> Lsp.ShowMessageRequest.result

val print_showStatus : Lsp.ShowStatusFB.showStatusParams -> Yojson.Safe.t

val print_connectionStatus : Lsp.ConnectionStatusFB.params -> Yojson.Safe.t

val parse_hover : Yojson.Safe.t option -> Lsp.Hover.params

val print_hover : Lsp.Hover.result -> Yojson.Safe.t

val parse_completionItem :
  Yojson.Safe.t option -> Lsp.CompletionItemResolve.params

val print_completionItem : Lsp.Completion.completionItem -> Yojson.Safe.t

val parse_completion : Yojson.Safe.t option -> Lsp.Completion.params

val print_completion : Lsp.Completion.result -> Yojson.Safe.t

val parse_callItem : Yojson.Safe.t option -> Lsp.CallHierarchyItem.t

val print_callItem : Lsp.CallHierarchyItem.t -> Yojson.Safe.t

val parse_callHierarchyCalls :
  Yojson.Safe.t option -> Lsp.CallHierarchyCallsRequestParam.t

val print_PrepareCallHierarchyResult :
  Lsp.PrepareCallHierarchy.result -> Yojson.Safe.t

val print_CallHierarchyIncomingCallsResult :
  Lsp.CallHierarchyIncomingCalls.result -> Yojson.Safe.t

val print_CallHierarchyOutgoingCallsResult :
  Lsp.CallHierarchyOutgoingCalls.result -> Yojson.Safe.t

val parse_willSaveWaitUntil :
  Yojson.Safe.t option -> Lsp.WillSaveWaitUntil.params

val parse_workspaceSymbol : Yojson.Safe.t option -> Lsp.WorkspaceSymbol.params

val print_workspaceSymbol : Lsp.WorkspaceSymbol.result -> Yojson.Safe.t

val parse_documentSymbol : Yojson.Safe.t option -> Lsp.DocumentSymbol.params

val print_documentSymbol : Lsp.DocumentSymbol.result -> Yojson.Safe.t

val parse_findReferences : Yojson.Safe.t option -> Lsp.FindReferences.params

val parse_documentHighlight :
  Yojson.Safe.t option -> Lsp.DocumentHighlight.params

val print_documentHighlight : Lsp.DocumentHighlight.result -> Yojson.Safe.t

val parse_documentFormatting :
  Yojson.Safe.t option -> Lsp.DocumentFormatting.params

val print_documentFormatting : Lsp.DocumentFormatting.result -> Yojson.Safe.t

val parse_documentRangeFormatting :
  Yojson.Safe.t option -> Lsp.DocumentRangeFormatting.params

val print_documentRangeFormatting :
  Lsp.DocumentRangeFormatting.result -> Yojson.Safe.t

val parse_documentOnTypeFormatting :
  Yojson.Safe.t option -> Lsp.DocumentOnTypeFormatting.params

val print_documentOnTypeFormatting :
  Lsp.DocumentOnTypeFormatting.result -> Yojson.Safe.t

val parse_initialize : Yojson.Safe.t option -> Lsp.Initialize.params

val print_initializeError : Lsp.Initialize.errorData -> Yojson.Safe.t

val print_initialize : Lsp.Initialize.result -> Yojson.Safe.t

val print_registerCapability : Lsp.RegisterCapability.params -> Yojson.Safe.t

val parse_didChangeWatchedFiles :
  Yojson.Safe.t option -> Lsp.DidChangeWatchedFiles.params

val print_error : Lsp.Error.t -> Yojson.Safe.t

val error_to_log_string : Lsp.Error.t -> string

val parse_error : Yojson.Safe.t -> Lsp.Error.t

val request_name_to_string : Lsp.lsp_request -> string

val result_name_to_string : Lsp.lsp_result -> string

val notification_name_to_string : Lsp.lsp_notification -> string

val message_name_to_string : Lsp.lsp_message -> string

val denorm_message_to_string : Lsp.lsp_message -> string

val parse_lsp_request : string -> Yojson.Safe.t option -> Lsp.lsp_request

val parse_lsp_notification :
  string -> Yojson.Safe.t option -> Lsp.lsp_notification

val parse_lsp_result : Lsp.lsp_request -> Yojson.Safe.t -> Lsp.lsp_result

val parse_lsp :
  Yojson.Safe.t -> (Lsp.lsp_id -> Lsp.lsp_request) -> Lsp.lsp_message

val print_lsp_request : Lsp.lsp_id -> Lsp.lsp_request -> Yojson.Safe.t

val print_lsp_response : Lsp.lsp_id -> Lsp.lsp_result -> Yojson.Safe.t

val print_lsp_notification : Lsp.lsp_notification -> Yojson.Safe.t

val print_lsp : Lsp.lsp_message -> Yojson.Safe.t

val get_uri_opt : Lsp.lsp_message -> Lsp.DocumentUri.t option
