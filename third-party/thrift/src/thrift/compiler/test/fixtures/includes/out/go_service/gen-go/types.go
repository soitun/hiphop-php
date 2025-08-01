// Autogenerated by Thrift for thrift/compiler/test/fixtures/includes/src/service.thrift
//
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
//  @generated

package service

import (
    "fmt"
    "reflect"

    module "module"
    includes "includes"
    thrift "github.com/facebook/fbthrift/thrift/lib/go/thrift/types"
)

// (needed to ensure safety because of naive import list construction)
var _ = module.GoUnusedProtection__
var _ = includes.GoUnusedProtection__
var _ = fmt.Printf
var _ = reflect.Ptr
var _ = thrift.VOID

type IncludesIncluded = includes.Included

func NewIncludesIncluded() *IncludesIncluded {
    return includes.NewIncluded()
}

func WriteIncludesIncluded(item *IncludesIncluded, p thrift.Encoder) error {
    if err := item.Write(p); err != nil {
        return err
    }
    return nil
}

func ReadIncludesIncluded(p thrift.Decoder) (*IncludesIncluded, error) {
    var decodeResult *IncludesIncluded
    decodeErr := func() error {
        result := includes.NewIncluded()
        err := result.Read(p)
        if err != nil {
            return err
        }
        decodeResult = result
        return nil
    }()
    return decodeResult, decodeErr
}

type IncludesTransitiveFoo = includes.TransitiveFoo

func NewIncludesTransitiveFoo() *IncludesTransitiveFoo {
    return includes.NewTransitiveFoo()
}

func WriteIncludesTransitiveFoo(item *IncludesTransitiveFoo, p thrift.Encoder) error {
    err := includes.WriteTransitiveFoo(item, p)
    if err != nil {
        return err
    }
    return nil
}

func ReadIncludesTransitiveFoo(p thrift.Decoder) (*IncludesTransitiveFoo, error) {
    var decodeResult *IncludesTransitiveFoo
    decodeErr := func() error {
        result, err := includes.ReadTransitiveFoo(p)
        if err != nil {
            return err
        }
        decodeResult = result
        return nil
    }()
    return decodeResult, decodeErr
}

type reqMyServiceQuery struct {
    S *module.MyStruct `thrift:"s,1" json:"s" db:"s"`
    I *includes.Included `thrift:"i,2" json:"i" db:"i"`
}
// Compile time interface enforcer
var _ thrift.Struct = (*reqMyServiceQuery)(nil)

// Deprecated: MyServiceQueryArgsDeprecated is deprecated, since it is supposed to be internal.
type MyServiceQueryArgsDeprecated = reqMyServiceQuery

func newReqMyServiceQuery() *reqMyServiceQuery {
    return (&reqMyServiceQuery{}).setDefaults()
}

func (x *reqMyServiceQuery) GetS() *module.MyStruct {
    if !x.IsSetS() {
        return nil
    }
    return x.S
}

func (x *reqMyServiceQuery) GetI() *includes.Included {
    if !x.IsSetI() {
        return nil
    }
    return x.I
}

func (x *reqMyServiceQuery) SetSNonCompat(value *module.MyStruct) *reqMyServiceQuery {
    x.S = value
    return x
}

func (x *reqMyServiceQuery) SetS(value *module.MyStruct) *reqMyServiceQuery {
    x.S = value
    return x
}

func (x *reqMyServiceQuery) SetINonCompat(value *includes.Included) *reqMyServiceQuery {
    x.I = value
    return x
}

func (x *reqMyServiceQuery) SetI(value *includes.Included) *reqMyServiceQuery {
    x.I = value
    return x
}

func (x *reqMyServiceQuery) IsSetS() bool {
    return x != nil && x.S != nil
}

func (x *reqMyServiceQuery) IsSetI() bool {
    return x != nil && x.I != nil
}

func (x *reqMyServiceQuery) writeField1(p thrift.Encoder) error {  // S
    if !x.IsSetS() {
        return nil
    }

    if err := p.WriteFieldBegin("s", thrift.STRUCT, 1); err != nil {
        return thrift.PrependError("reqMyServiceQuery write field begin error: ", err)
    }

    item := x.S
    if err := item.Write(p); err != nil {
        return err
    }

    if err := p.WriteFieldEnd(); err != nil {
        return thrift.PrependError("reqMyServiceQuery write field end error: ", err)
    }
    return nil
}

func (x *reqMyServiceQuery) writeField2(p thrift.Encoder) error {  // I
    if !x.IsSetI() {
        return nil
    }

    if err := p.WriteFieldBegin("i", thrift.STRUCT, 2); err != nil {
        return thrift.PrependError("reqMyServiceQuery write field begin error: ", err)
    }

    item := x.I
    if err := item.Write(p); err != nil {
        return err
    }

    if err := p.WriteFieldEnd(); err != nil {
        return thrift.PrependError("reqMyServiceQuery write field end error: ", err)
    }
    return nil
}

func (x *reqMyServiceQuery) readField1(p thrift.Decoder) error {  // S
    result := module.NewMyStruct()
    err := result.Read(p)
    if err != nil {
        return err
    }

    x.S = result
    return nil
}

func (x *reqMyServiceQuery) readField2(p thrift.Decoder) error {  // I
    result := includes.NewIncluded()
    err := result.Read(p)
    if err != nil {
        return err
    }

    x.I = result
    return nil
}





func (x *reqMyServiceQuery) Write(p thrift.Encoder) error {
    if err := p.WriteStructBegin("reqMyServiceQuery"); err != nil {
        return thrift.PrependError("reqMyServiceQuery write struct begin error: ", err)
    }

    if err := x.writeField1(p); err != nil {
        return err
    }
    if err := x.writeField2(p); err != nil {
        return err
    }

    if err := p.WriteFieldStop(); err != nil {
        return thrift.PrependError("reqMyServiceQuery write field stop error: ", err)
    }

    if err := p.WriteStructEnd(); err != nil {
        return thrift.PrependError("reqMyServiceQuery write struct end error: ", err)
    }
    return nil
}

func (x *reqMyServiceQuery) Read(p thrift.Decoder) error {
    if _, err := p.ReadStructBegin(); err != nil {
        return thrift.PrependError("reqMyServiceQuery read error: ", err)
    }

    for {
        fieldName, wireType, id, err := p.ReadFieldBegin()
        if err != nil {
            return thrift.PrependError(fmt.Sprintf("reqMyServiceQuery field %d ('%s') read error: ", id, fieldName), err)
        }

        if wireType == thrift.STOP {
            break;
        }

        var fieldReadErr error
        switch {
        case ((id == 1 && wireType == thrift.STRUCT) || (id == thrift.NO_FIELD_ID && fieldName == "s")):  // s
            fieldReadErr = x.readField1(p)
        case ((id == 2 && wireType == thrift.STRUCT) || (id == thrift.NO_FIELD_ID && fieldName == "i")):  // i
            fieldReadErr = x.readField2(p)
        default:
            fieldReadErr = p.Skip(wireType)
        }

        if fieldReadErr != nil {
            return fieldReadErr
        }

        if err := p.ReadFieldEnd(); err != nil {
            return err
        }
    }

    if err := p.ReadStructEnd(); err != nil {
        return thrift.PrependError("reqMyServiceQuery read struct end error: ", err)
    }

    return nil
}

func (x *reqMyServiceQuery) String() string {
    return thrift.StructToString(reflect.ValueOf(x))
}

func (x *reqMyServiceQuery) setDefaults() *reqMyServiceQuery {
    return x.
        SetSNonCompat(module.NewMyStruct()).
        SetINonCompat(includes.NewIncluded())
}

type respMyServiceQuery struct {
}
// Compile time interface enforcer
var _ thrift.Struct = (*respMyServiceQuery)(nil)
var _ thrift.WritableResult = (*respMyServiceQuery)(nil)

// Deprecated: MyServiceQueryResultDeprecated is deprecated, since it is supposed to be internal.
type MyServiceQueryResultDeprecated = respMyServiceQuery

func newRespMyServiceQuery() *respMyServiceQuery {
    return (&respMyServiceQuery{}).setDefaults()
}



func (x *respMyServiceQuery) Exception() thrift.WritableException {
    return nil
}

func (x *respMyServiceQuery) Write(p thrift.Encoder) error {
    if err := p.WriteStructBegin("respMyServiceQuery"); err != nil {
        return thrift.PrependError("respMyServiceQuery write struct begin error: ", err)
    }


    if err := p.WriteFieldStop(); err != nil {
        return thrift.PrependError("respMyServiceQuery write field stop error: ", err)
    }

    if err := p.WriteStructEnd(); err != nil {
        return thrift.PrependError("respMyServiceQuery write struct end error: ", err)
    }
    return nil
}

func (x *respMyServiceQuery) Read(p thrift.Decoder) error {
    if _, err := p.ReadStructBegin(); err != nil {
        return thrift.PrependError("respMyServiceQuery read error: ", err)
    }

    for {
        fieldName, wireType, id, err := p.ReadFieldBegin()
        if err != nil {
            return thrift.PrependError(fmt.Sprintf("respMyServiceQuery field %d ('%s') read error: ", id, fieldName), err)
        }

        if wireType == thrift.STOP {
            break;
        }

        var fieldReadErr error
        switch {
        default:
            fieldReadErr = p.Skip(wireType)
        }

        if fieldReadErr != nil {
            return fieldReadErr
        }

        if err := p.ReadFieldEnd(); err != nil {
            return err
        }
    }

    if err := p.ReadStructEnd(); err != nil {
        return thrift.PrependError("respMyServiceQuery read struct end error: ", err)
    }

    return nil
}

func (x *respMyServiceQuery) String() string {
    return thrift.StructToString(reflect.ValueOf(x))
}

func (x *respMyServiceQuery) setDefaults() *respMyServiceQuery {
    return x
}

type reqMyServiceHasArgDocs struct {
    S *module.MyStruct `thrift:"s,1" json:"s" db:"s"`
    I *includes.Included `thrift:"i,2" json:"i" db:"i"`
}
// Compile time interface enforcer
var _ thrift.Struct = (*reqMyServiceHasArgDocs)(nil)

// Deprecated: MyServiceHasArgDocsArgsDeprecated is deprecated, since it is supposed to be internal.
type MyServiceHasArgDocsArgsDeprecated = reqMyServiceHasArgDocs

func newReqMyServiceHasArgDocs() *reqMyServiceHasArgDocs {
    return (&reqMyServiceHasArgDocs{}).setDefaults()
}

func (x *reqMyServiceHasArgDocs) GetS() *module.MyStruct {
    if !x.IsSetS() {
        return nil
    }
    return x.S
}

func (x *reqMyServiceHasArgDocs) GetI() *includes.Included {
    if !x.IsSetI() {
        return nil
    }
    return x.I
}

func (x *reqMyServiceHasArgDocs) SetSNonCompat(value *module.MyStruct) *reqMyServiceHasArgDocs {
    x.S = value
    return x
}

func (x *reqMyServiceHasArgDocs) SetS(value *module.MyStruct) *reqMyServiceHasArgDocs {
    x.S = value
    return x
}

func (x *reqMyServiceHasArgDocs) SetINonCompat(value *includes.Included) *reqMyServiceHasArgDocs {
    x.I = value
    return x
}

func (x *reqMyServiceHasArgDocs) SetI(value *includes.Included) *reqMyServiceHasArgDocs {
    x.I = value
    return x
}

func (x *reqMyServiceHasArgDocs) IsSetS() bool {
    return x != nil && x.S != nil
}

func (x *reqMyServiceHasArgDocs) IsSetI() bool {
    return x != nil && x.I != nil
}

func (x *reqMyServiceHasArgDocs) writeField1(p thrift.Encoder) error {  // S
    if !x.IsSetS() {
        return nil
    }

    if err := p.WriteFieldBegin("s", thrift.STRUCT, 1); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write field begin error: ", err)
    }

    item := x.S
    if err := item.Write(p); err != nil {
        return err
    }

    if err := p.WriteFieldEnd(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write field end error: ", err)
    }
    return nil
}

func (x *reqMyServiceHasArgDocs) writeField2(p thrift.Encoder) error {  // I
    if !x.IsSetI() {
        return nil
    }

    if err := p.WriteFieldBegin("i", thrift.STRUCT, 2); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write field begin error: ", err)
    }

    item := x.I
    if err := item.Write(p); err != nil {
        return err
    }

    if err := p.WriteFieldEnd(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write field end error: ", err)
    }
    return nil
}

func (x *reqMyServiceHasArgDocs) readField1(p thrift.Decoder) error {  // S
    result := module.NewMyStruct()
    err := result.Read(p)
    if err != nil {
        return err
    }

    x.S = result
    return nil
}

func (x *reqMyServiceHasArgDocs) readField2(p thrift.Decoder) error {  // I
    result := includes.NewIncluded()
    err := result.Read(p)
    if err != nil {
        return err
    }

    x.I = result
    return nil
}





func (x *reqMyServiceHasArgDocs) Write(p thrift.Encoder) error {
    if err := p.WriteStructBegin("reqMyServiceHasArgDocs"); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write struct begin error: ", err)
    }

    if err := x.writeField1(p); err != nil {
        return err
    }
    if err := x.writeField2(p); err != nil {
        return err
    }

    if err := p.WriteFieldStop(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write field stop error: ", err)
    }

    if err := p.WriteStructEnd(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs write struct end error: ", err)
    }
    return nil
}

func (x *reqMyServiceHasArgDocs) Read(p thrift.Decoder) error {
    if _, err := p.ReadStructBegin(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs read error: ", err)
    }

    for {
        fieldName, wireType, id, err := p.ReadFieldBegin()
        if err != nil {
            return thrift.PrependError(fmt.Sprintf("reqMyServiceHasArgDocs field %d ('%s') read error: ", id, fieldName), err)
        }

        if wireType == thrift.STOP {
            break;
        }

        var fieldReadErr error
        switch {
        case ((id == 1 && wireType == thrift.STRUCT) || (id == thrift.NO_FIELD_ID && fieldName == "s")):  // s
            fieldReadErr = x.readField1(p)
        case ((id == 2 && wireType == thrift.STRUCT) || (id == thrift.NO_FIELD_ID && fieldName == "i")):  // i
            fieldReadErr = x.readField2(p)
        default:
            fieldReadErr = p.Skip(wireType)
        }

        if fieldReadErr != nil {
            return fieldReadErr
        }

        if err := p.ReadFieldEnd(); err != nil {
            return err
        }
    }

    if err := p.ReadStructEnd(); err != nil {
        return thrift.PrependError("reqMyServiceHasArgDocs read struct end error: ", err)
    }

    return nil
}

func (x *reqMyServiceHasArgDocs) String() string {
    return thrift.StructToString(reflect.ValueOf(x))
}

func (x *reqMyServiceHasArgDocs) setDefaults() *reqMyServiceHasArgDocs {
    return x.
        SetSNonCompat(module.NewMyStruct()).
        SetINonCompat(includes.NewIncluded())
}

type respMyServiceHasArgDocs struct {
}
// Compile time interface enforcer
var _ thrift.Struct = (*respMyServiceHasArgDocs)(nil)
var _ thrift.WritableResult = (*respMyServiceHasArgDocs)(nil)

// Deprecated: MyServiceHasArgDocsResultDeprecated is deprecated, since it is supposed to be internal.
type MyServiceHasArgDocsResultDeprecated = respMyServiceHasArgDocs

func newRespMyServiceHasArgDocs() *respMyServiceHasArgDocs {
    return (&respMyServiceHasArgDocs{}).setDefaults()
}



func (x *respMyServiceHasArgDocs) Exception() thrift.WritableException {
    return nil
}

func (x *respMyServiceHasArgDocs) Write(p thrift.Encoder) error {
    if err := p.WriteStructBegin("respMyServiceHasArgDocs"); err != nil {
        return thrift.PrependError("respMyServiceHasArgDocs write struct begin error: ", err)
    }


    if err := p.WriteFieldStop(); err != nil {
        return thrift.PrependError("respMyServiceHasArgDocs write field stop error: ", err)
    }

    if err := p.WriteStructEnd(); err != nil {
        return thrift.PrependError("respMyServiceHasArgDocs write struct end error: ", err)
    }
    return nil
}

func (x *respMyServiceHasArgDocs) Read(p thrift.Decoder) error {
    if _, err := p.ReadStructBegin(); err != nil {
        return thrift.PrependError("respMyServiceHasArgDocs read error: ", err)
    }

    for {
        fieldName, wireType, id, err := p.ReadFieldBegin()
        if err != nil {
            return thrift.PrependError(fmt.Sprintf("respMyServiceHasArgDocs field %d ('%s') read error: ", id, fieldName), err)
        }

        if wireType == thrift.STOP {
            break;
        }

        var fieldReadErr error
        switch {
        default:
            fieldReadErr = p.Skip(wireType)
        }

        if fieldReadErr != nil {
            return fieldReadErr
        }

        if err := p.ReadFieldEnd(); err != nil {
            return err
        }
    }

    if err := p.ReadStructEnd(); err != nil {
        return thrift.PrependError("respMyServiceHasArgDocs read struct end error: ", err)
    }

    return nil
}

func (x *respMyServiceHasArgDocs) String() string {
    return thrift.StructToString(reflect.ValueOf(x))
}

func (x *respMyServiceHasArgDocs) setDefaults() *respMyServiceHasArgDocs {
    return x
}


