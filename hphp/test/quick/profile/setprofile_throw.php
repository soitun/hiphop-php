<?hh


function throwing_profiler(string $case, string $func) :void{
  if ($case == 'enter' && ($func == 'bar' || $func == 'baz')) {
    throw new Exception("yeah");
  }
}

function bar() :void{ echo "bar()\n"; }
function baz() :void{ echo "baz()\n"; }

function foo(string $f) :void{
  HH\FIXME\UNSAFE_CAST<string, (function(): void)>($f, 'String function name')();
}

<<__EntryPoint>>
function setprofile_throw(): void {
// Test throwing on function entry
  fb_setprofile('throwing_profiler');
  try { foo('bar'); } catch (Exception $x) { echo "Caught\n"; }
  try { foo('baz'); } catch (Exception $x) { echo "Caught\n"; }
  fb_setprofile(null);
}
