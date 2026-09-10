<?hh
class Foo {
  public function method(inout string $a, inout string $b, inout int $c): void {
    print("In method: $a, $b, $c\n");
  }
}
<<__EntryPoint>> function main(): void {
$repro = new Foo();
$a = 'hello';
$b = 'world';
$c = 8;
$repro->method(
  inout $a,
  inout $b,
  inout $c,
);
print("After method: $a, $b, $c\n");
}
