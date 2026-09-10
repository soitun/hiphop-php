<?hh

function test1() : void {
  var_dump(__METHOD__);
}

<<__EntryPoint>>
function main() : void {
fb_rename_function('test1', 'fiz');
}
