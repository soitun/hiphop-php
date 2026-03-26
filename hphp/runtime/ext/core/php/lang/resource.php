<?hh

// Used to represent resources
class __resource {
  public function __toString(): string {
    return hphp_to_string($this);
  }
}
