<?hh

class aa {
  protected function blah() :void{
    echo "protected aa::blah\n";
    var_dump($this);
  }

  protected function func(mixed $o) :void{
    echo "protected aa::blah\n";
    var_dump($o === $this);
  }
}

class a extends aa {
  protected function blah() :void{
    echo "private a::blah\n";
  }

  public static function stat() :void{
    echo "public static a::stat\n";
  }

  public function nons() :void{
    $str = 'blah';
    HH\dynamic_meth_caller(self::class, $str)($this);
    self::stat();
    parent::blah();

    parent::func(null);
    parent::func($this);
    parent::func(null);
    parent::func($this);
  }
}

<<__EntryPoint>> function main(): void {
  $a = new a();
  $a->nons();
}
