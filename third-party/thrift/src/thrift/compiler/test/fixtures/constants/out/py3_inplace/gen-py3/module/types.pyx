#
# Autogenerated by Thrift for thrift/compiler/test/fixtures/constants/src/module.thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#
cimport cython as __cython
from cpython.object cimport PyTypeObject
from libcpp.memory cimport shared_ptr, make_shared, unique_ptr
from libcpp.optional cimport optional as __optional
from libcpp.string cimport string
from libcpp cimport bool as cbool
from libcpp.iterator cimport inserter as cinserter
from libcpp.utility cimport move as cmove
from cpython cimport bool as pbool
from cython.operator cimport dereference as deref, preincrement as inc, address as ptr_address
import thrift.py3.types
from thrift.py3.types import _IsSet as _fbthrift_IsSet
from thrift.py3.types cimport make_unique
cimport thrift.py3.types
cimport thrift.py3.exceptions
cimport thrift.python.exceptions
import thrift.python.converter
from thrift.python.types import EnumMeta as __EnumMeta
from thrift.python.std_libcpp cimport sv_to_str as __sv_to_str, string_view as __cstring_view
from thrift.python.types cimport BadEnum as __BadEnum
from thrift.py3.types cimport (
    richcmp as __richcmp,
    init_unicode_from_cpp as __init_unicode_from_cpp,
    set_iter as __set_iter,
    map_iter as __map_iter,
    reference_shared_ptr as __reference_shared_ptr,
    get_field_name_by_index as __get_field_name_by_index,
    reset_field as __reset_field,
    translate_cpp_enum_to_python,
    const_pointer_cast,
    make_const_shared,
    constant_shared_ptr,
    deref_const as __deref_const,
    mixin_deprecation_log_error,
)
from thrift.py3.types cimport _ensure_py3_or_raise, _ensure_py3_container_or_raise
cimport thrift.py3.serializer as serializer
from thrift.python.protocol cimport Protocol as __Protocol
import folly.iobuf as _fbthrift_iobuf
from folly.optional cimport cOptional
from folly.memory cimport to_shared_ptr as __to_shared_ptr
from folly.range cimport Range as __cRange

import sys
from collections.abc import Sequence, Set, Mapping, Iterable
import weakref as __weakref
import builtins as _builtins
import importlib



import module.types_inplace_FBTHRIFT_ONLY_DO_NOT_USE as _fbthrift_types_inplace
Internship = _fbthrift_types_inplace.Internship
Range = _fbthrift_types_inplace.Range
struct1 = _fbthrift_types_inplace.struct1
struct2 = _fbthrift_types_inplace.struct2
struct3 = _fbthrift_types_inplace.struct3
struct4 = _fbthrift_types_inplace.struct4
union1 = _fbthrift_types_inplace.union1
union2 = _fbthrift_types_inplace.union2
EmptyEnum = _fbthrift_types_inplace.EmptyEnum
City = _fbthrift_types_inplace.City
Company = _fbthrift_types_inplace.Company
List__i32 = _fbthrift_types_inplace.List__i32
Map__string_i32 = _fbthrift_types_inplace.Map__string_i32
List__Map__string_i32 = _fbthrift_types_inplace.List__Map__string_i32
Map__string_string = _fbthrift_types_inplace.Map__string_string
List__Company = _fbthrift_types_inplace.List__Company
List__Range = _fbthrift_types_inplace.List__Range
List__Internship = _fbthrift_types_inplace.List__Internship
List__string = _fbthrift_types_inplace.List__string
Set__i32 = _fbthrift_types_inplace.Set__i32
Set__string = _fbthrift_types_inplace.Set__string
Map__i32_i32 = _fbthrift_types_inplace.Map__i32_i32
Map__i32_string = _fbthrift_types_inplace.Map__i32_string
Map__i32_bool = _fbthrift_types_inplace.Map__i32_bool
myInt = _fbthrift_types_inplace.myInt
name = _fbthrift_types_inplace.name
multi_line_string = _fbthrift_types_inplace.multi_line_string
states = _fbthrift_types_inplace.states
x = _fbthrift_types_inplace.x
y = _fbthrift_types_inplace.y
z = _fbthrift_types_inplace.z
zeroDoubleValue = _fbthrift_types_inplace.zeroDoubleValue
longDoubleValue = _fbthrift_types_inplace.longDoubleValue
bin = _fbthrift_types_inplace.bin
my_company = _fbthrift_types_inplace.my_company
foo = _fbthrift_types_inplace.foo
bar = _fbthrift_types_inplace.bar
mymap = _fbthrift_types_inplace.mymap
my_apps = _fbthrift_types_inplace.my_apps
instagram = _fbthrift_types_inplace.instagram
partial_const = _fbthrift_types_inplace.partial_const
kRanges = _fbthrift_types_inplace.kRanges
internList = _fbthrift_types_inplace.internList
pod_0 = _fbthrift_types_inplace.pod_0
pod_s_0 = _fbthrift_types_inplace.pod_s_0
pod_1 = _fbthrift_types_inplace.pod_1
pod_s_1 = _fbthrift_types_inplace.pod_s_1
pod_2 = _fbthrift_types_inplace.pod_2
pod_trailing_commas = _fbthrift_types_inplace.pod_trailing_commas
pod_s_2 = _fbthrift_types_inplace.pod_s_2
pod_3 = _fbthrift_types_inplace.pod_3
pod_s_3 = _fbthrift_types_inplace.pod_s_3
pod_4 = _fbthrift_types_inplace.pod_4
u_1_1 = _fbthrift_types_inplace.u_1_1
u_1_2 = _fbthrift_types_inplace.u_1_2
u_1_3 = _fbthrift_types_inplace.u_1_3
u_2_1 = _fbthrift_types_inplace.u_2_1
u_2_2 = _fbthrift_types_inplace.u_2_2
u_2_3 = _fbthrift_types_inplace.u_2_3
u_2_4 = _fbthrift_types_inplace.u_2_4
u_2_5 = _fbthrift_types_inplace.u_2_5
u_2_6 = _fbthrift_types_inplace.u_2_6
apostrophe = _fbthrift_types_inplace.apostrophe
tripleApostrophe = _fbthrift_types_inplace.tripleApostrophe
quotationMark = _fbthrift_types_inplace.quotationMark
backslash = _fbthrift_types_inplace.backslash
escaped_a = _fbthrift_types_inplace.escaped_a
char2ascii = _fbthrift_types_inplace.char2ascii
escaped_strings = _fbthrift_types_inplace.escaped_strings
unicode_list = _fbthrift_types_inplace.unicode_list
false_c = _fbthrift_types_inplace.false_c
true_c = _fbthrift_types_inplace.true_c
zero_byte = _fbthrift_types_inplace.zero_byte
zero16 = _fbthrift_types_inplace.zero16
zero32 = _fbthrift_types_inplace.zero32
zero64 = _fbthrift_types_inplace.zero64
zero_dot_zero = _fbthrift_types_inplace.zero_dot_zero
empty_string = _fbthrift_types_inplace.empty_string
empty_int_list = _fbthrift_types_inplace.empty_int_list
empty_string_list = _fbthrift_types_inplace.empty_string_list
empty_int_set = _fbthrift_types_inplace.empty_int_set
empty_string_set = _fbthrift_types_inplace.empty_string_set
empty_int_int_map = _fbthrift_types_inplace.empty_int_int_map
empty_int_string_map = _fbthrift_types_inplace.empty_int_string_map
empty_string_int_map = _fbthrift_types_inplace.empty_string_int_map
empty_string_string_map = _fbthrift_types_inplace.empty_string_string_map
unicode_map = _fbthrift_types_inplace.unicode_map
maxIntDec = _fbthrift_types_inplace.maxIntDec
maxIntOct = _fbthrift_types_inplace.maxIntOct
maxIntHex = _fbthrift_types_inplace.maxIntHex
maxIntBin = _fbthrift_types_inplace.maxIntBin
maxDub = _fbthrift_types_inplace.maxDub
minDub = _fbthrift_types_inplace.minDub
minSDub = _fbthrift_types_inplace.minSDub
maxPIntDec = _fbthrift_types_inplace.maxPIntDec
maxPIntOct = _fbthrift_types_inplace.maxPIntOct
maxPIntHex = _fbthrift_types_inplace.maxPIntHex
maxPIntBin = _fbthrift_types_inplace.maxPIntBin
maxPDub = _fbthrift_types_inplace.maxPDub
minPDub = _fbthrift_types_inplace.minPDub
minPSDub = _fbthrift_types_inplace.minPSDub
minIntDec = _fbthrift_types_inplace.minIntDec
minIntOct = _fbthrift_types_inplace.minIntOct
minIntHex = _fbthrift_types_inplace.minIntHex
minIntBin = _fbthrift_types_inplace.minIntBin
maxNDub = _fbthrift_types_inplace.maxNDub
minNDub = _fbthrift_types_inplace.minNDub
minNSDub = _fbthrift_types_inplace.minNSDub
I2B = _fbthrift_types_inplace.I2B
I2B_REF = _fbthrift_types_inplace.I2B_REF

MyCompany = Company
MyStringIdentifier = str
MyIntIdentifier = int
MyMapIdentifier = Map__string_string
