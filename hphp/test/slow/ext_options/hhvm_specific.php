<?hh


<<__EntryPoint>>
function main_hhvm_specific() :mixed{
var_dump(ini_get('hhvm.stats.slot_duration'));
var_dump(ini_get('hhvm.pid_file'));
}
