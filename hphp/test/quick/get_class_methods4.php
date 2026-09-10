<?hh

abstract class B {
  private function priv() :void{ echo "B::priv\n"; }
  public function func():void{
    $this->priv();
    var_dump(get_class_methods($this));
  }
}

class C extends B {
  private function priv() :void{ echo "C::priv\n"; }
}
<<__EntryPoint>> function main(): void {
$obj = new C();
$obj->func();
}
