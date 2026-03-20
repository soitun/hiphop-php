<?hh

class C {
  public int $myprop = 1;
}

trait T {
  require extends C;
}

<<__EnableMethodTraitDiamond>>
class MyClass extends C {
  use T;
}
