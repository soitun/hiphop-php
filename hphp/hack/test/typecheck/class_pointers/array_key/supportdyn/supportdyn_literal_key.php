<?hh

<<file: __EnableUnstableFeatures('class_type')>>

class A {
  public static function meth(): void {}
}

function wrapped(supportdyn<class<A>> $c): void {
  $d = dict[$c => 1];
  foreach ($d as $k => $_) {
    $k::meth();
  }
}

function bare(class<A> $c): void {
  $d = dict[$c => 1];
  foreach ($d as $k => $_) {
    $k::meth();
  }
}

function wrapped_keyset_element(supportdyn<class<A>> $c): void {
  $k = keyset[$c];
  foreach ($k as $e) {
    $e::meth();
  }
}
