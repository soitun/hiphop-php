<?hh

abstract class A {
  abstract public function foo(): int;

  public function bar(): int {
    return $this->foo();
  }
}

abstract class B extends A {
  public function baz(): int {
    return $this->bar();
  }
}

class B1 extends B {
  <<__Override>>
  public function foo(): int {
    return 10;
  }
}

class B2 extends B {
  <<__Override>>
  public function foo(): int {
    return 11;
  }
}

class C1 extends A {
  <<__Override>>
  public function foo(): int {
    return 100;
  }
}

class C2 extends A {
  <<__Override>>
  public function foo(): int {
    return 110;
  }
}

function bar(A $a): void {
  var_dump($a->bar());
}

function fox(B $b): int {
  return $b->baz();
}

<<__EntryPoint>>
function main() : void {
  bar(new C1());
  bar(new C2());
  bar(new B1());
  fox(new B1());
}
