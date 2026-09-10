<?hh

<<file: __EnableUnstableFeatures('class_type')>>

class A {
  public static function meth(): void {}
}

function dict_write(class<A> $c): void {
  $d = dict[];
  $d[$c] = 1;
  foreach ($d as $k => $_) {
    $k::meth();
  }
}

function map_write(class<A> $c): void {
  $m = Map {};
  $m[$c] = 1;
  foreach ($m as $k => $_) {
    $k::meth();
  }
}

function nested_write(class<A> $c): void {
  $d = dict[];
  $d[$c] = dict[];
  $d[$c][$c] = 1;
  foreach ($d as $_ => $inner) {
    foreach ($inner as $k => $_) {
      $k::meth();
    }
  }
}

function annotated_key(dict<classname<A>, int> $d, class<A> $c): void {
  $d[$c] = 1;
}

function keyset_append(class<A> $c): void {
  $ks = keyset[];
  $ks[] = $c;
  foreach ($ks as $value) {
    $value::meth();
  }
}

function set_append(class<A> $c): void {
  $set = Set {};
  $set[] = $c;
  foreach ($set as $value) {
    $value::meth();
  }
}

function vec_append_unchanged(class<A> $c): void {
  $vec = vec[];
  $vec[] = $c;
  foreach ($vec as $value) {
    $value::meth();
  }
}
