<?hh

class A {
  <<__LSB>> private static string $x = "hello";
  <<__LSB>> protected static int $y = 123;
  <<__LSB>> public static vec<int> $z = vec[1,2,3];

  static public function dump() :void{
    var_dump(static::$x);
    var_dump(static::$y);
    var_dump(static::$z);
    static::$x = "world";
    static::$y = 1234;
    static::$z[] = 4;
  }
}

class B extends A {
}

class C extends B {
}
<<__EntryPoint>> function main(): void {
A::dump();
B::dump();
C::dump();

A::dump();
B::dump();
C::dump();
}
