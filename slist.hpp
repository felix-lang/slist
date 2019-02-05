// Requires C++11
#pragma once

#include <cstdlib>

// for debugging only
#include <iostream>
using namespace std;

class Slist {
  template<class T>
  struct node_t {
     ::std::size_t refcnt;
     node_t *next;
     T data;
  };

public:
  template<class T>
  class slist {
    node_t<T> *p;

    void decref() {
// cout << "decref-start" << endl;
      auto top = p;
      while(top && --top->refcnt == 0) {
// cout << "decref loop" << endl;
        auto tmp = top->next;
        delete top;
        top = tmp;
      }
// cout << "decref-end" << endl;
    }
    void incref() const {
// cout << "incref-start" << endl;
      if(p)p->refcnt++;
// cout << "incref-end" << endl;
    }

    slist (node_t<T> *q) : p(q) { incref(); }

  public:
    // destructor
    ~slist() { cout << "slist destructor" << endl; decref(); }

    // default constructor is empty list
    slist() : p (nullptr) {}

    // one element list
    slist (T v) : p(new node_t<T> {1,nullptr,v}) {}

    // cons constructor
    slist (T head, slist const &tail) : p(new node_t<T> {1,tail.p,head}) { 
// cout << "cons constructor" << endl; 
      tail.incref(); 
    }

    // copy constructor increments refcnt is not empty
    slist(slist const& that): p(that.p) { incref(); }

    // move constructor sets argument to empty
    slist(slist && that): p(that.p) { that.p = nullptr; }

    // copy assignment
    slist &operator=(slist const& that) {
//cout << "Copy assignment start" << endl;
      if (this != &that) {
//cout << "Copy assignment do" << endl;
         decref();
         p = that.p;
         incref();
      }
//cout << "Copy assignment end" << endl;
      return *this;
    }

    // move assignment
    slist &operator=(slist && that) {
// cout << "rvalue assignment" << endl;
      decref();
      p = that.p;
      that.p = nullptr;
      return *this;
    }

    // empty list
    bool empty() const { return p == nullptr; }

    // unique list: not empty list is considered uniq
    bool uniq() const { 
// cout << "unique start" << endl;
      for(auto q = p; q; q=q->next)
        if(q->refcnt>1) return false; 
      return true; 
    }

    // cons: return a new list with given head and this list as the tail
    slist cons (T head) const {
//cout << "Cons method" << endl;
      return slist (head, *this);
    }

    size_t size() const {
      size_t n = 0; 
      for(auto q=p;q;q=q->next,++n); 
      return n; 
    }
  };  // slist
 
  // functional interface

  // cons
  template<class T>
  static slist<T> cons (T head, slist<T> const &tail) { return tail.cons(head); }

  // precondition: x not empty
  template<class T>
  static T head (slist<T> const &x) { return x.p->data; }

  // tail
  // precondition: x not empty
  template<class T>
  static slist<T> tail (slist<T> const &x) { return slist<T> (x.p->next); }

  // uniqueness test
  template<class T>
  static bool uniq (slist<T> const &x) { return x.uniq(); }

  // empty test
  template<class T>
  static bool empty (slist<T> const &x) { return x.empty(); }


}; // Slist
