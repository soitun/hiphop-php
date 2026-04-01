<?hh

function expects_int(int $_): void {}

function foo(): void {
  $x = HH\FIXME\UNSAFE_CAST<string, bool>("hello");
  $y = $x;
  expects_int($y);
}
