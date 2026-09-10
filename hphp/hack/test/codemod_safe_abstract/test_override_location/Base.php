<?hh

class Base {
  public static function make(): this {
    return new static();
  }
}
