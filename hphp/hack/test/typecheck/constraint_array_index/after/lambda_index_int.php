<?hh
function f(): void {
  $v = vec[0];
  $f = ($i ==> $v[$i]);
  $f(0);
}
