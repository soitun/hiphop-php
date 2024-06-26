<?hh
/* Prototype  : string strval  ( mixed $var  )
 * Description: Get the string value of a variable.
 * Source code: ext/standard/string.c
 */
<<__EntryPoint>> function main(): void {
echo "*** Testing strval() : basic variations ***\n";

error_reporting(E_ALL ^ E_NOTICE);

$simple_heredoc =<<<EOT
Simple HEREDOC string
EOT;


//array of values to iterate over
$values = vec[
            // Simple strings
/*1*/        "Hello World",
            'Hello World',

            // String with control chars
/*3*/        "String\nwith\ncontrol\ncharacters\r\n",

            // String with quotes
/*4*/        "String with \"quotes\"",

            //Numeric String
/*5*/        "123456",

            // Hexadecimal string
/*6*/        "0xABC",

            //Heredoc String
/*7*/        $simple_heredoc
];

// loop through each element of the array for strval
$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( strval($value) );
      $iterator++;
};
echo "===DONE===\n";
}
