<?hh

function show(arraykey $x) :void{
  echo $x;

  if(true) {
    echo "t\n";
  } else {
    echo "f\n";
  }
}
<<__EntryPoint>> function main(): void {
show(7);
show('test');
}
