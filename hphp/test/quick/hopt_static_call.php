<?hh
class AA {
  public static function f1(string $a1) :void{ print "Pass\n"; }
  public static function f0(string $a1) :void{ AA::f1($a1); }
}
<<__EntryPoint>> function main(): void {
AA::f0("Hello World\n");
}
