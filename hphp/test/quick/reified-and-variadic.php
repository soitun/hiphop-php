<?hh

class C {}

function reified_and_variadic<reify T>(mixed ...$vs) :void{
  foreach ($vs as $v) {
    var_dump($v);
  }
}

<<__EntryPoint>>
function main() :void{
  reified_and_variadic<bool>(new C(), new C());
}
