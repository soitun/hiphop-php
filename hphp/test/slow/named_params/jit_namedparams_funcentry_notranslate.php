<?hh
<<file: __EnableUnstableFeatures('named_parameters')>>

// Regression test for D117998003: findOrTranslate() checked sk.funcEntry()
// instead of sk.anyFuncEntry() when reusing a translation, so a namedParams
// func entry resumed as trans() rather than transFuncEntry() and crashed in
// handleRetranslateAnyFuncEntry().  Load-bearing: array_map() needs >= 3
// arguments, JitGlobalTranslationLimit must run out part way through the
// victim list, and JitLiveStreakRatio=1 keeps repo mode from emitting the
// prologues up front.

function v0(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v1(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v2(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v3(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v4(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v5(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v6(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v7(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v8(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v9(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v10(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v11(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v12(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v13(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v14(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }
function v15(mixed $x, named int $a = 1, named int $b = 2): int { return (int)$x + $a + $b; }

<<__EntryPoint>>
function main(): void {
  $n = 0;
  foreach (vec[v0<>, v1<>, v2<>, v3<>, v4<>, v5<>, v6<>, v7<>, v8<>, v9<>, v10<>, v11<>, v12<>, v13<>, v14<>, v15<>] as $f) {
    $n += count(array_map($f, vec[1, 2, 3, 4]));
  }
  echo "ok n=$n\n";
}
