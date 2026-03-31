<?hh

class Bar {}

<<__RequirePackage("intern")>>
function require_pkg_intern_fun(Bar $b): void {}
