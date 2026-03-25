<?hh
<<__EntryPoint>>
function main(): void {
  $root = getenv('HPHP_TEST_SBCC_RUN_ROOT');
  require_once($root.'/flib/Greeter.php');
  $stats = HH\SBCC\get_stats();
  echo "value:".SbccHitGreeter::hello()."\n";
  echo "hits:".$stats['hits']."\n";
  echo "misses:".$stats['misses']."\n";
  echo "corrupt:".$stats['corrupt']."\n";
}
