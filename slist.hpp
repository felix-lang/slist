// Requires C++11
#pragma once

#include <cstdlib>
#include <utility>
#include <initializer_list>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace Slist {

  template<class T>
  class slist {

    struct slist_node_t {
       ::std::size_t refcnt;
       slist_node_t *next;
       T data;
    };

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

    slist_node_t *p;

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

    slist (slist_node_t *q) : p(q) { incref(); }

    void inplace_rev() {
      slist_node_t *nutail = nullptr; 
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
    slist_node_t *last() const {
      auto p1 = p;
      slist_node_t *p2 = nullptr;
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
    slist_node_t **plast_next() {
      return p?&(last()->next):&p;
    }

    // splice two lists together
    // not safe unless this list is unqqie
    // this routine does NOT increment y's refcount
    // because if it sourced from an rvalue it is invariant,
    // but should be incremented if sourced from an lvalue
    // the caller must do the incref!
    void splice (slist_node_t *y) {
      if (!p) { // this list is empty 
        if(y) { // that list is non-empty
          p = y;  // set this list to that list
        } 
        // else both lists empty, nothing to do
        return; 
      }
      // this list is not empty
      if(y) last() -> next = y;
      return;
    }
     
    template<class F>
    slist<T> map(F && f, T const *, int) const {
      auto u = uniq();
      std::cout << "f: T -> T, uniq=" << u << std::endl;
      if (u) {
        for(auto q = p; q != nullptr; q=q->next) q->data = f(q->data);
        return *this;
      }
      else {
        slist<T> s;
        typename slist<T>::output_control_t o(&s.p);
        for (auto v : *this) o.put(f(v));
        return s;
      }
    }
    
    template<class U, class F>
    slist<U> map(F && f, U const *, ...) const {
        std::cout << "f: T -> U" << std::endl;
      slist<U> s;
      typename slist<U>::output_control_t o(&s.p);
      for (auto v : *this) o.put(f(v));
      return s;
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
    slist (T v) : p(new slist_node_t {1,nullptr,v}) {}

    slist (std::initializer_list<T> const &c) : p(nullptr) {
      output_control_t o(&p);
      for (auto v : c) o.put(v);
    }

    // cons constructor
    slist (T head, slist const &tail) : p(new slist_node_t {1,tail.p,head}) { 
      tail.incref(); 
    }

    // cons constructor (rvalue tail)
    slist (T head, slist &&tail) : p(new slist_node_t {1,tail.p,head}) { 
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

    template<class F>
    auto map(F && f) const -> slist<decltype(f(std::declval<T>()))> {
        using U = decltype(f(std::declval<T>()));
        return map(std::forward<F>(f), reinterpret_cast<U*>(0), 0);
    }

    // TODO: provide unique version
    // if the list is unique, filter can just chain together
    // the nodes of the selected elements.
    template<class F>
    slist filter(F f) const {
      slist<T> s;
      typename slist::output_control_t o(&s.p);
      for (auto v : *this)
        if (f (v)) o.put(v);
      return s;
    }

    template<class U, class F>
    U fold_left(U init, F f) const {
      return std::accumulate(begin(), end(), init, f);
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

    struct output_control_t {
      using difference_type = void;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::output_iterator_tag; 

      slist_node_t **last;
      slist_node_t *cur;
      output_control_t(slist_node_t **phead) { 
        last = phead; 
        cur = nullptr; 
        assert (*phead==nullptr); 
      }
      void put(T const &v) {
        cur = new slist_node_t {1,nullptr,v};
        *last = cur;
        last = &(cur->next);
      }
      output_control_t& operator++() {return *this; }
      output_control_t& operator++(int) {return *this; }

      template<class V>
      struct output_proxy_t { 
        output_control_t *p;
        output_proxy_t(typename slist<V>::output_control_t *q) : p(q) {}
        void operator=(V const &v) { p->put(v); }
      };
      output_proxy_t<T> operator*() { return this; } 
    };

 
 
    class slist_input_iterator;
    friend slist_input_iterator;

    class slist_input_iterator  {
      slist s;
    public:
      using difference_type = void;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::forward_iterator_tag; 

      slist_input_iterator (slist<T> const& x) : s(x) {}
 
      // pre-incr (set to tail, return prev)
      slist_input_iterator &operator ++ () { s.p = s.p-> next; return *this; }
      // post-incr (set to tail, return value)
      slist_input_iterator operator ++ (int) { auto cur = *this; s.p = s.p-> next; return cur; }
      // deref (head)
      T operator *() const { return s.p->data; }
      // identity (not value equality)
      bool operator == (slist_input_iterator const &y) const { return s.p == y.s.p; }
      // identity (not value equality)
      bool operator != (slist_input_iterator const &y) const { return s.p != y.s.p; }
    };

    class slist_forward_iterator;
    friend slist_forward_iterator;

    class slist_forward_iterator  {
      slist s; // holds list intact during scan
      slist_node_t *p; // weak pointer to current node
    public:
      using difference_type = void;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::forward_iterator_tag; 

      slist_forward_iterator (slist<T> const& x) : s(x), p(x.p) {}
 
      // pre-incr (set to tail, return prev)
      slist_forward_iterator &operator ++ () { p = p-> next; return *this; }
      // post-incr (set to tail, return value)
      slist_forward_iterator operator ++ (int) { auto cur = *this; p = p-> next; return cur; }
      // deref (head)
      T operator *() const { return p->data; }
      // identity (not value equality)
      bool operator == (slist_forward_iterator const &y) const { return p == y.p; }
      // identity (not value equality)
      bool operator != (slist_forward_iterator const &y) const { return p != y.p; }
    };

    // The default iterator is the faster forward iterator
    // start iterator
    slist_forward_iterator begin() const { return slist_forward_iterator {*this} ; }
    // end iterator
    slist_forward_iterator end() const { return slist<T>(); }

    // start iterator
    slist_input_iterator begin_input() const { return slist_input_iterator {*this} ; }
    // end iterator
    slist_input_iterator end_input() const { return slist(); }

    slist::output_control_t get_back_inserter() {
      // ensure the list is uniq
      if(uniq()) {
        // pointer to slot containing node pointer
        slist_node_t **q = plast_next();
        assert (*q == nullptr);
        return output_control_t(q);
      }
      else { // the list isn't unique, so copy it
std::cout << " back inserter copying list " << std::endl;
        auto newhead = new slist_node_t {1,nullptr,p->data};
        output_control_t o(&newhead->next);
        for(auto q=p->next; q; q=q->next) o.put(q->data);
        decref();
        p = newhead;
        return o; 
      }
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
  std::string str(F f, slist<T> const &x) {
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
    typename slist<T>::output_control_t o(&s.p);
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
    typename slist<T>::output_control_t o(&s.p);
    for (auto v : c) o.put(v);
    return s;
  }

  // **********************************************************
  // Construct from STL begin/end iterators
  // iterator value type must be T
  template<class T, class I>
  slist<T> slist_from_iterators(I const &begin, I const &end) {
    slist<T> s;
    typename slist<T>::output_control_t o(&s.p);
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

  template<class T, class F>
  auto map(slist<T> const &x, F f)  { return x.template map(f); }
 
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
    typename slist<P>::output_control_t o(&s.p);
    for (;i != ie && j != je; ++i,++j) {
      o.put(std::make_pair(*i,*j));
    }
    return s;
  } 

  template<class T, class U>
  std::pair<slist<T>,slist<U>> unzip (slist<std::pair<T,U>> const &a) {
    using P = std::pair<T,U>;
    auto i = a.begin();
    auto ie = a.end();
    slist<T> ls;
    slist<U> rs;
    typename slist<T>::output_control_t l(&ls.p);
    typename slist<U>::output_control_t r(&rs.p);

    for (;i != ie ; ++i) {
      auto v = *i;
      l.put(v.first);
      r.put(v.second);
    }
    return std::make_pair(ls,rs);
  } 

  template<class T, class U, class F>
  U fold_left(F f, U init, slist<T> x) {
    return x.fold_left(init, f);
  }

}; // Slist

