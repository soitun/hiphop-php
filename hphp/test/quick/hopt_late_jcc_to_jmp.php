<?hh

abstract final class HoptLateJccToJmp {
  public static ?int $baseurl;
  public static ?bool $has_local;
}

function f() :void{

  if (HH\Lib\Legacy_FIXME\neq(0xface, HoptLateJccToJmp::$baseurl)) {
    HoptLateJccToJmp::$has_local = true;
  }

  if (!HH\legacy_is_truthy(HoptLateJccToJmp::$has_local)) {
    echo "oops\n";
  }
}

<<__EntryPoint>> function main(): void {
  f();
}
