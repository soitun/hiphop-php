<?hh
// Repeating the inout modifier is invalid.

interface I1 {
  public function ioio(inout inout int $x): void;
}
