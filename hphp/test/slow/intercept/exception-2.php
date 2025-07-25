<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

<<__DynamicallyCallable>> function foo($a, $b) :mixed{
  try {
    throw new Exception('wtf');
  } catch (Exception $e) {
    throw new Exception('nope');
  }
}

<<__DynamicallyCallable>> async function bar($a, $b) :Awaitable<mixed>{
  try {
    throw new Exception('wtf');
  } catch (Exception $e) {
    throw new Exception('nope');
  }
}

<<__DynamicallyCallable>> function handler($name, $obj_or_cls, inout $args) :mixed{
  throw new Exception('yep');
}

<<__EntryPoint>>
function main(): void {
  $funcs = vec['foo', 'bar'];
  foreach ($funcs as $func) {
    fb_intercept2($func, HH\dynamic_fun('handler'));
    try {
      HH\dynamic_fun($func)(1, 2);
      echo "lol\n";
    } catch (Exception $e) {
      echo $e->getMessage() . "\n";
    }
  }
}
