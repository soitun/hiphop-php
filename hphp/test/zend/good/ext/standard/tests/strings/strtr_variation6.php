<?hh
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'from'
 *  and expected type for 'str' & 'to' arguments
*/

//defining a class
class sample  {
  public function __toString() :mixed{
    return "sample object";
  }
}
<<__EntryPoint>> function main(): void {
echo "*** Testing strtr() function: with unexpected inputs for 'from' ***\n";


//getting the resource
$file_handle = fopen(__FILE__, "r");

//defining 'str' argument
$str = "012atm";

// array of values for 'from'
$from_arr =  vec[

          // integer values
/*1*/      0,
          1,
          -2,

          // float values
/*4*/      10.5,
          -20.5,
          10.1234567e10,

          // array values
/*7*/     'Array',
          'Array',
          'Array',

          // boolean values
/*10*/      true,
          false,
          TRUE,
          FALSE,

          // null vlaues
/*14*/      NULL,
          null,

          // objects
/*16*/      new sample(),

          // resource
/*17*/      $file_handle,


];

//defining 'to' argument
$to = "atm012";

// loop through with each element of the $from array to test strtr() function
$count = 1;
for($index = 0; $index < count($from_arr); $index++) {
  echo "-- Iteration $count --\n";
  $from = $from_arr[$index];
  var_dump( strtr($str, $from, $to) );
  $count ++;
}

fclose($file_handle);  //closing the file handle
echo "===DONE===\n";
}
