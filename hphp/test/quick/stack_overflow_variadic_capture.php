<?hh

function asd(int $x, int $y, arraykey ...$z) :void{
  asd($x, $y, $x + $y, $y + $x, "asd");
}
<<__EntryPoint>> function main(): void {
asd(1, 2);
}
