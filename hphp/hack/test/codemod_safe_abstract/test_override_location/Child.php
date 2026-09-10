<?hh

class Child extends Base {
  <<__NeedsConcrete>>
  public static function make(): this {
    return new static();
  }
}
