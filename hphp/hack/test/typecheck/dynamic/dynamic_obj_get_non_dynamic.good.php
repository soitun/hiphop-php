<?hh

function property_name(): string {
  return 'foo';
}

<<__SupportDynamicType>>
class C {}

function make_like<T>(T $value): ~T {
  return $value;
}

function dynamic_receiver(dynamic $object): void {
  $object->{property_name()};
}

function generic_dynamic_receiver<T as dynamic>(T $object): void {
  $object->{property_name()};
}

function generic_transitive_dynamic_receiver<T as Tu, Tu as dynamic>(
  T $object,
): void {
  $object->{property_name()};
}

function nullsafe_dynamic_receiver(dynamic $object): void {
  $object?->{property_name()};
}

function nullsafe_nullable_dynamic_receiver(?dynamic $object): void {
  $object?->{property_name()};
}

function nothing_source(): nothing {
  throw new Exception();
}

function nothing_receiver(string $property): void {
  nothing_source()->$property;
}

function dynamic_dict_receiver(
  (dynamic & dict<string, int>) $dict,
): void {
  $dict->{property_name()};
}

function dynamic_int_receiver((dynamic & int) $value): void {
  $value->{property_name()};
}

function dynamic_nonnull_receiver((dynamic & nonnull) $object): void {
  $object->{property_name()};
}

function normalized_dynamic_receiver((dynamic & ~nonnull) $object): void {
  $object->{property_name()};
}

function dynamic_nonnull_variable_receiver(
  (dynamic & nonnull) $value,
  string $property,
): void {
  $value->$property;
}

function captured_like_dynamic_intersection_receiver(
  dynamic $result,
  string $property,
): void {
  $object = make_like($result['object'] as C);
  $object->{property_name()};
  $object->$property;
}
