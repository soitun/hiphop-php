<?hh

function test_break() {
  error_log("test_break selina");
  $x = 'test_break() in test1.php';
  foo($x);
  foo($x);
  $obj = new cls();
  $obj->pubObj($x);
  cls::pubCls($x);
}

<<__EntryPoint>> function mainn() { include 'break1.php'; test_break();
echo "request done\n"; }
