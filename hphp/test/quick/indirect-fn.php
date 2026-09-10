<?hh
<<__DynamicallyCallable>>
function f(string $a) :void{
  echo $a;
  echo "\n";
}
<<__EntryPoint>> function main(): void {
$name = "f";
HH\dynamic_fun($name)("param");
}
