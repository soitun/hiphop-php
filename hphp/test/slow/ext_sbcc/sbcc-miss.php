<?hh
<<__EntryPoint>>
function main(): void {
  $root = getenv('HPHP_TEST_SBCC_RUN_ROOT');
  require_once($root.'/flib/NotInCache.php');
  require_once($root.'/flib/InCache.php');
  $stats = HH\SBCC\get_stats();
  echo "not_in_cache:".SbccNotInCache::val()."\n";
  echo "in_cache:".SbccInCache::val()."\n";
  echo "hits:".$stats['hits']."\n";
  echo "misses:".$stats['misses']."\n";
  echo "corrupt:".$stats['corrupt']."\n";
}
