<?hh
<<__EntryPoint>>
function main(): void {
  $root = getenv('HPHP_TEST_SBCC_RUN_ROOT');
  require_once($root.'/flib/Hit.php');
  require_once($root.'/flib/Stale.php');
  require_once($root.'/flib/Miss.php');
  $stats = HH\SBCC\get_stats();
  echo "hit:".SbccHit::val()."\n";
  echo "stale:".SbccStale::val()."\n";
  echo "miss:".SbccMiss::val()."\n";
  echo "hits:".$stats['hits']."\n";
  echo "misses:".$stats['misses']."\n";
  echo "corrupt:".$stats['corrupt']."\n";
}
