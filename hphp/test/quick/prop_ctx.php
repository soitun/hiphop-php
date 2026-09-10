<?hh

trait t {
  public static function f(mixed $o) :void{
    var_dump((HH\FIXME\UNSAFE_CAST<mixed, dynamic>($o, 'Runtime property'))->prop);
  }
  public static function set(mixed $o, string $v) :void{
    (HH\FIXME\UNSAFE_CAST<mixed, dynamic>($o, 'Runtime property'))->prop = $v;
    var_dump($o);
  }
}

class a {
  use t;
  private string $prop = 'I am private in a';
}

class b extends a {
  public string $prop = 'I am public in b';
}

<<__EntryPoint>> function main(): void {
  $b = new b();
  $b::f($b);
  t::f($b);

  $b::set($b, 'new value');
  t::set($b, 'newer value');

  $a = new a();
  $a::f($a);
  t::f($a);
}
