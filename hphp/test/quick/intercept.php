<?hh

function foo() :mixed{ var_dump(__METHOD__); }
<<__DynamicallyCallable>> function bar($_1, $_2, inout $_3) :mixed{
  var_dump(__METHOD__);
  throw new Exception;
}

<<__EntryPoint>> function boo(): void {
  fb_intercept2('foo', HH\dynamic_fun('bar'));
  try {
    foo();
  } catch (Exception $e) {
    var_dump("caught:" . $e->getMessage());
  }
}
