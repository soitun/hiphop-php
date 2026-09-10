<?hh
// Test the effectiveness of pre-coloring
// Shouldn't have any shuffling for concats
function foo(int $t0, int $t1, int $t2, int $t3, int $t4, int $t5, int $t6) :void{
  $sum = 0;
  $sum = $sum + $t0;
  $sum = $sum + $t1;
  $sum = $sum + $t2;
  $sum = $sum + $t3;
  $sum = $sum + $t4;
  $sum = $sum + $t5;
  $sum = $sum + $t6;
  $concat = $t3 . $t6;
  if ($sum > 0) {
    echo "sum = " . $sum . "\n";
    echo "concat = " . $concat . "\n";
  }
}
<<__EntryPoint>> function main(): void {
foo(1, 2, 3, 4, 5, 6, 7);
}
