<?hh

function main(int $x, int $y) :void{
  while ($x < $y) {
    echo $x . "\n";
    $x++;
  }
}
<<__EntryPoint>> function main_entry(): void {
main(1, 4);

echo "Done\n";
}
