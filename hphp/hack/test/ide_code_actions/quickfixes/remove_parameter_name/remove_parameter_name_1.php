<?hh
<<file: __EnableUnstableFeatures('named_parameters')>>

function bad((function(int $positional_param_with_name): void) $f): void {}
                            // ^ at-caret
