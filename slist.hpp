// Requires C++11
#pragma once

#include <cstdlib>
#include <utility>
#include <initializer_list>
#include <cassert>

// for debugging only
#include <iostream>
using namespace std;

namespace Slist {

  template<class T>
  struct node_t {
     ::std::size_t refcnt;
     node_t *next;
     T data;
  };

  template<class U>
  struct output_control_t {
    node_t<U> **last;
    node_t<U> *cur;
    output_control_t(node_t<U> **phead) { 
      last = phead; 
      cur = nullptr; 
      assert (*phead==nullptr); 
    }
    void put(U const &v) {
      cur = new node_t<U> {1,nullptr,v};
      *last = cur;
      last = &(cur->next);
    }
    output_control_t& operator++() {return *this; }
    output_control_t& operator++(int) {return *this; }

    template<class V>
    struct output_proxy_t { 
      output_control_t *p;
      output_proxy_t(output_control_t<V> *q) : p(q) {}
      void operator=(V const &v) { p->put(v); }
    };
    output_proxy_t<U> operator*() { return this; } 
  };


  template<class T>
  class slist {

    template<class U>
    friend slist<U> operator + (U,slist<U> const&);

    template<class U>
    friend slist<U> rev(slist<U> &&x);

    template<class U>
    friend slist<U> join (slist<U> const &a, slist<U> const &b);

    template<class U>
    friend slist<U> copy(slist<U> const &x);

    template<class C>
    friend auto slist_from_container(C const &c) -> slist<typename C::value_type>;

    template<class U, class I>
    friend slist<U> slist_from_iterators(I const &begin, I const &end);

    template<class U, class V>
    friend slist<std::pair<U,V> > zip (slist<U> const &a, slist<V> const &b);

    template<class U, class V>
    friend std::pair<slist<U>,slist<V>> unzip (slist<std::pair<U,V>> const &a);


    template<class U>
    friend class slist;

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
      node_t<T> *p2 = nullptr;
      while(p1) {
        p2 = p1;
        p1 = p1 -> next;
      }
      return p2;
    }

    // a pointer to the slot containing the next pointer
    // of the last node, or, if there is no last node,
    // the list handle pointer. The pointed at slot
    // should always be NULL! The precondition for this
    // pointer to be safely used is that the list is unique!
    node_t<T> **plast_next() {
      return p?&(last()->next):&p;
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

    slist (std::initializer_list<T> const &c) : p(nullptr) {
      output_control_t<T> o(&p);
      for (auto v : c) o.put(v);
    }

    // cons constructor
    slist (T head, slist const &tail) : p(new node_t<T> {1,tail.p,head}) { 
      tail.incref(); 
    }

    // cons constructor (rvalue tail)
    slist (T head, slist &&tail) : p(new node_t<T> {1,tail.p,head}) { 
      tail.p=nullptr;
    }

    // copy constructor increments refcnt if not empty
    slist(slist const& that)noexcept : p(that.p) { 
      //cout << "Copy ctor" << endl;
      incref(); 
    }

    // move constructor sets argument to empty
    slist(slist && that)noexcept : p(that.p) { 
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
    bool empty() const noexcept { return p == nullptr; }

    // unique list: not empty list is considered uniq
    bool uniq() const noexcept { 
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


    size_t size() const noexcept {
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

    T head() const { assert(p); return p->data; }

    slist tail () const { assert(p); return p->next; }

    // TODO: if the list is unique and the map
    // is an operator T->T the data can just be updated in place
    template<class U, class F>
    slist<U> map(F f) const {
      slist<U> s;
      output_control_t<U> o(&s.p);
      for (auto v : *this) o.put(f(v));
      return s;
    }


    // TODO: provide unique version
    // if the list is unique, filter can just chain together
    // the nodes of the selected elements.
    template<class F>
    slist filter(F f) const {
      slist<T> s;
      output_control_t<T> o(&s.p);
      for (auto v : *this)
        if (f (v)) o.put(v);
      return s;
    }



    // **********************************************************
    // iterators
    //
    // We provide two types of iterator.
    // 
    // Input iterator is slower, but consumes the list if it is unique.
    // It requires and increment and subsequent decrement of the current
    // node ref count as it scans. However, there is no increment of the
    // first node if is acquired from an rvalue list, so when it advances
    // past the first node, that node is deleted if node has a ref count of 1.
    //
    // Forward iterator is faster because it uses a weak pointer for
    // scanning which does not touch the reference count. The list is held
    // intact during the whole scan by incrementing the count of the
    // head node.  Although much faster, the whole list is alway retained
    // until the scan is complete.
    //
    // The faster forward iterator is the default, because it has better
    // performance and no storage overhead in the usual case where
    // the list is already not unique, that is, it is either an lvalue,
    // or an rvalue refering to a list which has other references to it
    // anyhow.
    
 
    template<class U>
    class slist_input_iterator;
    friend slist_input_iterator<T>;

    template<class U>
    class slist_input_iterator  {
      slist<U> s;
    public:
      slist_input_iterator (slist<U> const& x) : s(x) {}
 
      // pre-incr (set to tail, return prev)
      slist_input_iterator &operator ++ () { s.p = s.p-> next; return *this; }
      // post-incr (set to tail, return value)
      slist_input_iterator operator ++ (int) { auto cur = *this; s.p = s.p-> next; return cur; }
      // deref (head)
      U operator *() const { return s.p->data; }
      // identity (not value equality)
      bool operator == (slist_input_iterator<T> const &y) const { return s.p == y.s.p; }
      // identity (not value equality)
      bool operator != (slist_input_iterator<T> const &y) const { return s.p != y.s.p; }
    };

    template<class U>
    class slist_forward_iterator;
    friend slist_forward_iterator<T>;

    template<class U>
    class slist_forward_iterator  {
      slist<U> s; // holds list intact during scan
      node_t<U> *p; // weak pointer to current node
    public:
      slist_forward_iterator (slist<U> const& x) : s(x), p(x.p) {}
 
      // pre-incr (set to tail, return prev)
      slist_forward_iterator &operator ++ () { p = p-> next; return *this; }
      // post-incr (set to tail, return value)
      slist_forward_iterator operator ++ (int) { auto cur = *this; p = p-> next; return cur; }
      // deref (head)
      U operator *() const { return p->data; }
      // identity (not value equality)
      bool operator == (slist_forward_iterator<T> const &y) const { return p == y.p; }
      // identity (not value equality)
      bool operator != (slist_forward_iterator<T> const &y) const { return p != y.p; }
    };

    // The default iterator is the faster forward iterator
    // start iterator
    slist_forward_iterator<T> begin() const { return slist_forward_iterator<T> {*this} ; }
    // end iterator
    slist_forward_iterator <T> end() const { return slist<T>(); }

    // start iterator
    slist_input_iterator<T> begin_input() const { return slist_input_iterator<T> {*this} ; }
    // end iterator
    slist_input_iterator <T> end_input() const { return slist<T>(); }

    output_control_t<T> get_output_iterator() {
      // ensure the list is uniq
      if(!uniq()) { decref(); p = nullptr; }
      // pointer to slot containing node pointer
      node_t<T> **q = plast_next();
      assert (*q == nullptr);
      return output_control_t(q);
    }

  };  // slist



  // **********************************************************
  // functional interface
  // **********************************************************

  template<class T>
  size_t size(slist<T> const &x) { return x.size(); }

  // **********************************************************
  // cons
  template<class T>
  slist<T> cons (T head, slist<T> const &tail) { return tail.cons(head); }

  template<class T>
  slist<T> cons (T head, slist<T> &&tail) { return ::std::move(tail).cons(head); }


  // **********************************************************
  // head
  // precondition: x not empty
  template<class T>
  T head (slist<T> const &x) { return x.head(); }

  // **********************************************************
  // tail
  // precondition: x not empty
  template<class T>
  slist<T> tail (slist<T> const &x) { return x.tail(); }

  // **********************************************************
  // uniqueness test
  template<class T>
  bool uniq (slist<T> const &x) { return x.uniq(); }

  // **********************************************************
  // empty test
  template<class T>
  bool empty (slist<T> const &x) { return x.empty(); }


  // **********************************************************
  // conversion to string
  template<class T, class F> 
  string str(F f, slist<T> const &x) {
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
  slist<T> rev (slist<T> const &a) {
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
  slist<T> rev(slist<T> &&x) {
    if(x.uniq()) {
      x.inplace_rev();
      return std::move(x);
    }
    return rev(x);
  }

  template<class T>
  slist<T> copy(slist<T> const &x) {
    slist<T> s;
    output_control_t<T> o(&s.p);
    for (auto v: x) o.put(v);
    return s;
  }

  // **********************************************************
  // join two lists
  // unoptimised
  // FIXME! C++ can copy a list without reversing it
  template<class T>
  slist<T> join (slist<T> const &a, slist<T> const &b) {
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
  // **********************************************************
  // Construct from STL container
  // container value type must be T
  template<class C>
  auto slist_from_container(C const &c) -> slist<typename C::value_type> {
    using T = typename C::value_type;
    slist<T> s;
    output_control_t<T> o(&s.p);
    for (auto v : c) o.put(v);
    return s;
  }

  // **********************************************************
  // Construct from STL begin/end iterators
  // iterator value type must be T
  template<class T, class I>
  slist<T> slist_from_iterators(I const &begin, I const &end) {
    slist<T> s;
    output_control_t<T> o(&s.p);
    for (auto j  = begin; j != end; ++j) o.put(*j);
    return s;
  }

  template<class T>
  slist<T> operator+(T head, slist<T> const &tail) 
    { return tail.cons(head); }

  template<class T>
  slist<T> operator+(T head, slist<T> &&tail) 
    { return ::std::move(tail).cons(head); }

  template<class T>
  slist<T> operator + (slist<T> const &a, slist<T> const &b) 
    { return Slist::join(a,b); }

  template<class U, class T, class F>
  slist<U> map(slist<T> const &x, F f) { return x.slist<T>::template map<U>(f); }
 
  template<class T, class F>
  slist<T> filter(slist<T> const &x, F f) { return x.filter(f); }
  
  template<class T, class U>
  slist<std::pair<T,U> > zip (slist<T> const &a, slist<U> const &b) {
    using P = std::pair<T,U>;
    auto i = a.begin();
    auto ie = a.end();
    auto j = b.begin();
    auto je = b.end();
    slist<P> s;
    output_control_t<P> o(&s.p);
    for (;i != ie && j != je; ++i,++j) {
      o.put(make_pair(*i,*j));
    }
    return s;
  } 

  template<class T, class U>
  std::pair<slist<T>,slist<U>> unzip (slist<pair<T,U>> const &a) {
    using P = std::pair<T,U>;
    auto i = a.begin();
    auto ie = a.end();
    slist<T> ls;
    slist<U> rs;
    output_control_t<T> l(&ls.p);
    output_control_t<U> r(&rs.p);

    for (;i != ie ; ++i) {
      auto v = *i;
      l.put(v.first);
      r.put(v.second);
    }
    return make_pair(ls,rs);
  } 

}; // Slist



