<?hh

function expects_int(int $_): void {}

function foo(): void {
  expects_int(HH\FIXME\UNSAFE_CAST<string, bool>("hello"));
}
