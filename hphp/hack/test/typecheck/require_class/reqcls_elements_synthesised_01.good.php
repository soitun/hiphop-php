<?hh

trait T1 {
  public function foo(): void {}
}

trait T2 {
  use T1;
  require class C;
}

final class C {
  use T2;
  public function foo(): void {}
}
