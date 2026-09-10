<?hh

function foo(inout string $str) :void{
  $str[3] = '.';
  $str .= "\n";
}
<<__EntryPoint>> function main(): void {
$a = "abc";
foo(inout $a);
var_dump($a);
}
