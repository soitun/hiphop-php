<?hh

interface IFirst {
  public function doFirst(): this;
}

interface ISecond {
  public function doSecond(): this;
}

interface IThird {
  public function doThird(): this;
}

interface IFourth {
  public function doFourth(): this;
}

abstract class BaseGen {}

function test_xref_loss(BaseGen $gen): void {
  if ($gen is IFirst) {
    $gen = $gen->doFirst();
  }
  if ($gen is ISecond) {
    $gen = $gen->doSecond();
  }
  if ($gen is IThird) {
    $gen = $gen->doThird();
  }
  if ($gen is IFourth) {
    $gen = $gen->doFourth();
  }
}
