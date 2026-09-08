//// abstract_method_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalInout {
  public function test(optional inout int $x): void;
}

//// abstract_method_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutOptional {
  public function test(inout optional int $x): void;
}

//// abstract_method_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutNamed {
  public function test(inout named int $x): void;
}

//// abstract_method_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutReadonly {
  public function test(inout readonly int $x): void;
}

//// abstract_method_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedOptional {
  public function test(named optional int $x): void;
}

//// abstract_method_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedInout {
  public function test(named inout int $x): void;
}

//// abstract_method_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyOptional {
  public function test(readonly optional int $x): void;
}

//// abstract_method_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyInout {
  public function test(readonly inout int $x): void;
}

//// abstract_method_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyNamed {
  public function test(readonly named int $x): void;
}

//// abstract_method_optional_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalInoutNamed {
  public function test(optional inout named int $x): void;
}

//// abstract_method_optional_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalInoutReadonly {
  public function test(optional inout readonly int $x): void;
}

//// abstract_method_optional_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalNamedInout {
  public function test(optional named inout int $x): void;
}

//// abstract_method_optional_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalReadonlyInout {
  public function test(optional readonly inout int $x): void;
}

//// abstract_method_optional_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalReadonlyNamed {
  public function test(optional readonly named int $x): void;
}

//// abstract_method_inout_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutOptionalNamed {
  public function test(inout optional named int $x): void;
}

//// abstract_method_inout_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutOptionalReadonly {
  public function test(inout optional readonly int $x): void;
}

//// abstract_method_inout_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutNamedOptional {
  public function test(inout named optional int $x): void;
}

//// abstract_method_inout_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutNamedReadonly {
  public function test(inout named readonly int $x): void;
}

//// abstract_method_inout_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutReadonlyOptional {
  public function test(inout readonly optional int $x): void;
}

//// abstract_method_inout_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutReadonlyNamed {
  public function test(inout readonly named int $x): void;
}

//// abstract_method_named_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedOptionalInout {
  public function test(named optional inout int $x): void;
}

//// abstract_method_named_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedOptionalReadonly {
  public function test(named optional readonly int $x): void;
}

//// abstract_method_named_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedInoutOptional {
  public function test(named inout optional int $x): void;
}

//// abstract_method_named_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedInoutReadonly {
  public function test(named inout readonly int $x): void;
}

//// abstract_method_named_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedReadonlyOptional {
  public function test(named readonly optional int $x): void;
}

//// abstract_method_named_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedReadonlyInout {
  public function test(named readonly inout int $x): void;
}

//// abstract_method_readonly_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyOptionalInout {
  public function test(readonly optional inout int $x): void;
}

//// abstract_method_readonly_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyOptionalNamed {
  public function test(readonly optional named int $x): void;
}

//// abstract_method_readonly_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyInoutOptional {
  public function test(readonly inout optional int $x): void;
}

//// abstract_method_readonly_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyInoutNamed {
  public function test(readonly inout named int $x): void;
}

//// abstract_method_readonly_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyNamedOptional {
  public function test(readonly named optional int $x): void;
}

//// abstract_method_readonly_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyNamedInout {
  public function test(readonly named inout int $x): void;
}

//// abstract_method_optional_inout_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalInoutNamedReadonly {
  public function test(optional inout named readonly int $x): void;
}

//// abstract_method_optional_inout_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalInoutReadonlyNamed {
  public function test(optional inout readonly named int $x): void;
}

//// abstract_method_optional_named_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalNamedInoutReadonly {
  public function test(optional named inout readonly int $x): void;
}

//// abstract_method_optional_named_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalNamedReadonlyInout {
  public function test(optional named readonly inout int $x): void;
}

//// abstract_method_optional_readonly_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalReadonlyInoutNamed {
  public function test(optional readonly inout named int $x): void;
}

//// abstract_method_optional_readonly_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalReadonlyNamedInout {
  public function test(optional readonly named inout int $x): void;
}

//// abstract_method_inout_optional_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutOptionalNamedReadonly {
  public function test(inout optional named readonly int $x): void;
}

//// abstract_method_inout_optional_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutOptionalReadonlyNamed {
  public function test(inout optional readonly named int $x): void;
}

//// abstract_method_inout_named_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutNamedOptionalReadonly {
  public function test(inout named optional readonly int $x): void;
}

//// abstract_method_inout_named_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutNamedReadonlyOptional {
  public function test(inout named readonly optional int $x): void;
}

//// abstract_method_inout_readonly_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutReadonlyOptionalNamed {
  public function test(inout readonly optional named int $x): void;
}

//// abstract_method_inout_readonly_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutReadonlyNamedOptional {
  public function test(inout readonly named optional int $x): void;
}

//// abstract_method_named_optional_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedOptionalInoutReadonly {
  public function test(named optional inout readonly int $x): void;
}

//// abstract_method_named_optional_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedOptionalReadonlyInout {
  public function test(named optional readonly inout int $x): void;
}

//// abstract_method_named_inout_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedInoutOptionalReadonly {
  public function test(named inout optional readonly int $x): void;
}

//// abstract_method_named_inout_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedInoutReadonlyOptional {
  public function test(named inout readonly optional int $x): void;
}

//// abstract_method_named_readonly_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedReadonlyOptionalInout {
  public function test(named readonly optional inout int $x): void;
}

//// abstract_method_named_readonly_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedReadonlyInoutOptional {
  public function test(named readonly inout optional int $x): void;
}

//// abstract_method_readonly_optional_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyOptionalInoutNamed {
  public function test(readonly optional inout named int $x): void;
}

//// abstract_method_readonly_optional_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyOptionalNamedInout {
  public function test(readonly optional named inout int $x): void;
}

//// abstract_method_readonly_inout_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyInoutOptionalNamed {
  public function test(readonly inout optional named int $x): void;
}

//// abstract_method_readonly_inout_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyInoutNamedOptional {
  public function test(readonly inout named optional int $x): void;
}

//// abstract_method_readonly_named_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyNamedOptionalInout {
  public function test(readonly named optional inout int $x): void;
}

//// abstract_method_readonly_named_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyNamedInoutOptional {
  public function test(readonly named inout optional int $x): void;
}

//// abstract_method_optional_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodOptionalOptional {
  public function test(optional optional int $x): void;
}

//// abstract_method_inout_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodInoutInout {
  public function test(inout inout int $x): void;
}

//// abstract_method_named_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodNamedNamed {
  public function test(named named int $x): void;
}

//// abstract_method_readonly_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

interface AbstractMethodReadonlyReadonly {
  public function test(readonly readonly int $x): void;
}

//// function_type_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalInout = (function(optional inout int): void);

//// function_type_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutOptional = (function(inout optional int): void);

//// function_type_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutNamed = (function(inout named int $x): void);

//// function_type_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedOptional = (function(named optional int $x): void);

//// function_type_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedInout = (function(named inout int $x): void);

//// function_type_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyOptional = (function(readonly optional int): void);

//// function_type_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyInout = (function(readonly inout int): void);

//// function_type_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyNamed = (function(readonly named int $x): void);

//// function_type_optional_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalInoutNamed = (function(optional inout named int $x): void);

//// function_type_optional_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalInoutReadonly = (function(optional inout readonly int): void);

//// function_type_optional_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalNamedInout = (function(optional named inout int $x): void);

//// function_type_optional_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalReadonlyInout = (function(optional readonly inout int): void);

//// function_type_optional_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalReadonlyNamed = (function(optional readonly named int $x): void);

//// function_type_inout_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutOptionalNamed = (function(inout optional named int $x): void);

//// function_type_inout_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutOptionalReadonly = (function(inout optional readonly int): void);

//// function_type_inout_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutNamedOptional = (function(inout named optional int $x): void);

//// function_type_inout_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutNamedReadonly = (function(inout named readonly int $x): void);

//// function_type_inout_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutReadonlyOptional = (function(inout readonly optional int): void);

//// function_type_inout_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutReadonlyNamed = (function(inout readonly named int $x): void);

//// function_type_named_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedOptionalInout = (function(named optional inout int $x): void);

//// function_type_named_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedOptionalReadonly = (function(named optional readonly int $x): void);

//// function_type_named_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedInoutOptional = (function(named inout optional int $x): void);

//// function_type_named_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedInoutReadonly = (function(named inout readonly int $x): void);

//// function_type_named_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedReadonlyOptional = (function(named readonly optional int $x): void);

//// function_type_named_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedReadonlyInout = (function(named readonly inout int $x): void);

//// function_type_readonly_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyOptionalInout = (function(readonly optional inout int): void);

//// function_type_readonly_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyOptionalNamed = (function(readonly optional named int $x): void);

//// function_type_readonly_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyInoutOptional = (function(readonly inout optional int): void);

//// function_type_readonly_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyInoutNamed = (function(readonly inout named int $x): void);

//// function_type_readonly_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyNamedOptional = (function(readonly named optional int $x): void);

//// function_type_readonly_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyNamedInout = (function(readonly named inout int $x): void);

//// function_type_optional_inout_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalInoutNamedReadonly = (function(optional inout named readonly int $x): void);

//// function_type_optional_inout_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalInoutReadonlyNamed = (function(optional inout readonly named int $x): void);

//// function_type_optional_named_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalNamedInoutReadonly = (function(optional named inout readonly int $x): void);

//// function_type_optional_named_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalNamedReadonlyInout = (function(optional named readonly inout int $x): void);

//// function_type_optional_readonly_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalReadonlyInoutNamed = (function(optional readonly inout named int $x): void);

//// function_type_optional_readonly_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalReadonlyNamedInout = (function(optional readonly named inout int $x): void);

//// function_type_inout_optional_named_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutOptionalNamedReadonly = (function(inout optional named readonly int $x): void);

//// function_type_inout_optional_readonly_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutOptionalReadonlyNamed = (function(inout optional readonly named int $x): void);

//// function_type_inout_named_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutNamedOptionalReadonly = (function(inout named optional readonly int $x): void);

//// function_type_inout_named_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutNamedReadonlyOptional = (function(inout named readonly optional int $x): void);

//// function_type_inout_readonly_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutReadonlyOptionalNamed = (function(inout readonly optional named int $x): void);

//// function_type_inout_readonly_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutReadonlyNamedOptional = (function(inout readonly named optional int $x): void);

//// function_type_named_optional_inout_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedOptionalInoutReadonly = (function(named optional inout readonly int $x): void);

//// function_type_named_optional_readonly_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedOptionalReadonlyInout = (function(named optional readonly inout int $x): void);

//// function_type_named_inout_optional_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedInoutOptionalReadonly = (function(named inout optional readonly int $x): void);

//// function_type_named_inout_readonly_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedInoutReadonlyOptional = (function(named inout readonly optional int $x): void);

//// function_type_named_readonly_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedReadonlyOptionalInout = (function(named readonly optional inout int $x): void);

//// function_type_named_readonly_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedReadonlyInoutOptional = (function(named readonly inout optional int $x): void);

//// function_type_readonly_optional_inout_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyOptionalInoutNamed = (function(readonly optional inout named int $x): void);

//// function_type_readonly_optional_named_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyOptionalNamedInout = (function(readonly optional named inout int $x): void);

//// function_type_readonly_inout_optional_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyInoutOptionalNamed = (function(readonly inout optional named int $x): void);

//// function_type_readonly_inout_named_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyInoutNamedOptional = (function(readonly inout named optional int $x): void);

//// function_type_readonly_named_optional_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyNamedOptionalInout = (function(readonly named optional inout int $x): void);

//// function_type_readonly_named_inout_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyNamedInoutOptional = (function(readonly named inout optional int $x): void);

//// function_type_optional_optional.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeOptionalOptional = (function(optional optional int): void);

//// function_type_inout_inout.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeInoutInout = (function(inout inout int): void);

//// function_type_named_named.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeNamedNamed = (function(named named int $x): void);

//// function_type_readonly_readonly.php
<?hh
<<file:__EnableUnstableFeatures('named_parameters')>>

type FunctionTypeReadonlyReadonly = (function(readonly readonly int): void);
