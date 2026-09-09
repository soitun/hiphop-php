<?hh

<<file: __EnableUnstableFeatures('class_type')>>

class A {
  public static function meth(): void {}
}

class B {
  public static function meth(): void {}
}

function union_write(class<A> $a, class<B> $b, bool $cond): void {
  $key = $cond ? $a : $b;
  $d = dict[];
  $d[$key] = 1;
  foreach ($d as $key => $_) {
    $key::meth();
  }
}
