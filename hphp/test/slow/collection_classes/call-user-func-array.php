<?hh
class C {
  <<__DynamicallyCallable>> public static function foo(...$args) :mixed{
    var_dump(__FUNCTION__, $args);
  }
}
function main() :mixed{
  $cufa = 'call_user_func_array';
  if (\HH\global_get('argc') > 1000000000) {
    $cufa = 'call_user_func';
  }
  call_user_func_array(HH\dynamic_fun('var_dump'), dict[4 => 123, 6 => 456]);
  echo "\n";
  call_user_func_array('C::foo', dict[4 => 123, 6 => 456]);
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), dict[4 => 123, 6 => 456]);
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', dict[4 => 123, 6 => 456]);
  echo "\n";
  call_user_func_array(HH\dynamic_fun('var_dump'), Vector {123, 456});
  echo "\n";
  call_user_func_array('C::foo', Vector {123, 456});
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), Vector {123, 456});
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', Vector {123, 456});
  echo "\n";
  call_user_func_array(HH\dynamic_fun('var_dump'), Map {2 => 123});
  echo "\n";
  call_user_func_array('C::foo', Map {2 => 123});
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), Map {2 => 123});
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', Map {2 => 123});
  echo "\n";
  call_user_func_array(HH\dynamic_fun('var_dump'), Map {4 => 123, 6 => 456});
  echo "\n";
  call_user_func_array('C::foo', Map {4 => 123, 6 => 456});
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), Map {4 => 123, 6 => 456});
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', Map {4 => 123, 6 => 456});
  echo "\n";
  call_user_func_array(HH\dynamic_fun('var_dump'), Set {123});
  echo "\n";
  call_user_func_array('C::foo', Set {123});
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), Set {123});
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', Set {123});
  echo "\n";
  call_user_func_array(HH\dynamic_fun('var_dump'), Pair {11, 'a'});
  echo "\n";
  call_user_func_array('C::foo', Pair {11, 'a'});
  echo "\n";
  HH\dynamic_fun($cufa)(HH\dynamic_fun('var_dump'), Pair {11, 'a'});
  echo "\n";
  HH\dynamic_fun($cufa)('C::foo', Pair {11, 'a'});
  echo "\n";
}


<<__EntryPoint>>
function main_call_user_func_array() :mixed{
main();
}
