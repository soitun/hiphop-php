<?hh

function my_handler(int $errno, string $errstr, string $file, int $line) :mixed{
  throw new Exception($errstr);
}

function try_takes_nonnull(mixed $x) :void{
  try {
    takes_nonnull(HH\FIXME\UNSAFE_CAST<mixed, nonnull>(
      $x,
      'Runtime enforcement is the behavior under test',
    ));
  } catch (Exception $e) {
    echo $e->getMessage(), "\n";
  }
}

function takes_nonnull(nonnull $x) :void{
  var_dump($x);
}

function main() :void{
  try_takes_nonnull(42);
  try_takes_nonnull(3.14);
  try_takes_nonnull('abc');
  try_takes_nonnull(true);
  try_takes_nonnull(false);
  try_takes_nonnull(new stdClass());
  try_takes_nonnull(null); // nope: null
}
<<__EntryPoint>> function main_entry(): void {
set_error_handler(my_handler<>);
main();
}
