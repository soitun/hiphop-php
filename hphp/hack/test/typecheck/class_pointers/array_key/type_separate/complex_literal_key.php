////defs.php
<?hh

<<file: __EnableUnstableFeatures('class_type')>>

class A {
  public static function meth(): void {}
}
class B {
  public static function meth(): void {}
}

newtype OpaqueClsA as class<A> = class<A>;

////main.php
<?hh

<<file: __EnableUnstableFeatures('class_type')>>

function bare(class<A> $c): void {
  $d = dict[$c => 1];
}

function union_of_class_pointers(class<A> $a, class<B> $b, bool $cond): void {
  $c = $cond ? $a : $b;
  $d = dict[$c => 1];
}

function union_with_classname(class<A> $a, classname<B> $n, bool $cond): void {
  $c = $cond ? $a : $n;
  $d = dict[$c => 1];
  hh_show($d);
}

// `class_sub_classname`'s `class<A> <: string` collapses this before `coerce_to_name` sees it.
function union_with_string(class<A> $a, string $s, bool $cond): void {
  $c = $cond ? $a : $s;
  $d = dict[$c => 1];
  hh_show($d);
}

function generic_bound<T as class<A>>(T $c): void {
  $d = dict[$c => 1];
}

abstract class HasTypeConstant {
  abstract const type TCls as class<A>;

  public function key(this::TCls $c): void {
    $d = dict[$c => 1];
  }
}

function opaque_newtype(OpaqueClsA $c): void {
  $d = dict[$c => 1];
}
