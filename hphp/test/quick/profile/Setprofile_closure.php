<?hh

function globalFunc(mixed $a) :void{
  $l1 = ($_) ==> { error_log('In lambda1'); };
  $l2 = ($_) ==> { error_log('In lambda2'); };
  error_log('In globalFunc');
  $l1($a);
  $l2($a);
}

class SomeClass {
  public static function someFunc(mixed $a) :void{
    $l1 = ($_) ==> { error_log('In lambda1'); };
    $l2 = ($_) ==> { error_log('In lambda2'); };
    error_log('In someFunc');
    $l1($a);
    $l2($a);
  }
}

function proffunc(string $event, string $name, mixed $info) :void{
  if ($name === "error_log" || $name === "fb_setprofile") {
    return;
  }
  echo "** $event $name\n";
}

function main(mixed $a) :void{
  fb_setprofile(
    proffunc<>,
    SETPROFILE_FLAGS_RESUME_AWARE | SETPROFILE_FLAGS_DEFAULT,
  );
  globalFunc($a);
  SomeClass::someFunc($a);
  fb_setprofile(null);
}

<<__EntryPoint>> function main_entry(): void {
  main(42);
}
