<?hh


trait T {
  private function bar() :void{}
  public function foo() :void{}
}

class A {
  use T;
}
<<__EntryPoint>> function main(): void {
print_r(get_class_methods('A'));
}
