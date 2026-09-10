<?hh
class Foo {

  private static mixed $barX;
  public static function bar() :void{
  }
}
<<__EntryPoint>> function main(): void {
var_dump((new ReflectionClass('Foo'))->isInstantiable());
}
