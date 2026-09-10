<?hh

// ok, 'type' is context sensitive
function type() :void{
  echo "Hi\n";
}

class Foo {
  const TYPE = 'hi2';
}

type t = int;
function wat(t $type) :void{
  echo $type . "\n";
}
<<__EntryPoint>> function main(): void {
type();

echo Foo::TYPE . "\n";

wat(12);
}
