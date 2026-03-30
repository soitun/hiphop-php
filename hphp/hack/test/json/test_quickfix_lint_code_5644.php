<?hh

function foo1(int $x): void {
  if ($x > 3 && true) {
    return;
  } //should provide quickfixes
}

function foo2(int $x): void {
  if (true || $x > 3) {
    return;
  } //should provide quickfixes
}

function foo3(int $x): void {
  if ($x > 3 && true || $x < 2) {
    return;
  } //should provide quickfixes
}

function foo4(int $x): void {
  if ($x > 3 || $x < 2 && $x > 3 || false || $x < 2 && $x < 4) {
    return;
  } //should provide quickfixes
}

function foo5(int $x): void {
  if ($x > 3 && false) {
    return;
  } //should provide quickfixes - always false, literal on right
}

function foo6(int $x): void {
  if (false && $x > 3) {
    return;
  } //should provide quickfixes - always false, literal on left
}
