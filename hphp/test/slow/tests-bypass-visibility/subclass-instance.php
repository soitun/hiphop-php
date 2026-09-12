<?hh

class WWWTest {}

class Base {
  <<__TestsBypassVisibility>>
  private string $privProp = "Base::privProp";

  <<__TestsBypassVisibility>>
  private function priv(): mixed { return "Base::priv"; }

  <<__TestsBypassVisibility>>
  protected function prot(): mixed { return "Base::prot"; }

  <<__TestsBypassVisibility>>
  private static function spriv(): mixed { return "Base::spriv"; }
}

class Child extends Base {}
class GrandChild extends Child {}

class SubclassTest extends WWWTest {
  public function test(): void {
    $base = new Base();
    echo $base->priv() . "\n";
    echo $base->prot() . "\n";
    echo $base->privProp . "\n";

    $child = new Child();
    echo $child->priv() . "\n";
    echo $child->prot() . "\n";
    echo $child->privProp . "\n";
    $child->privProp = "Child::privProp";
    echo $child->privProp . "\n";

    $grandchild = new GrandChild();
    echo $grandchild->priv() . "\n";
    echo $grandchild->prot() . "\n";
    echo $grandchild->privProp . "\n";

    echo Base::spriv() . "\n";
  }
}

<<__EntryPoint>>
function main(): void {
  (new SubclassTest())->test();
}
