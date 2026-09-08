<?hh
class Foo {}
function foo(
  inout readonly Foo $x // Readonly and inout are incompatible.
): void {
}
