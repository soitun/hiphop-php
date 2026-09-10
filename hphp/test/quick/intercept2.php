<?hh

/*
 * Test intercepts where callsites have already been bound to the
 * pre-intercept function.
 */

function foo(mixed $i = 10) :void{
  var_dump(__METHOD__);
}

<<__DynamicallyCallable>> function bar(mixed $_1, mixed $_2, inout mixed $_3) :mixed{
  var_dump(__METHOD__);
  return shape('value' => null);
}

function test() :void{
  foo();
}

class C {
  public function snoot() :void{
    var_dump(__METHOD__);
  }
}


<<__DynamicallyCallable>> function swizzle(mixed $name, mixed $obj, inout mixed $args) :mixed{
  var_dump($name, $obj, $args);
  return shape();
}

<<__EntryPoint>> function main(): void {
  $c = new C();
  for ($i = 0; $i < 3; $i++) {
    test();
    foo();
    $c->snoot();
    if ($i == 1) {
      fb_intercept2('foo', HH\dynamic_fun('bar'));
      fb_intercept2('C::snoot', HH\dynamic_fun('swizzle'));
    }
  }
}
