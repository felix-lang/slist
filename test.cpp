#include "slist.hpp"
#include <iostream>
#include <string>

using namespace std;

string int_to_string(int x) { return to_string(x); }

int main() {
  cout << "Hello world" << endl; 
  Slist::slist<int> x;
  cout << "x Default slist size = " << x.size() << ", uniq=" << x.uniq() << endl;
  auto y = x;
  cout << "y Copy slist size = " << x.size() << ", uniq=" << x.uniq() << endl;
  x = Slist::cons (42,x);
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  y = x;
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "y Cons size = " << y.size() << ", uniq=" << y.uniq() << endl;
  x = Slist::slist<int>();
  cout << "concat by cons test" << endl;
  x = Slist::cons(1,Slist::cons(2,Slist::cons(3,Slist::slist<int>())));
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "Forward= " << Slist::str (int_to_string,x) << endl;
  x = Slist::rev (x);
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "Reversed = " << Slist::str (int_to_string,x) << endl;
  x = Slist::rev (std::move(x)); // in place reversal
  cout << "Reversed again = " << Slist::str (int_to_string,x) << endl;
}
