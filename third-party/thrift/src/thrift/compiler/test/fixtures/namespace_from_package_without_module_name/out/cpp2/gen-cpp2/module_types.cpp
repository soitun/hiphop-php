/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/namespace_from_package_without_module_name/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */
#include "thrift/compiler/test/fixtures/namespace_from_package_without_module_name/gen-cpp2/module_types.h"
#include "thrift/compiler/test/fixtures/namespace_from_package_without_module_name/gen-cpp2/module_types.tcc"

#include <thrift/lib/cpp2/gen/module_types_cpp.h>

#include "thrift/compiler/test/fixtures/namespace_from_package_without_module_name/gen-cpp2/module_data.h"
[[maybe_unused]] static constexpr std::string_view kModuleName = "module";


namespace apache {
namespace thrift {
namespace detail {

void TccStructTraits<::test::namespace_from_package_without_module_name::Foo>::translateFieldName(
    std::string_view _fname,
    int16_t& fid,
    apache::thrift::protocol::TType& _ftype) noexcept {
  using data = apache::thrift::TStructDataStorage<::test::namespace_from_package_without_module_name::Foo>;
  static const st::translate_field_name_table table{
      data::fields_size,
      data::fields_names.data(),
      data::fields_ids.data(),
      data::fields_types.data()};
  st::translate_field_name(_fname, fid, _ftype, table);
}

} // namespace detail
} // namespace thrift
} // namespace apache

namespace test::namespace_from_package_without_module_name {

std::string_view Foo::__fbthrift_thrift_uri() {
  return "test.dev/namespace_from_package_without_module_name/Foo";
}

std::string_view Foo::__fbthrift_get_field_name(::apache::thrift::FieldOrdinal ord) {
  if (ord == ::apache::thrift::FieldOrdinal{0}) { return {}; }
  return apache::thrift::TStructDataStorage<Foo>::fields_names[folly::to_underlying(ord) - 1];
}
std::string_view Foo::__fbthrift_get_class_name() {
  return apache::thrift::TStructDataStorage<Foo>::name;
}


Foo::Foo(apache::thrift::FragileConstructor, ::std::int64_t MyInt__arg) :
    __fbthrift_field_MyInt(std::move(MyInt__arg)) { 
  __isset.set(folly::index_constant<0>(), true);
}


void Foo::__fbthrift_clear() {
  // clear all fields
  this->__fbthrift_field_MyInt = ::std::int64_t();
  __isset = {};
}

void Foo::__fbthrift_clear_terse_fields() {
}

bool Foo::__fbthrift_is_empty() const {
  return false;
}

bool Foo::operator==([[maybe_unused]] const Foo& rhs) const {
  return ::apache::thrift::op::detail::StructEquality{}(*this, rhs);
}

bool Foo::operator<([[maybe_unused]] const Foo& rhs) const {
  return ::apache::thrift::op::detail::StructLessThan{}(*this, rhs);
}


::std::int64_t Foo::get_MyInt() const {
  return __fbthrift_field_MyInt;
}

::std::int64_t& Foo::set_MyInt(::std::int64_t MyInt_) {
  MyInt_ref() = MyInt_;
  return __fbthrift_field_MyInt;
}

void swap([[maybe_unused]] Foo& a, [[maybe_unused]] Foo& b) {
  using ::std::swap;
  swap(a.__fbthrift_field_MyInt, b.__fbthrift_field_MyInt);
  swap(a.__isset, b.__isset);
}



} // namespace test::namespace_from_package_without_module_name

namespace test::namespace_from_package_without_module_name { namespace {
[[maybe_unused]] FOLLY_ERASE void validateAdapters() {
}
}} // namespace test::namespace_from_package_without_module_name
namespace apache::thrift::detail::annotation {
}
