<?hh

abstract class Base {
  public static function configPrivacy(): void {}
}

trait TraitA {
  require extends Base;

  <<__Override>>
  public static function configPrivacy(): void {
    echo "TraitA\n";
  }
}

trait TraitB {
  require extends Base;

  <<__Override>>
  public static function configPrivacy(): void {
    echo "TraitB\n";
  }
}

abstract class Middle extends Base {
  use TraitA;
}

// Hack correctly reports an error here
final class Child1 extends Base {
  use TraitA;
  use TraitB;
}

// Hack correctly reports an error here too
final class Child2 extends Middle {
  use TraitA;
  use TraitB;
}
