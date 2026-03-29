//// ptr7a.php
<?hh

// package pkg2 (no override) - this is the target
class TargetClass7 {}

class N1 {} // this extra class silences the filename linter

//// ptr7b.php
<?hh

<<file: __PackageOverride('pkg1')>>

// package pkg2 via PACKAGES.toml, but overridden to pkg1
// Referencing TargetClass7::class should fire lint 5656 because
// pkg1 does not include pkg2
class SourceClass7 {
  public function test(): void {
    $c = TargetClass7::class; // should raise lint 5656
  }
}

class N2 {} // this extra class silences the filename linter
