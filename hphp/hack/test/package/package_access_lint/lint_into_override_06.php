//// warning_02.php
<?hh

<<file: __PackageOverride('pkg1')>>

// package pkg2 but package pkg1 via packageoverride
class MyInternType {}

class Extra1 {} // this extra class silences the filename linter

//// bar.php
<?hh

// package pkg1
class StaticHelper {
  public static function call<T>(T $_): void {}
}

class InstanceHelper {
  public function call<T>(T $_): void {}
}

class TestClass {
  public function bar(): void {
    // Static method call with explicit generic arg
    StaticHelper::call<MyInternType>(new MyInternType());
    // Instance method call with explicit generic arg
    $obj = new InstanceHelper();
    $obj->call<MyInternType>(new MyInternType());
    // Nested generic in method call
    $obj->call<vec<MyInternType>>(vec[new MyInternType()]);
    // Deeply nested generic in method call
    $obj->call<dict<string, vec<MyInternType>>>(dict[]);
  }
}

class Extra2 {} // this extra class silences the filename linter
