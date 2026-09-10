<?hh

function exn_throw(Throwable $exn) :mixed{
  throw new Exception('throwing second');
}
<<__EntryPoint>>
function main() :mixed{
  set_exception_handler(exn_throw<>);
  throw new Exception('throwing first');
}
