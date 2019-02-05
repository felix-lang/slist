#include "slist.hpp"
#include <iostream>
using namespace std;

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
}
