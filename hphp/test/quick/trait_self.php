<?hh

trait T {
  static public function foo() :void{ self::bar(); }
  static public function bar() :void{ var_dump(__METHOD__); }
}
<<__EntryPoint>> function main(): void {
T::foo();
}
