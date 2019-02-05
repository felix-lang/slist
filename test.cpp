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

}
