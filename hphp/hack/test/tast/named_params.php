<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

function foo(named int $x): void {}

function main(): void {
  foo(x=3);
}
