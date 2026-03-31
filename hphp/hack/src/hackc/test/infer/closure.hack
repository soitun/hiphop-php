// RUN: %hackc compile-infer --fail-fast %s | FileCheck %s

// TEST-CHECK-BAL: define $root.closure1
// CHECK: define $root.closure1($this: *void, $x: .notnull *HackString) : *HackMixed {
// CHECK: local $y: *void
// CHECK: #b0:
// CHECK: // .column 8
// CHECK:   n0: *HackMixed = load &$x
// CHECK: // .column 8
// CHECK:   n1 = __sil_allocate(<Closure$closure1>)
// CHECK:   n2 = Closure$closure1.__construct(n1, null, n0)
// CHECK: // .column 3
// CHECK:   store &$y <- n1: *HackMixed
// CHECK: // .column 10
// CHECK:   n3: *HackMixed = load &$y
// CHECK: // .column 3
// CHECK:   ret n3
// CHECK: }

// TEST-CHECK-BAL: define Closure$closure1.__construct
// CHECK: define Closure$closure1.__construct($this: .notnull *Closure$closure1, this: *HackMixed, x: *HackMixed) : *HackMixed {
// CHECK: #b0:
// CHECK:   n0: *HackMixed = load &this
// CHECK:   n1: *HackMixed = load &$this
// CHECK:   store n1.?.this <- n0: *HackMixed
// CHECK:   n2: *HackMixed = load &x
// CHECK:   n3: *HackMixed = load &$this
// CHECK:   store n3.?.x <- n2: *HackMixed
// CHECK:   ret null
// CHECK: }

// TEST-CHECK-BAL: define Closure$closure1.__invoke
// CHECK: define Closure$closure1.__invoke($this: .notnull *Closure$closure1) : *HackMixed {
// CHECK: local $x: *void
// CHECK: #b0:
// CHECK:   n0: *HackMixed = load &$this
// CHECK:   n1: *HackMixed = load n0.?.x
// CHECK:   store &$x <- n1: *HackMixed
// CHECK:   n2: *HackMixed = load &$this
// CHECK:   n3: *HackMixed = load n2.?.this
// CHECK:   store &$this <- n3: *HackMixed
// CHECK: // .column 21
// CHECK:   n4: *HackMixed = load &$x
// CHECK: // .column 15
// CHECK:   n5 = $builtins.hhbc_print(n4)
// CHECK: // .column 15
// CHECK:   ret n5
// CHECK: }

function closure1(string $x): mixed {
  $y = () ==> print($x);
  return $y;
}

class C {

  public static function main(int $x): void {
    $f = (int $y) ==> self::closure($x, $y);
  }

  public static function closure(int $i, int $j): int {
    return $i + $j;
  }
}
// TEST-CHECK-BAL: define Closure$C::main.__invoke
// CHECK: define Closure$C::main.__invoke($this: .notnull *Closure$C::main, $y: .notnull *HackInt) : *HackMixed {
// CHECK: local $x: *void
// CHECK: #b0:
// CHECK:   n0: *HackMixed = load &$this
// CHECK:   n1: *HackMixed = load n0.?.x
// CHECK:   store &$x <- n1: *HackMixed
// CHECK:   n2: *HackMixed = load &$this
// CHECK:   n3: *HackMixed = load n2.?.this
// CHECK:   store &$this <- n3: *HackMixed
// CHECK: // .column 37
// CHECK:   n4: *HackMixed = load &$x
// CHECK: // .column 41
// CHECK:   n5: *HackMixed = load &$y
// CHECK: // .column 23
// CHECK:   n6: *Closure$C::main = load &$this
// CHECK:   n7 = Closure$C::main.closure(n6, n4, n5)
// CHECK: // .column 23
// CHECK:   ret n7
// CHECK: }

// // Closure$C::main.closure ==> this is wrong! It should be C$static::closure

// Closures that capture $_ should have the property name mangled
// because standalone '_' is not a valid Textual identifier.
// Here $_ from the foreach has function scope and gets captured by the lambda.

// TEST-CHECK-BAL: type Closure$closure_underscore
// CHECK: type Closure$closure_underscore extends Closure = .kind="class" {
// CHECK:   this: .public *HackMixed;
// CHECK:   params: .private *HackMixed;
// CHECK:   param_map: .private *HackMixed;
// CHECK:   mangled:::_: .private *HackMixed
// CHECK: }

// TEST-CHECK-BAL: define Closure$closure_underscore.__construct
// CHECK: define Closure$closure_underscore.__construct($this: .notnull *Closure$closure_underscore, this: *HackMixed, params: *HackMixed, param_map: *HackMixed, mangled:::_: *HackMixed) : *HackMixed {
// CHECK: #b0:
// CHECK:   n0: *HackMixed = load &this
// CHECK:   n1: *HackMixed = load &$this
// CHECK:   store n1.?.this <- n0: *HackMixed
// CHECK:   n2: *HackMixed = load &params
// CHECK:   n3: *HackMixed = load &$this
// CHECK:   store n3.?.params <- n2: *HackMixed
// CHECK:   n4: *HackMixed = load &param_map
// CHECK:   n5: *HackMixed = load &$this
// CHECK:   store n5.?.param_map <- n4: *HackMixed
// CHECK:   n6: *HackMixed = load &mangled:::_
// CHECK:   n7: *HackMixed = load &$this
// CHECK:   store n7.?.mangled:::_ <- n6: *HackMixed
// CHECK:   ret null
// CHECK: }

// TEST-CHECK-BAL: define Closure$closure_underscore.__invoke
// CHECK: define Closure$closure_underscore.__invoke($this: .notnull *Closure$closure_underscore, $param_name: *HackMixed) : *HackMixed {
// CHECK: local $_: *void, $param_map: *void, $params: *void, iter0: *void, $0: *void
// CHECK: #b0:
// CHECK:   n0: *HackMixed = load &$this
// CHECK:   n1: *HackMixed = load n0.?.mangled:::_
// CHECK:   store &$_ <- n1: *HackMixed
// CHECK:   n2: *HackMixed = load &$this
// CHECK:   n3: *HackMixed = load n2.?.param_map
// CHECK:   store &$param_map <- n3: *HackMixed
// CHECK:   n4: *HackMixed = load &$this
// CHECK:   n5: *HackMixed = load n4.?.params
// CHECK:   store &$params <- n5: *HackMixed
// CHECK:   n6: *HackMixed = load &$this
// CHECK:   n7: *HackMixed = load n6.?.this
// CHECK:   store &$this <- n7: *HackMixed
// CHECK: // .column 14
// CHECK:   n8: *HackMixed = load &$params
// CHECK: // .column 14
// CHECK:   n9 = $builtins.hhbc_iter_base(n8)
// CHECK: // .column 14
// CHECK:   store &$0 <- n9: *HackMixed
// CHECK: // .column 14
// CHECK:   jmp b1
// CHECK: #b1:
// CHECK: // .column 14
// CHECK:   n10: *HackMixed = load &$0
// CHECK: // .column 14
// CHECK:   n11 = $builtins.hhbc_iter_init(&iter0, n10)
// CHECK: // .column 14
// CHECK:   jmp b2, b7
// CHECK:   .handlers b9
// CHECK: #b2:
// CHECK: // .column 14
// CHECK:   prune $builtins.hack_is_true(n11)
// CHECK: // .column 14
// CHECK:   jmp b3
// CHECK: #b3:
// CHECK: // .column 14
// CHECK:   n12: *HackMixed = load &iter0
// CHECK: // .column 14
// CHECK:   n13: *HackMixed = load &$0
// CHECK: // .column 14
// CHECK:   n14 = $builtins.hhbc_iter_get_value(n12, n13)
// CHECK: // .column 14
// CHECK:   store &$param_map <- n14: *HackMixed
// CHECK: // .column 14
// CHECK:   n15: *HackMixed = load &iter0
// CHECK: // .column 14
// CHECK:   n16: *HackMixed = load &$0
// CHECK: // .column 14
// CHECK:   n17 = $builtins.hhbc_iter_get_key(n15, n16)
// CHECK: // .column 14
// CHECK:   store &$_ <- n17: *HackMixed
// CHECK: // .column 12
// CHECK:   n18: *HackMixed = load &$param_map
// CHECK: // .column 7
// CHECK:   n19 = $builtins.hhbc_print(n18)
// CHECK: // .column 5
// CHECK:   n20: *HackMixed = load &$0
// CHECK: // .column 5
// CHECK:   n21: *HackMixed = load &iter0
// CHECK: // .column 5
// CHECK:   n22 = $builtins.hhbc_iter_next(n21, n20)
// CHECK: // .column 5
// CHECK:   jmp b5, b6
// CHECK:   .handlers b4
// CHECK: #b4(n23: *HackMixed):
// CHECK: // .column 5
// CHECK:   n24: *HackMixed = load &iter0
// CHECK: // .column 5
// CHECK:   n25 = $builtins.hhbc_iter_free(n24)
// CHECK: // .column 5
// CHECK:   throw n23
// CHECK:   .handlers b9
// CHECK: #b5:
// CHECK: // .column 5
// CHECK:   prune $builtins.hack_is_true(n22)
// CHECK: // .column 5
// CHECK:   jmp b8
// CHECK: #b6:
// CHECK: // .column 5
// CHECK:   prune ! $builtins.hack_is_true(n22)
// CHECK: // .column 5
// CHECK:   jmp b3
// CHECK: #b7:
// CHECK: // .column 14
// CHECK:   prune ! $builtins.hack_is_true(n11)
// CHECK: // .column 14
// CHECK:   jmp b8
// CHECK: #b8:
// CHECK: // .column 5
// CHECK:   jmp b10
// CHECK:   .handlers b9
// CHECK: #b9(n26: *HackMixed):
// CHECK: // .column 5
// CHECK:   store &$0 <- null: *HackMixed
// CHECK: // .column 5
// CHECK:   throw n26
// CHECK: #b10:
// CHECK: // .column 5
// CHECK:   store &$0 <- null: *HackMixed
// CHECK: // .column 5
// CHECK:   store &$0 <- null: *HackMixed
// CHECK: // .column 12
// CHECK:   n27: *HackMixed = load &$param_name
// CHECK: // .column 5
// CHECK:   ret n27
// CHECK: }

function closure_underscore(
  vec<string> $params,
  keyset<string> $names,
): void {
  $fn = ($param_name) ==> {
    foreach ($params as $_ => $param_map) {
      echo $param_map;
    }
    return $param_name;
  };

  foreach ($params as $_ => $param_map) {
    foreach ($names as $_) {
      echo $param_map;
    }
  }

  $fn('test');
}
