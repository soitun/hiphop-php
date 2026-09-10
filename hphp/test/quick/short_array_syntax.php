<?hh
function f(vec<int> $x = vec[1]) :mixed{
  return $x;
}
class C {
  public static vec<int> $y = vec[2, 3];
}
<<__EntryPoint>> function main(): void {
var_dump(vec[]);
var_dump(f());
var_dump(C::$y);
}
