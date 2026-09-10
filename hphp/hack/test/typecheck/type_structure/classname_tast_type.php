<?hh

class Ent {}

interface IHelper {
  abstract const type TEnt as Ent;
}

class Helper implements IHelper {
  const type TEnt = Ent;
}

function get_cls(): classname<Ent> {
  return nameof Ent;
}

function get_helper(): class<IHelper> {
  return Helper::class;
}

// The synthesised TAST node must carry the class(name) type, not the
// underlying TypeStructure, or TAST checks see disjoint types.
function test(): void {
  $cls = get_cls();
  $helper = get_helper();
  HH\type_structure_classname($helper, 'TEnt') === $cls;
  HH\type_structure_class($helper, 'TEnt') === Helper::class;
}
