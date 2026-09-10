<?hh

class blah {

  private static int $breakerX = 0;
  private function breaker(): ?vec<nothing>{
    $__lval_tmp_0 = self::$breakerX;
    self::$breakerX++;
    return $__lval_tmp_0== 0 ? vec[] : null;
  }

  public function foo() :void{
    $x = 0;
    $y = 0;

    if ($this->breaker() === NULL) {
      echo "hi\n";
    }
    echo "ok\n";
  }
}

<<__EntryPoint>> function main(): void {
  $x = new blah();
  $x->foo();
  $x->foo();
}
