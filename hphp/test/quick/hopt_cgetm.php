<?hh
// Copyright 2004-2015 Facebook. All Rights Reserved.

class C {
  public int $x;
  public function __construct(int $val) {
    $this->x = $val;
  }
  public function getX(): int{
    return $this->x;
  }
  public function incX(): int{
    $__lval_tmp_0 = $this->x;
    $this->x++;
    return $__lval_tmp_0;
  }
}


function foo(C $o):void{
  echo $o->getX();
  echo "\n";
  echo $o->incX();
  echo "\n";
  echo $o->getX();
  echo "\n";
}
<<__EntryPoint>> function main(): void {
$c = new C(2); // change to 4.0
foo($c);
}
