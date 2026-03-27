//// warning_02.php
<?hh

<<file: __PackageOverride('pkg1')>>

// package pkg2 but package pkg1 via packageoverride
class MyInternType {}

class Extra1 {} // this extra class silences the filename linter

//// bar.php
<?hh

// package pkg1
function my_function_call<T>(T $_): void {}
function my_nested_function_call<T>(vec<T> $_): void {}

class TestClass {
  public function bar(): void {
    my_function_call<MyInternType>(new MyInternType());  // should raise lint error
    my_nested_function_call<MyInternType>(vec[new MyInternType()]);  // should raise lint error on nested type
  }
}

class Extra2 {} // this extra class silences the filename linter
