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
    friend class Slist;
    node_t<T> *p;

    void decref() {
      auto top = p;
      while(top && --top->refcnt == 0) {
        auto tmp = top->next;
        delete top;
        top = tmp;
      }
    }
    void incref() const {
      if(p)p->refcnt++;
    }

    slist (node_t<T> *q) : p(q) { incref(); }

    void inplace_rev() {
      node_t<T> *nutail = nullptr; 
      auto cur = p;
      while(cur)
      {
        auto oldtail = cur->next;   // save old tail in temp
        cur->next= nutail;          // overwrite current node tail
        nutail = cur;                                   // set new tail to current
        cur = oldtail;                                  // set current to saved old tail
      }
      p = nutail;               
    }
    node_t<T> *last() const {
      auto p1 = p;
      node_t<T> *p2 = nullptr;;
      while(p1) {
        p2 = p1;
        p1 = p1 -> next;
      }
      return p2;
    }

    // splice two lists together
    // not safe unless this list is unqqie
    // this routine does NOT increment y's refcount
    // because if it sourced from an rvalue it is invariant,
    // but should be incremented if sourced from an lvalue
    // the caller must do the incref!
    void splice (node_t<T> *y) {
      if (!p) { // this list is empty 
        if(y) { // that list is non-empty
          p = y;  // set this list to that list
        } 
        // else both lists empty, nothing to do
        return; 
      }
cout << "splice to last" << endl;
      // this list is not empty
      if(y) last() -> next = y;
      return;
    }
     


  public:
    // destructor
    ~slist() {
      //cout << "slist destructor" << endl; 
      decref(); 
    }

    // default constructor is empty list
    slist() : p (nullptr) {}

    // one element list
    slist (T v) : p(new node_t<T> {1,nullptr,v}) {}

    // cons constructor
    slist (T head, slist const &tail) : p(new node_t<T> {1,tail.p,head}) { 
      tail.incref(); 
    }

    // cons constructor (rvalue tail)
    slist (T head, slist &&tail) : p(new node_t<T> {1,tail.p,head}) { 
      tail.p=nullptr;
    }

    // copy constructor increments refcnt if not empty
    slist(slist const& that): p(that.p) { 
      //cout << "Copy ctor" << endl;
      incref(); 
    }

    // move constructor sets argument to empty
    slist(slist && that): p(that.p) { 
      that.p = nullptr; 
    }

    // copy assignment
    slist &operator=(slist const& that) {
      if (this != &that) {
         decref();
         p = that.p;
         incref();
      }
      return *this;
    }

    // move assignment
    slist &operator=(slist && that) {
      decref();
      p = that.p;
      that.p = nullptr;
      return *this;
    }

    // empty list
    bool empty() const { return p == nullptr; }

    // unique list: not empty list is considered uniq
    bool uniq() const { 
      for(auto q = p; q; q=q->next)
        if(q->refcnt>1) return false; 
      return true; 
    }

    // cons: return a new list with given head and this list as the tail
    slist cons (T head) const & {
      return slist (head, *this);
    }

   slist cons (T head) && {
      return slist (head, ::std::move(*this));
    }


    size_t size() const {
      size_t n = 0; 
      for(auto q=p;q;q=q->next,++n); 
      return n; 
    }

    slist rev() const {
      slist<T> r; // empty
      auto q = p;
      while(q) {
        r = r.cons(q->data);
        q = q->next;
      }
      return r;
    }

    T head() const { return p->data; }

    slist tail () const { return p->next; }

    template<class U>
    class slist_iterator;

    friend slist_iterator<T>;

    // **********************************************************
    // iterator
    template<class U>
    class slist_iterator  {
      slist<U> s;
    public:
      slist_iterator (slist<U> const& x) : s(x) {}
 
      // pre-incr (set to tail, return prev)
      slist_iterator &operator ++ () { s.p = s.p-> next; return *this; }
      // post-incr (set to tail, return value)
      slist_iterator operator ++ (int) { auto cur = *this; s.p = s.p-> next; return cur; }
      // deref (head)
      U operator *() const { return s.p->data; }
      // identity (not value equality)
      bool operator == (slist_iterator<T> const &y) const { return s.p == y.s.p; }
      // identity (not value equality)
      bool operator != (slist_iterator<T> const &y) const { return s.p != y.s.p; }
    };

    // start iterator
    slist_iterator<T> begin() const { return slist_iterator<T> {*this} ; }
    // end iterator
    slist_iterator <T> end() const { return slist<T>(); }


  };  // slist



  // **********************************************************
  // functional interface
  // **********************************************************

  template<class T>
  static size_t size(slist<T> const &x) { return x.size(); }

  // **********************************************************
  // cons
  template<class T>
  static slist<T> cons (T head, slist<T> const &tail) { return tail.cons(head); }

  template<class T>
  static slist<T> cons (T head, slist<T> &&tail) { return ::std::move(tail).cons(head); }

  // **********************************************************
  // head
  // precondition: x not empty
  template<class T>
  static T head (slist<T> const &x) { return x.head(); }

  // **********************************************************
  // tail
  // precondition: x not empty
  template<class T>
  static slist<T> tail (slist<T> const &x) { return x.tail(); }

  // **********************************************************
  // uniqueness test
  template<class T>
  static bool uniq (slist<T> const &x) { return x.uniq(); }

  // **********************************************************
  // empty test
  template<class T>
  static bool empty (slist<T> const &x) { return x.empty(); }


  // **********************************************************
  // conversion to string
  template<class T, class F> 
  static string str(F f, slist<T> const &x) {
    if (x.empty()) return "()";
    auto h = head (x);
    auto t = tail (x);
    auto s = "(" + f (h);
    for(;!empty(t); t = tail(t)) {
      h = head (t);
      s += ", " + f(h);
    }
    return s + ")";
  }

  // **********************************************************
  // copying rev
  template<class T>
  static slist<T> rev (slist<T> const &a) {
    auto x = a;
    auto y = slist<T>();
    while(!x.empty()) {
      y = cons(x.head(), y);
      x = x.tail();
    }
    return std::move(y);
  }

  // maybe in place rev
  template<class T>
  static slist<T> rev(slist<T> &&x) {
    if(x.uniq()) {
      x.inplace_rev();
      return std::move(x);
    }
    return rev(x);
  }

  template<class T>
  static slist<T> copy(slist<T> const &x) {
    auto res = slist<T>(); // Empty
    auto pprev = &(res.p);  // place to put pointer to new node
    auto q = x.p;
    while(q) {
      auto newnode = new node_t<T>{1,nullptr, q->data};
      q = q->next;
      *pprev = newnode;
      pprev = &(newnode->next);
    }
    return std::move(res);
  }

  // **********************************************************
  // join two lists
  // unoptimised
  // FIXME! C++ can copy a list without reversing it
  template<class T>
  static slist<T> join (slist<T> const &a, slist<T> const &b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    slist<T> res;
    slist<T> left = a;
    while(!left.empty()) { res = res.cons(left.head()); left = left.tail(); }
    res.inplace_rev();
    // note: faster splice uses the fact we already know the tail node
    res.splice(b.p);
    b.incref();
    return res;
  }
}; // Slist
