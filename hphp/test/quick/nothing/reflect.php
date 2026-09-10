<?hh

class Foo {
  const type T = nothing;
  public function bar(nothing $a, int $b, Foo::T $c): nothing {
    return $a;
  }
}

function foobar(vec<nothing> $x): nothing {
  return $x[0];
}

function dump(ReflectionFunctionAbstract $x) :void{
  $return_type = HH\FIXME\UNSAFE_CAST<?ReflectionType, ReflectionType>(
    $x->getReturnType(),
    'The reflected declarations have return types',
  );
  var_dump($return_type->__toString());
  foreach ($x->getParameters() as $param) {
    $type = HH\FIXME\UNSAFE_CAST<?ReflectionType, ReflectionType>(
      $param->getType(),
      'The reflected declarations have parameter types',
    );
    var_dump($type->__toString());
  }
}

<<__EntryPoint>> function main(): void {
  echo "\nReflectionMethod:\n";
  dump(new ReflectionMethod('Foo::bar'));
  echo "\nReflectionFunction:\n";
  dump(new ReflectionFunction('foobar'));
}
