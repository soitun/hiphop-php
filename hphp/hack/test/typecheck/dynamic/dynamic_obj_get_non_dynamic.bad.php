<?hh

<<__SupportDynamicType>>
class C {
  public int $foo = 0;
}

function property_name(): string {
  return 'foo';
}

function method_name(): string {
  return 'method';
}

// Typing[4525]: computed member expressions on non-dynamic receivers.
function dict_receiver(dict<string, int> $dict): void {
  $dict->{property_name()};
}

function dict_write(dict<string, int> $dict): void {
  $dict->{property_name()} = 1;
}

function dict_method(dict<string, int> $dict): void {
  $dict->{method_name()}();
}

function shape_receiver(shape('foo' => int) $shape): void {
  $shape->{property_name()};
}

function union_receiver((C | dict<string, int>) $value): void {
  $value->{property_name()};
}

function like_dict_receiver(~dict<string, int> $dict): void {
  $dict->{property_name()};
}

function generic_like_dict_receiver<T as ~dict<string, int>>(T $dict): void {
  $dict->{property_name()};
}

function nullable_object_receiver(?C $object): void {
  $object->{property_name()};
}

function nullsafe_dict_receiver(?dict<string, int> $dict): void {
  $dict?->{property_name()};
}

function nullsafe_object_receiver(?C $object): void {
  $object?->{property_name()};
}

function object_receiver(C $object): void {
  $object->{property_name()};
}

function like_object_receiver(~C $object): void {
  $object->{property_name()};
}

function generic_like_object_receiver<T as ~C>(T $object): void {
  $object->{property_name()};
}

function generic_cycle_receiver<T as Tu, Tu as T>(T $object): void {
  $object->{property_name()};
}

function nullable_dynamic_receiver(?dynamic $object): void {
  $object->{property_name()};
}

function tany_source() {}

function tany_receiver(string $property): void {
  tany_source()->$property;
}

class ErrorTyvarReceiver {}

function error_tyvar_receiver(
  ErrorTyvarReceiver $object,
  string $property,
): void {
  $object->missing->$property;
}

function dynamic_nonnull_union_receiver(
  ((dynamic & nonnull) | C) $object,
): void {
  $object->{property_name()};
}

// Typing[4525]: local-variable member names on non-dynamic receivers.
function dict_variable_receiver(
  dict<string, int> $dict,
  string $property,
): void {
  $dict->$property;
}

function mixed_variable_receiver(mixed $value, string $property): void {
  $value->$property;
}

function nonnull_variable_receiver(
  nonnull $value,
  string $property,
): void {
  $value->$property;
}
