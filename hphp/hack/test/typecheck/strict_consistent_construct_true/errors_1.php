<?hh
<<__ConsistentConstruct>>
abstract final class AbstractFinalCCDirect {}

<<__ConsistentConstruct>>
abstract class AbstractCCRoot {}
abstract final class AbstractFinalChildOfCC extends AbstractCCRoot {}

<<__ConsistentConstruct>>
class CCGrandparent {}
class MiddleClass extends CCGrandparent {}
abstract final class AbstractFinalGrandchild extends MiddleClass {}
