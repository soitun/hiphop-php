<?hh

class c{

}
function main(mixed $o) :void{
  if (false) {}
  (HH\FIXME\UNSAFE_CAST<mixed, dynamic>($o, 'Intentional missing method'))->foo();
}
<<__EntryPoint>> function main_entry(): void {
main(new c());
}
