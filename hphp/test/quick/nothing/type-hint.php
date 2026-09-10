<?hh

function my_handler(int $errno, string $errstr, string $file, int $line) :mixed{
  throw new Exception($errstr);
}

function try_takes_nothing(mixed $x) :void{
  try {
    takes_nothing(HH\FIXME\UNSAFE_CAST<mixed, nothing>(
      $x,
      'Runtime enforcement is the behavior under test',
    ));
  } catch (Exception $e) {
    echo $e->getMessage(), "\n";
  }
}

function takes_nothing(nothing $x) :void{
  var_dump($x);
}

function main() :void{
  try_takes_nothing(42);
  try_takes_nothing(3.14);
  try_takes_nothing('abc');
  try_takes_nothing(true);
  try_takes_nothing(false);
  try_takes_nothing(new stdClass());
  try_takes_nothing(null);
}
<<__EntryPoint>> function main_entry(): void {
set_error_handler(my_handler<>);
main();
}
