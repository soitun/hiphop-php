<?hh

// Warning: line numbers are sensitive, do not change

class :fb:my:thing {

  public static function doIt(): void {
    echo "doing my thing\n";
  }
}

<<__EntryPoint>>
function main(): void {
  error_log('break4.php loaded');
}
