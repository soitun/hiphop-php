// Autogenerated by Thrift for thrift/compiler/test/fixtures/interactions/src/module.thrift
//
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
//  @generated

package module

import (
    "maps"

    shared "shared"
    thrift "github.com/facebook/fbthrift/thrift/lib/go/thrift/types"
    metadata "github.com/facebook/fbthrift/thrift/lib/thrift/metadata"
)

// (needed to ensure safety because of naive import list construction)
var _ = shared.GoUnusedProtection__
var _ = thrift.VOID
var _ = maps.Copy[map[int]int, map[int]int]
var _ = metadata.GoUnusedProtection__

// Premade Thrift types
var (
    premadeThriftType_string =
        &metadata.ThriftType{
            TPrimitive:
                thrift.Pointerize(metadata.ThriftPrimitiveType_THRIFT_STRING_TYPE),
        }
    premadeThriftType_module_ShouldBeBoxed =
        &metadata.ThriftType{
            TStruct:
                &metadata.ThriftStructType{
                    Name: "module.ShouldBeBoxed",
                },
        }
    premadeThriftType_module_CustomException =
        &metadata.ThriftType{
            TStruct:
                &metadata.ThriftStructType{
                    Name: "module.CustomException",
                },
        }
    premadeThriftType_void =
        &metadata.ThriftType{
            TPrimitive:
                thrift.Pointerize(metadata.ThriftPrimitiveType_THRIFT_VOID_TYPE),
        }
)

var premadeThriftTypesMap = func() map[string]*metadata.ThriftType {
    fbthriftThriftTypesMap := make(map[string]*metadata.ThriftType)
    fbthriftThriftTypesMap["string"] = premadeThriftType_string
    fbthriftThriftTypesMap["module.ShouldBeBoxed"] = premadeThriftType_module_ShouldBeBoxed
    fbthriftThriftTypesMap["module.CustomException"] = premadeThriftType_module_CustomException
    fbthriftThriftTypesMap["void"] = premadeThriftType_void
    return fbthriftThriftTypesMap
}()

var structMetadatas = func() []*metadata.ThriftStruct {
    fbthriftResults := make([]*metadata.ThriftStruct, 0)
    func() {
        fbthriftResults = append(fbthriftResults,
            &metadata.ThriftStruct{
                Name:    "module.ShouldBeBoxed",
                IsUnion: false,
                Fields:  []*metadata.ThriftField{
                    &metadata.ThriftField{
                        Id:         1,
                        Name:       "sessionId",
                        IsOptional: false,
                        Type:       premadeThriftType_string,
                    },
                },
            },
        )
    }()
    return fbthriftResults
}()

var exceptionMetadatas = func() []*metadata.ThriftException {
    fbthriftResults := make([]*metadata.ThriftException, 0)
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftException{
            Name: "module.CustomException",
            Fields: []*metadata.ThriftField{
                &metadata.ThriftField{
                    Id:         1,
                    Name:       "message",
                    IsOptional: false,
                    Type:       premadeThriftType_string,
                },
            },
        },
    )
    return fbthriftResults
}()

var enumMetadatas = func() []*metadata.ThriftEnum {
    fbthriftResults := make([]*metadata.ThriftEnum, 0)
    return fbthriftResults
}()

var serviceMetadatas = func() []*metadata.ThriftService {
    fbthriftResults := make([]*metadata.ThriftService, 0)
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftService{
            Name:      "module.MyService",
            Functions: []*metadata.ThriftFunction{
                &metadata.ThriftFunction{
                    Name:       "foo",
                    IsOneway:   false,
                    ReturnType: premadeThriftType_void,
                },
            },
        },
    )
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftService{
            Name:      "module.Factories",
            Functions: []*metadata.ThriftFunction{
                &metadata.ThriftFunction{
                    Name:       "foo",
                    IsOneway:   false,
                    ReturnType: premadeThriftType_void,
                },
            },
        },
    )
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftService{
            Name:      "module.Perform",
            Functions: []*metadata.ThriftFunction{
                &metadata.ThriftFunction{
                    Name:       "foo",
                    IsOneway:   false,
                    ReturnType: premadeThriftType_void,
                },
            },
        },
    )
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftService{
            Name:      "module.InteractWithShared",
            Functions: []*metadata.ThriftFunction{
                &metadata.ThriftFunction{
                    Name:       "do_some_similar_things",
                    IsOneway:   false,
                    ReturnType: shared.GetMetadataThriftType("shared.DoSomethingResult"),
                },
            },
        },
    )
    fbthriftResults = append(fbthriftResults,
        &metadata.ThriftService{
            Name:      "module.BoxService",
            Functions: []*metadata.ThriftFunction{
            },
        },
    )
    return fbthriftResults
}()

// Thrift metadata for this package, as well as all of its recursive imports.
var packageThriftMetadata = func() *metadata.ThriftMetadata {
    allEnumsMap := make(map[string]*metadata.ThriftEnum)
    allStructsMap := make(map[string]*metadata.ThriftStruct)
    allExceptionsMap := make(map[string]*metadata.ThriftException)
    allServicesMap := make(map[string]*metadata.ThriftService)

    // Add enum metadatas from the current program...
    for _, enumMetadata := range enumMetadatas {
        allEnumsMap[enumMetadata.GetName()] = enumMetadata
    }
    // Add struct metadatas from the current program...
    for _, structMetadata := range structMetadatas {
        allStructsMap[structMetadata.GetName()] = structMetadata
    }
    // Add exception metadatas from the current program...
    for _, exceptionMetadata := range exceptionMetadatas {
        allExceptionsMap[exceptionMetadata.GetName()] = exceptionMetadata
    }
    // Add service metadatas from the current program...
    for _, serviceMetadata := range serviceMetadatas {
        allServicesMap[serviceMetadata.GetName()] = serviceMetadata
    }

    // Obtain Thrift metadatas from recursively included programs...
    var recursiveThriftMetadatas []*metadata.ThriftMetadata
    recursiveThriftMetadatas = append(recursiveThriftMetadatas, shared.GetThriftMetadata())

    // ...now merge metadatas from recursively included programs.
    for _, thriftMetadata := range recursiveThriftMetadatas {
        maps.Copy(allEnumsMap, thriftMetadata.GetEnums())
        maps.Copy(allStructsMap, thriftMetadata.GetStructs())
        maps.Copy(allExceptionsMap, thriftMetadata.GetExceptions())
        maps.Copy(allServicesMap, thriftMetadata.GetServices())
    }

    return metadata.NewThriftMetadata().
        SetEnums(allEnumsMap).
        SetStructs(allStructsMap).
        SetExceptions(allExceptionsMap).
        SetServices(allServicesMap)
}()

// GetMetadataThriftType (INTERNAL USE ONLY).
// Returns metadata ThriftType for a given full type name.
func GetMetadataThriftType(fullName string) *metadata.ThriftType {
    return premadeThriftTypesMap[fullName]
}

// GetThriftMetadata returns complete Thrift metadata for current and imported packages.
func GetThriftMetadata() *metadata.ThriftMetadata {
    return packageThriftMetadata
}

// GetThriftMetadataForService returns Thrift metadata for the given service.
func GetThriftMetadataForService(scopedServiceName string) *metadata.ThriftMetadata {
    allServicesMap := packageThriftMetadata.GetServices()
    relevantServicesMap := make(map[string]*metadata.ThriftService)

    serviceMetadata := allServicesMap[scopedServiceName]
    // Visit and record all recursive parents of the target service.
    for serviceMetadata != nil {
        relevantServicesMap[serviceMetadata.GetName()] = serviceMetadata
        if serviceMetadata.IsSetParent() {
            serviceMetadata = allServicesMap[serviceMetadata.GetParent()]
        } else {
            serviceMetadata = nil
        }
    }

    return metadata.NewThriftMetadata().
        SetEnums(packageThriftMetadata.GetEnums()).
        SetStructs(packageThriftMetadata.GetStructs()).
        SetExceptions(packageThriftMetadata.GetExceptions()).
        SetServices(relevantServicesMap)
}
