<?hh

function my_handler(int $errno, string $errstr, string $file, int $line) :mixed{
  throw new Exception($errstr);
}

function try_takes_null(mixed $x) :void{
  try {
    takes_null(HH\FIXME\UNSAFE_CAST<mixed, null>(
      $x,
      'Runtime enforcement is the behavior under test',
    ));
  } catch (Exception $e) {
    echo $e->getMessage(), "\n";
  }
}

function takes_null(null $x) :void{
  var_dump($x);
}

function main() :void{
  try_takes_null(null);
  try_takes_null(42);
  try_takes_null(3.14);
  try_takes_null('abc');
  try_takes_null(true);
  try_takes_null(false);
  try_takes_null(new stdClass());
}
<<__EntryPoint>> function main_entry(): void {
set_error_handler(my_handler<>);
main();
}
