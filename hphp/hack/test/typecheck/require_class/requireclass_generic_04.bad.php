<?hh


interface I {}

trait T1<T> {
  require class T;
}

trait T2<T> {
  require extends T;
}
