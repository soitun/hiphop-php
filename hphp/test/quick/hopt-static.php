<?hh

class Foo {
  public static int $z = 0;

  public static function setZ(int $a) :void{
    Foo::$z = $a;
  }

  public static function getZ(): int{
    return Foo::$z;
  }
}
<<__EntryPoint>> function main(): void {
Foo::setZ(4);

var_dump(Foo::getZ());
}
