<?hh

function my_handler(int $errno, string $errstr, string $file, int $line) :mixed{
  throw new Exception($errstr);
}

function try_takes_arraykey(mixed $a) :void{
  try {
    takes_arraykey(HH\FIXME\UNSAFE_CAST<mixed, arraykey>(
      $a,
      'Runtime enforcement is the behavior under test',
    ));
  } catch (Exception $e) {
    echo $e->getMessage(), "\n";
  }
}

function takes_arraykey(arraykey $a) :void{
  var_dump($a);
}

function main() :void{
  try_takes_arraykey(10);
  try_takes_arraykey('abc');
  try_takes_arraykey('100');
  try_takes_arraykey('10.5');
  try_takes_arraykey(10.5); // nope: force explicit cast to int or string
  try_takes_arraykey(true); // nope: force explicit cast to int or string
  try_takes_arraykey(null); // nope: force explicit cast to int or string
  try_takes_arraykey(new stdClass()); // nope: object
}
<<__EntryPoint>> function main_entry(): void {
set_error_handler(my_handler<>);
main();
}
