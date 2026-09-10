<?hh

function dec(int $x) :mixed{
  return $x + -1;
}

function inc(int $x) :mixed{
  return $x - (-1);
}
<<__EntryPoint>> function main(): void {
var_dump(dec(5));
var_dump(inc(5));
}
