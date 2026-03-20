<?hh

// fine, as long as all members of T are SDT (following the usual SDT
// trait use rules)

trait T {
  require class C;
}

<<__SupportDynamicType>>
final class C {
  use T;
}
