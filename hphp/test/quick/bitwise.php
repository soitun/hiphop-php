<?hh

function f() :void{
  $x = 204; // 11001100 in binary
  $y = 170; // 10101010 in binary
  echo ($x ^ $y); // 01100110 in binary
  echo "\n";
  echo ($x & $y);
  echo "\n";
  echo ($x | $y);
  echo "\n";
  echo (~$x);
  echo "\n";
}

// Pairwise probe.
function probe(arraykey $l, arraykey $r) :void{
  echo "-------\n";
  echo "left: ";  var_dump($l);
  echo "right: "; var_dump($r);
  $orig_l = $l;
  if(!($l is string && $r is string)) {
    $l = (int)$l;
    $r = (int)$r;
  }
  $dynamic_l = HH\FIXME\UNSAFE_CAST<arraykey, dynamic>(
    $l,
    'String bitwise operations are intentional in this test',
  );
  $dynamic_r = HH\FIXME\UNSAFE_CAST<arraykey, dynamic>(
    $r,
    'String bitwise operations are intentional in this test',
  );
  $v = ($dynamic_l & $dynamic_r); var_dump($v);
  $v = ($dynamic_l | $dynamic_r); var_dump($v);
  $v = ($dynamic_l ^ $dynamic_r); var_dump($v);
  $dynamic_orig_l = HH\FIXME\UNSAFE_CAST<arraykey, dynamic>(
    $orig_l,
    'String bitwise operations are intentional in this test',
  );
  $v = ~$dynamic_orig_l; var_dump($v);
}

<<__EntryPoint>>
function main() :void{
  f();
  $i = 0x3;
  $data = vec[15, "7", "not an int. at all."];
  foreach ($data as $left) {
    foreach ($data as $right) {
      probe($left, $right);
    }
  }
}
