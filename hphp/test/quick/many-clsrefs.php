<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

class Bar {
  public static ?int $prop1;
  public static ?int $prop2;
  public static ?int $prop3;
  public static ?int $prop4;
  public static ?int $prop5;
  public static ?int $prop6;
  public static ?int $prop7;
  public static ?int $prop8;
  public static ?int $prop9;
  public static ?int $prop10;

  static public function set((function(): int) $f) :void{
    self::$prop10 = $f();
    self::$prop9 = self::$prop10;
    self::$prop8 = self::$prop9;
    self::$prop7 = self::$prop8;
    self::$prop6 = self::$prop7;
    self::$prop5 = self::$prop6;
    self::$prop4 = self::$prop5;
    self::$prop3 = self::$prop4;
    self::$prop2 = self::$prop3;
    self::$prop1 = self::$prop2;
  }

  static public function dump() :void{
    var_dump(self::$prop1);
    var_dump(self::$prop2);
    var_dump(self::$prop3);
    var_dump(self::$prop4);
    var_dump(self::$prop5);
    var_dump(self::$prop6);
    var_dump(self::$prop7);
    var_dump(self::$prop8);
    var_dump(self::$prop9);
    var_dump(self::$prop10);
  }
}

function main() :void{
  Bar::set(() ==> 123);
  Bar::dump();
}
<<__EntryPoint>> function main_entry(): void {
main();
main();
}
