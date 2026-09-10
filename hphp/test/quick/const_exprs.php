<?hh

const int FOO = -(10+10);
const bool BAR = (10+10) < (10*10);
const num BAZ = 2 ** 10;
<<__EntryPoint>> function main(): void {
var_dump(FOO);
var_dump(BAR);
var_dump(BAZ);
}
