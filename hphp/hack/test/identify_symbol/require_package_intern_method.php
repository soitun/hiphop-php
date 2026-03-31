<?hh

class Baz {}

class Foo {
  <<__RequirePackage("intern")>>
  public function pkg_method(Baz $b): void {}
}
