<?hh


function go(mixed $c, int $k) :void{
  $c = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $c,
    'The test intentionally performs possibly invalid collection operations',
  );
  try {
    $unused = $c[$k];
  } catch (Exception $e) {
    var_dump($e->getMessage());
  }

  try {
    $c[$k] = 0;
  } catch (Exception $e) {
    var_dump($e->getMessage());
  }

  var_dump($c);
}
<<__EntryPoint>> function main(): void {
go(Vector {'zero', 'one'}, 2);
go(Vector {'zero', 'one'}, -2);
go(Pair {'zero', 'one'}, 2);
}
