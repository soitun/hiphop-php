<?hh

function foo () :void{}
async function async_foo() :Awaitable<void>{}

class Bar {
  public function foo () :void{}
  public async function asyncFoo() :Awaitable<void>{}
}
<<__EntryPoint>> function main(): void {
var_dump((new ReflectionFunction('foo'))->isAsync());
var_dump((new ReflectionFunction('async_foo'))->isAsync());
var_dump((new ReflectionMethod('Bar::foo'))->isAsync());
var_dump((new ReflectionMethod('Bar::asyncFoo'))->isAsync());
var_dump((new ReflectionFunction(async function () {}))->isAsync());
}
