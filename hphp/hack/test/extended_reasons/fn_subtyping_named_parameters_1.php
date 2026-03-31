<?hh

<<file: __EnableUnstableFeatures('named_parameters')>>

class A {
  public function example(int $x, named int $y, int $z): void {}
}

function main(): void {
  $a = new A();
  $a->example(y="", 3, 4);
}
