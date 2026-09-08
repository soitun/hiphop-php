<?hh
// Repeating the readonly modifier is invalid.

interface I1 {
  public function roro(readonly readonly int $x): void;
}
