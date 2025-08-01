#
# Autogenerated by Thrift
#
# DO NOT EDIT
#  @generated
#

from __future__ import annotations

import apache.thrift.metadata.thrift_types as _fbthrift_metadata

import s.thrift_enums as _fbthrift_current_module_enums
import s.thrift_enums


import b.thrift_enums
import b.thrift_metadata as _fbthrift__b__thrift_metadata

import c.thrift_enums
import c.thrift_metadata as _fbthrift__c__thrift_metadata


def gen_metadata_service_TestService() -> _fbthrift_metadata.ThriftMetadata:
    return _fbthrift_gen_metadata_service_TestService(_fbthrift_metadata.ThriftMetadata(structs={}, enums={}, exceptions={}, services={}))

def _fbthrift_gen_metadata_service_TestService(metadata_struct: _fbthrift_metadata.ThriftMetadata) -> _fbthrift_metadata.ThriftMetadata:
    qualified_name = "s.TestService"
    
    if qualified_name in metadata_struct.services:
        return metadata_struct
    
    functions = [
        _fbthrift_metadata.ThriftFunction(name="test", return_type=_fbthrift_metadata.ThriftType(t_primitive=_fbthrift_metadata.ThriftPrimitiveType.THRIFT_VOID_TYPE), arguments=[
        ], exceptions = [
            _fbthrift_metadata.ThriftField(id=1, type=_fbthrift_metadata.ThriftType(t_struct=_fbthrift_metadata.ThriftStructType(name="c.E")), name="ex", is_optional=False, structured_annotations=[
            ]),
        ], is_oneway=False, structured_annotations=[
        ]),
    ]
    
    service_dict = dict(metadata_struct.services)
    service_dict[qualified_name] = _fbthrift_metadata.ThriftService(name=qualified_name, functions=functions,  structured_annotations=[
    ])
    new_struct = metadata_struct(services=service_dict)
    
    
    new_struct = c.thrift_metadata._fbthrift_gen_metadata_exception_E(new_struct) # ex
    
     # return value
    
    
    return new_struct

def _fbthrift_metadata_service_response_TestService() -> _fbthrift_metadata.ThriftServiceMetadataResponse:
    metadata = gen_metadata_service_TestService()
    context = _fbthrift_metadata.ThriftServiceContext(service_info=metadata.services["s.TestService"], module=_fbthrift_metadata.ThriftModuleContext(name="s"))
    services = [_fbthrift_metadata.ThriftServiceContextRef(module=_fbthrift_metadata.ThriftModuleContext(name=name.split('.')[0]), service_name=name) for name in metadata.services]
    return _fbthrift_metadata.ThriftServiceMetadataResponse(metadata=metadata,context=context,services=services)



def getThriftModuleMetadata() -> _fbthrift_metadata.ThriftMetadata:
    meta = _fbthrift_metadata.ThriftMetadata(structs={}, enums={}, exceptions={}, services={})
    meta = _fbthrift_gen_metadata_service_TestService(meta)
    return meta
