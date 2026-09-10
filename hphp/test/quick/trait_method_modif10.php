<?hh
trait T {
  abstract static public function f():mixed;
}
abstract class Base {
  use T;
}
abstract class Foo extends Base {
  abstract static public function f():mixed;
}
class Bar extends Foo {
  static public function f() :void{
    echo "Foo\n";
  }
}
<<__EntryPoint>> function main(): void {
Bar::f();
}
