#
# Autogenerated by Thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#

from __future__ import absolute_import
import sys
from thrift.util.Recursive import fix_spec
from thrift.Thrift import TType, TMessageType, TPriority, TRequestContext, TProcessorEventHandler, TServerInterface, TProcessor, TException, TApplicationException, UnimplementedTypedef
from thrift.protocol.TProtocol import TProtocolException

from json import loads
import sys
if sys.version_info[0] >= 3:
  long = int


import pprint
import warnings
from thrift import Thrift
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.protocol import TCompactProtocol
from thrift.protocol import THeaderProtocol
fastproto = None
try:
  from thrift.protocol import fastproto
except ImportError:
  pass

def __EXPAND_THRIFT_SPEC(spec):
    next_id = 0
    for item in spec:
        item_id = item[0]
        if next_id >= 0 and item_id < 0:
            next_id = item_id
        if item_id != next_id:
            for _ in range(next_id, item_id):
                yield None
        yield item
        next_id = item_id + 1

class ThriftEnumWrapper(int):
  def __new__(cls, enum_class, value):
    return super().__new__(cls, value)
  def __init__(self, enum_class, value):    self.enum_class = enum_class
  def __repr__(self):
    return self.enum_class.__name__ + '.' + self.enum_class._VALUES_TO_NAMES[self]

all_structs = []
UTF8STRINGS = bool(0) or sys.version_info.major >= 3

__all__ = ['UTF8STRINGS', 'DoSomethingResult']

class DoSomethingResult:
  r"""
  Attributes:
   - s_res
   - i_res
  """

  thrift_spec = None
  thrift_field_annotations = None
  thrift_struct_annotations = None
  __init__ = None
  @staticmethod
  def isUnion():
    return False

  def read(self, iprot):
    if (isinstance(iprot, TBinaryProtocol.TBinaryProtocolAccelerated) or (isinstance(iprot, THeaderProtocol.THeaderProtocolAccelerate) and iprot.get_protocol_id() == THeaderProtocol.THeaderProtocol.T_BINARY_PROTOCOL)) and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastproto is not None:
      fastproto.decode(self, iprot.trans, [self.__class__, self.thrift_spec, False], utf8strings=UTF8STRINGS, protoid=0)
      return
    if (isinstance(iprot, TCompactProtocol.TCompactProtocolAccelerated) or (isinstance(iprot, THeaderProtocol.THeaderProtocolAccelerate) and iprot.get_protocol_id() == THeaderProtocol.THeaderProtocol.T_COMPACT_PROTOCOL)) and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastproto is not None:
      fastproto.decode(self, iprot.trans, [self.__class__, self.thrift_spec, False], utf8strings=UTF8STRINGS, protoid=2)
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.s_res = iprot.readString().decode('utf-8') if UTF8STRINGS else iprot.readString()
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.I32:
          self.i_res = iprot.readI32()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if (isinstance(oprot, TBinaryProtocol.TBinaryProtocolAccelerated) or (isinstance(oprot, THeaderProtocol.THeaderProtocolAccelerate) and oprot.get_protocol_id() == THeaderProtocol.THeaderProtocol.T_BINARY_PROTOCOL)) and self.thrift_spec is not None and fastproto is not None:
      oprot.trans.write(fastproto.encode(self, [self.__class__, self.thrift_spec, False], utf8strings=UTF8STRINGS, protoid=0))
      return
    if (isinstance(oprot, TCompactProtocol.TCompactProtocolAccelerated) or (isinstance(oprot, THeaderProtocol.THeaderProtocolAccelerate) and oprot.get_protocol_id() == THeaderProtocol.THeaderProtocol.T_COMPACT_PROTOCOL)) and self.thrift_spec is not None and fastproto is not None:
      oprot.trans.write(fastproto.encode(self, [self.__class__, self.thrift_spec, False], utf8strings=UTF8STRINGS, protoid=2))
      return
    oprot.writeStructBegin('DoSomethingResult')
    if self.s_res != None:
      oprot.writeFieldBegin('s_res', TType.STRING, 1)
      oprot.writeString(self.s_res.encode('utf-8')) if UTF8STRINGS and not isinstance(self.s_res, bytes) else oprot.writeString(self.s_res)
      oprot.writeFieldEnd()
    if self.i_res != None:
      oprot.writeFieldBegin('i_res', TType.I32, 2)
      oprot.writeI32(self.i_res)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def readFromJson(self, json, is_text=True, **kwargs):
    kwargs_copy = dict(kwargs)
    wrap_enum_constants = kwargs_copy.pop('wrap_enum_constants', False)
    relax_enum_validation = bool(kwargs_copy.pop('relax_enum_validation', not wrap_enum_constants))
    set_cls = kwargs_copy.pop('custom_set_cls', set)
    dict_cls = kwargs_copy.pop('custom_dict_cls', dict)
    if wrap_enum_constants and relax_enum_validation:
        raise ValueError(
            'wrap_enum_constants cannot be used together with relax_enum_validation'
        )
    if kwargs_copy:
        extra_kwargs = ', '.join(kwargs_copy.keys())
        raise ValueError(
            'Unexpected keyword arguments: ' + extra_kwargs
        )
    json_obj = json
    if is_text:
      json_obj = loads(json)
    if 's_res' in json_obj and json_obj['s_res'] is not None:
      self.s_res = json_obj['s_res']
    if 'i_res' in json_obj and json_obj['i_res'] is not None:
      self.i_res = json_obj['i_res']
      if self.i_res > 0x7fffffff or self.i_res < -0x80000000:
        raise TProtocolException(TProtocolException.INVALID_DATA, 'number exceeds limit in field')

  def __repr__(self):
    L = []
    padding = ' ' * 4
    if self.s_res is not None:
      value = pprint.pformat(self.s_res, indent=0)
      value = padding.join(value.splitlines(True))
      L.append('    s_res=%s' % (value))
    if self.i_res is not None:
      value = pprint.pformat(self.i_res, indent=0)
      value = padding.join(value.splitlines(True))
      L.append('    i_res=%s' % (value))
    return "%s(%s)" % (self.__class__.__name__, "\n" + ",\n".join(L) if L else '')

  def __eq__(self, other):
    if not isinstance(other, self.__class__):
      return False

    return self.__dict__ == other.__dict__ 

  def __ne__(self, other):
    return not (self == other)

  def __dir__(self):
    return (
      's_res',
      'i_res',
    )

  __hash__ = object.__hash__

  def _to_python(self):
    import importlib
    import thrift.python.converter
    python_types = importlib.import_module("test.fixtures.another_interactions.shared.thrift_types")
    return thrift.python.converter.to_python_struct(python_types.DoSomethingResult, self)

  def _to_mutable_python(self):
    import importlib
    import thrift.python.mutable_converter
    python_mutable_types = importlib.import_module("test.fixtures.another_interactions.shared.thrift_mutable_types")
    return thrift.python.mutable_converter.to_mutable_python_struct_or_union(python_mutable_types.DoSomethingResult, self)

  def _to_py3(self):
    import importlib
    import thrift.py3.converter
    py3_types = importlib.import_module("test.fixtures.another_interactions.shared.types")
    return thrift.py3.converter.to_py3_struct(py3_types.DoSomethingResult, self)

  def _to_py_deprecated(self):
    return self

all_structs.append(DoSomethingResult)
DoSomethingResult.thrift_spec = tuple(__EXPAND_THRIFT_SPEC((
  (1, TType.STRING, 's_res', True, None, 2, ), # 1
  (2, TType.I32, 'i_res', None, None, 2, ), # 2
)))

DoSomethingResult.thrift_struct_annotations = {
}
DoSomethingResult.thrift_field_annotations = {
}

def DoSomethingResult__init__(self, s_res=None, i_res=None,):
  self.s_res = s_res
  self.i_res = i_res

DoSomethingResult.__init__ = DoSomethingResult__init__

def DoSomethingResult__setstate__(self, state):
  state.setdefault('s_res', None)
  state.setdefault('i_res', None)
  self.__dict__ = state

DoSomethingResult.__getstate__ = lambda self: self.__dict__.copy()
DoSomethingResult.__setstate__ = DoSomethingResult__setstate__

fix_spec(all_structs)
del all_structs
