<?hh

<<__DynamicallyCallable>> function a() :mixed{
  echo "Hello from a\n";
}

<<__DynamicallyCallable>> function b() :mixed{
  echo "I am b\n";
}

function callfns($name, $name2) :mixed{
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
