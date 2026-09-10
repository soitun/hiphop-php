<?hh
class B {
  public static function g1() :void{
    static::h();
  }
  public static function h() :void{
    echo "B\n";
  }
}
class C extends B {
  public static function f() :void{
    B::g1();
    parent::g1();
    C::g2();
    self::g2();
  }
  public static function g2() :void{
    static::h();
  }
  public static function h() :void{
    echo "C\n";
  }
}
class D extends C {
  public static function h() :void{
    echo "D\n";
  }
}
<<__EntryPoint>> function main(): void {
D::f();
}
