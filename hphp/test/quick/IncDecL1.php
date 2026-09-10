<?hh

function error_boundary((function(): mixed) $fn) :mixed{
  try {
    return $fn();
  } catch (Exception $e) {
    print("Error: ".$e->getMessage()."\n");
    return null;
  }
}

function postInc(mixed $x) :mixed{
  $x = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $x,
    'The test intentionally applies increment or decrement to legacy values',
  );
  return error_boundary(() ==> { $r = $x; $x++; return $r; });
}

function preInc(mixed $x) :mixed{
  $x = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $x,
    'The test intentionally applies increment or decrement to legacy values',
  );
  return error_boundary(() ==> { ++$x; return $x; });
}

function postDec(mixed $x) :mixed{
  $x = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $x,
    'The test intentionally applies increment or decrement to legacy values',
  );
  return error_boundary(() ==> { $r = $x; $x--; return $r; });
}

function preDec(mixed $x) :mixed{
  $x = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $x,
    'The test intentionally applies increment or decrement to legacy values',
  );
  return error_boundary(() ==> { --$x; return $x; });
}
<<__EntryPoint>> function main(): void {
var_dump(postInc(2));
var_dump(preInc(2));
var_dump(postDec(2));
var_dump(preDec(2));

var_dump(postInc(2.5));
var_dump(preInc(2.5));
var_dump(postDec(2.5));
var_dump(preDec(2.5));

var_dump(postInc(false));
var_dump(preInc(false));
var_dump(postDec(false));
var_dump(preDec(false));

var_dump(postInc(true));
var_dump(preInc(true));
var_dump(postDec(true));
var_dump(preDec(true));

var_dump(postInc(null));
var_dump(preInc(null));
var_dump(postDec(null));
var_dump(preDec(null));
}
