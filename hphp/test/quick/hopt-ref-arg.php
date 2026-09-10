<?hh

function set(inout mixed $b) :void{
  $b = 3;
}

function run(inout mixed $a) :mixed{
  set(inout $a);
  return $a;
}
<<__EntryPoint>> function main(): void {
$a = 5;
run(inout $a);
var_dump($a);
}
