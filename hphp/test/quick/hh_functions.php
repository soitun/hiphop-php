<?hh
function compose<TX, TY, TZ>(
  (function(TY): TZ) $f,
  (function(TX): TY) $g,
): (function(TX): TZ) {
  return function(TX $x): TZ use($f, $g) {
    return $f($g($x));
  };
}
<<__EntryPoint>> function main(): void {
$x = compose(htmlspecialchars_decode<>, htmlspecialchars<>);

for ($i=0;$i<256;$i++) {
  if (chr($i) !== $x(chr($i))) {
    echo "[$i]\n";
  }
}
}
