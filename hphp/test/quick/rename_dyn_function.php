<?hh

<<__DynamicallyCallable>> function a() :void{
  echo "Hello from a\n";
}

<<__DynamicallyCallable>> function b() :void{
  echo "I am b\n";
}

function callfns(string $name, string $name2) :void{
  echo "Calling $name\n";
  HH\dynamic_fun($name)();
  echo "Calling $name2\n";
  HH\dynamic_fun($name2)();
}
<<__EntryPoint>> function main(): void {
callfns('a', 'b');
fb_rename_function('a', 'old_a');
fb_rename_function('b', 'a');
fb_rename_function('old_a', 'b');
callfns('a', 'b');
}
