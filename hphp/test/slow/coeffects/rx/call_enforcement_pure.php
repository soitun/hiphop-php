<?hh

<<__DynamicallyCallable>> function non_rx($fn) :mixed{
  if ($fn) HH\dynamic_fun($fn)(null);
}

<<__DynamicallyCallable>> function rx_local($fn)[rx_local] :mixed{
  if ($fn) HH\dynamic_fun($fn)(null);
}

<<__DynamicallyCallable>> function rx_shallow($fn)[rx_shallow] :mixed{
  if ($fn) HH\dynamic_fun($fn)(null);
}

<<__DynamicallyCallable>> function rx($fn)[rx] :mixed{
  if ($fn) HH\dynamic_fun($fn)(null);
}

<<__DynamicallyCallable>> function pure($fn)[] :mixed{
  if ($fn) HH\dynamic_fun($fn)(null);
}

<<__EntryPoint>>
function main() :mixed{
  $functions = vec['non_rx', 'rx_local', 'rx_shallow', 'rx', 'pure'];
  foreach ($functions as $caller) {
    foreach ($functions as $callee) {
      try {
        HH\dynamic_fun($caller)($callee);
        echo "$caller -> $callee: ok\n";
      } catch (Exception $e) {
        echo "$caller -> $callee: ".$e->getMessage()."\n";
      }
    }
  }
}
