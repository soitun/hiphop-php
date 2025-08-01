#
# Autogenerated by Thrift
#
# DO NOT EDIT
#  @generated
#

from __future__ import annotations

import typing as _typing
import builtins


import enum

import folly.iobuf as _fbthrift_iobuf
import invariant.thrift_abstract_types as _fbthrift_python_abstract_types
import thrift.python.types as _fbthrift_python_types
import thrift.python.exceptions as _fbthrift_python_exceptions


@_typing.final
class StructForInvariantTypes(_fbthrift_python_types.Struct, _fbthrift_python_abstract_types.StructForInvariantTypes):
    num: _typing.Final[builtins.int] = ...
    def __init__(
        self, *,
        num: _typing.Optional[builtins.int]=...
    ) -> None: ...

    def __call__(
        self, *,
        num: _typing.Optional[builtins.int]=...
    ) -> _typing.Self: ...
    def __iter__(self) -> _typing.Iterator[_typing.Tuple[builtins.str, _typing.Union[builtins.int]]]: ...
    def _to_python(self) -> _typing.Self: ...
    def _to_mutable_python(self) -> "invariant.thrift_mutable_types.StructForInvariantTypes": ...  # type: ignore
    def _to_py3(self) -> "invariant.types.StructForInvariantTypes": ...  # type: ignore
    def _to_py_deprecated(self) -> "invariant.ttypes.StructForInvariantTypes": ...  # type: ignore
_fbthrift_StructForInvariantTypes = StructForInvariantTypes

@_typing.final
class UnionForInvariantTypes(_fbthrift_python_types.Struct, _fbthrift_python_abstract_types.UnionForInvariantTypes):
    num32: _typing.Final[builtins.int] = ...
    num64: _typing.Final[builtins.int] = ...
    def __init__(
        self, *,
        num32: _typing.Optional[builtins.int]=...,
        num64: _typing.Optional[builtins.int]=...
    ) -> None: ...

    def __call__(
        self, *,
        num32: _typing.Optional[builtins.int]=...,
        num64: _typing.Optional[builtins.int]=...
    ) -> _typing.Self: ...
    def __iter__(self) -> _typing.Iterator[_typing.Tuple[builtins.str, _typing.Union[builtins.int, builtins.int]]]: ...
    def _to_python(self) -> _typing.Self: ...
    def _to_mutable_python(self) -> "invariant.thrift_mutable_types.UnionForInvariantTypes": ...  # type: ignore
    def _to_py3(self) -> "invariant.types.UnionForInvariantTypes": ...  # type: ignore
    def _to_py_deprecated(self) -> "invariant.ttypes.UnionForInvariantTypes": ...  # type: ignore
_fbthrift_UnionForInvariantTypes = UnionForInvariantTypes

@_typing.final
class InvariantTypes(_fbthrift_python_types.Union, _fbthrift_python_abstract_types.InvariantTypes):
    struct_map: _typing.Final[_typing.Mapping[_fbthrift_StructForInvariantTypes, builtins.int]] = ...
    union_map: _typing.Final[_typing.Mapping[_fbthrift_UnionForInvariantTypes, builtins.int]] = ...
    def __init__(
        self, *,
        struct_map: _typing.Optional[_typing.Mapping[_fbthrift_StructForInvariantTypes, builtins.int]]=...,
        union_map: _typing.Optional[_typing.Mapping[_fbthrift_UnionForInvariantTypes, builtins.int]]=...
    ) -> None: ...


    class Type(enum.Enum):
        EMPTY: InvariantTypes.Type = ...
        struct_map: InvariantTypes.Type = ...
        union_map: InvariantTypes.Type = ...

    class FbThriftUnionFieldEnum(enum.Enum):
        EMPTY: InvariantTypes.FbThriftUnionFieldEnum = ...
        struct_map: InvariantTypes.FbThriftUnionFieldEnum = ...
        union_map: InvariantTypes.FbThriftUnionFieldEnum = ...

    fbthrift_current_value: _typing.Final[_typing.Union[None, _typing.Mapping[_fbthrift_StructForInvariantTypes, builtins.int], _typing.Mapping[_fbthrift_UnionForInvariantTypes, builtins.int]]]
    fbthrift_current_field: _typing.Final[FbThriftUnionFieldEnum]
    @classmethod
    def fromValue(cls, value: _typing.Union[None, _typing.Mapping[_fbthrift_StructForInvariantTypes, builtins.int], _typing.Mapping[_fbthrift_UnionForInvariantTypes, builtins.int]]) -> InvariantTypes: ...
    value: _typing.Final[_typing.Union[None, _typing.Mapping[_fbthrift_StructForInvariantTypes, builtins.int], _typing.Mapping[_fbthrift_UnionForInvariantTypes, builtins.int]]]
    type: _typing.Final[Type]
    def get_type(self) -> Type: ...
    def _to_python(self) -> _typing.Self: ...
    def _to_mutable_python(self) -> "invariant.thrift_mutable_types.InvariantTypes": ...  # type: ignore
    def _to_py3(self) -> "invariant.types.InvariantTypes": ...  # type: ignore
    def _to_py_deprecated(self) -> "invariant.ttypes.InvariantTypes": ...  # type: ignore
_fbthrift_InvariantTypes = InvariantTypes
