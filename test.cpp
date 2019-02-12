#include "slist.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>

using namespace std;

string int_to_string(int x) { return to_string(x); }
string intpair_to_string(pair<int,int> x) { 
  return "(" + to_string(x.first) +"," + to_string(x.second) + ")"; 
}
string double_to_string(double x) { return to_string(x); }

using namespace Slist;

int main() {
  cout << "Hello world" << endl; 
  slist<int> x;
  cout << "x Default slist size = " << x.size() << ", uniq=" << x.uniq() << endl;
  auto y = x;
  cout << "y Copy slist size = " << x.size() << ", uniq=" << x.uniq() << endl;
  x = cons (42,x);
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  y = x;
  cout << " y = " << str (int_to_string,y) << endl;
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "y Cons size = " << y.size() << ", uniq=" << y.uniq() << endl;

  x = slist<int>();
  cout << "concat by cons test" << endl;
  x = cons(1,cons(2,cons(3,slist<int>())));
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "Forward= " << str (int_to_string,x) << endl;
  x = rev (x);
  cout << "x Cons size = " << x.size() << ", uniq=" << x.uniq() << endl;
  cout << "Reversed = " << str (int_to_string,x) << endl;
  x = rev (std::move(x)); // in place reversal
  cout << "Reversed again inplace  = " << str (int_to_string,x) << endl;

  auto z = join (x,y);
  cout << " z = x join y = " << str (int_to_string,z) << endl;

  auto z2 = x + y;
  cout << " z2 = x + y = " << str (int_to_string,z2) << endl;

  auto z3 = 99 + z2;
  cout << " z3 = 99 + z2  = " << str (int_to_string,z3) << endl;



  auto k = copy (z);
  cout << " k = copy z " << str (int_to_string,k) << endl;

  cout << " Iterator ";
  for (auto it = z.begin(); !(it == z.end()); ++it) cout << *it << ", ";
  cout << endl;

  cout << "Container "; for (auto v : k) cout << v << ","; cout << endl;

  auto vec = vector<int>();
  vec.push_back(11);
  vec.push_back(12);
  vec.push_back(13);
  vec.push_back(14);
  auto lfv = slist_from_container(vec);
  cout << "List from Vector " << str (int_to_string, lfv) << endl;

  auto linit = slist({1,3,5,7});
  cout << "List from initialiser list " << str (int_to_string, linit) << endl;

  auto lfi = slist_from_iterators<int>(vec.begin(), vec.end());
  cout << "List from iterators" << str (int_to_string, lfi) << endl;

  auto filtered = lfi.filter ([] (int x) { return x % 2 == 0; });
  cout << "filtered " << str (int_to_string, filtered) << endl;

  auto filtered2= filter (lfi,[] (int x) { return x % 2 == 0; });
  cout << "filtered " << str (int_to_string, filtered2) << endl;


  auto mapped = lfi.map<double> ([] (int x) { return double(x) + 0.1; });
  cout << "mapped " << str (double_to_string, mapped) << endl;

  auto mapped2 = map<double>(lfi,[] (int x) { return double(x) + 0.1; });
  cout << "mapped " << str (double_to_string, mapped2) << endl;

  auto zipped = zip(lfv,filtered2);
  cout << "zipped " << str (intpair_to_string, zipped) << endl;
  auto uz = unzip(zipped);
  cout << "unzipped first " << str (int_to_string, uz.first) << endl;
  cout << "unzipped second " << str (int_to_string, uz.second) << endl;

  slist<int> xx;
  auto px = xx.get_output_iterator();
  for (int i = 14; i < 20; ++i) *px++ = i;
  cout << "From output iterator " << str (int_to_string, xx) << endl;
  px = xx.get_output_iterator();
  for (int i = 14; i < 20; ++i) *px++ = i;
  cout << "From output iterator(should append) " << str (int_to_string, xx) << endl;

  auto sum = fold_left ([](int x, int y) { return x + y;},0,xx);
  cout << "Sum of last = " << sum << endl;
  
}
