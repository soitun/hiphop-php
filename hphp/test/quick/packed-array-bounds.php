<?hh


function main(vec<int> $a, int $i) :void{
  var_dump(isset($a[1 << 32]));
  var_dump(isset($a[$i]));
}
<<__EntryPoint>> function main_entry(): void {
main(vec[1, 2, 3, 4], 1 << 32 + 1);
}
