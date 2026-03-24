<?hh

trait T {
  require class C;

  public function bar(): int { return $this->foo(); }
}
