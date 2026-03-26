<?hh

function takes_representable_as_int(\HH\Runtime\RepresentableAs<int> $x): void {
  var_dump($x);
}

function takes_representable_as_string(\HH\Runtime\RepresentableAs<string> $x): void {
  var_dump($x);
}

function takes_representable_as_vec(\HH\Runtime\RepresentableAs<vec<int>> $x): void {
  var_dump($x);
}

<<__EntryPoint>>
function main(): void {
  // T <: RepresentableAs<T> — values pass through at runtime
  takes_representable_as_int(42);
  takes_representable_as_string("hello");
  takes_representable_as_vec(vec[1, 2, 3]);

  // Tuple upcast: (int, int) <: RepresentableAs<vec<int>>
  takes_representable_as_vec(tuple(10, 20));

  // reveal: RepresentableAs<T> -> T (identity at runtime)
  $x = 42;
  $revealed = \HH\Runtime\reveal($x);
  var_dump($revealed);

  $s = "world";
  var_dump(\HH\Runtime\reveal($s));

  echo "done\n";
}
