<?hh
// Copyright 2004-2015 Facebook. All Rights Reserved.

class c {
  private static ?dict<string, string> $thing;
  private static ?dict<string, string> $otherthing;

  public static function doit(arraykey $id, string $value) :void{
    self::$thing = dict[];
    self::$thing[(string)$id] = $value;
    self::$otherthing = dict[];
    self::$otherthing[(string)$id] = $value;
  }

  public static function dump() :void{
    var_dump(self::$thing, self::$otherthing);
  }
}

function main() :void{
  c::doit(0, 'hello');
  c::dump();
}
<<__EntryPoint>> function main_entry(): void {
echo "Calling main\n";
main();
echo "Done\n";
}
