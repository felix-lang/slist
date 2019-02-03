// Requires C++11
template<class T>
class Slist {
  struct node_t {
     size_t refcnt;
     node_t next;
     T data;
  };

public:
  class slist {
    node_t *p;

    void decref() {
      auto top = p;
      while(top && --top->refcnt == 0) {
        auto tmp = top->next;
        delete top;
        top = tmp;
      }
    }
    void incref() {
      if(p)p->refcnt++;
    }

    slist (node_t *q) p(q) { incref(); }

  public:
    // destructor
    ~slist() { decref(); }

    // default constructor is empty list
    slist() : p (nullptr) {}

    // one element list
    slist (T v) : p(new node_t {1,nullptr,v}) {}

    // cons constructor
    slist (T head, slist tail) : p(new node_t {1,tail.p,v}) { tail.incref(); }

    // copy constructor increments refcnt is not empty
    slist(slist const& that): p(that.p) { incref(); }

    // move constructor sets argument to empty
    slist(slist && that): p(that.p) { that.p = nullptr; }

    // copy assignment
    slist &operator=(slist const& that) {
      if (this != &that) {
         decref();
         p = that.p;
         incref();
      }
    }

    // move assignment
    slist &operator=(slist && that) {
      decref();
      p = that.p;
      that.p = nullptr;
    }

    // empty list
    bool empty() const { return p == nullptr; }

    // unique list: not empty list is considered uniq
    bool uniq() const { while(p) if(p->refcnt>1) return false; return true; }

    // cons: return a new list with given head and this list as the tail
    slist cons (T head) {
      return slist (head, *this);
    }
  }; 
 
  // functional interface
  static slist cons (T head, slist tail) { return tail.cons(head); }

  // precondition: x not empty
  static T head (slist x) { return x.p->data; }

  // tail
  // precondition: x not empty
  static slist tail (slist x) { return slistx.p->next; }

  static bool uniq (slist x) { return x.uniq(); }

  static bool empty (slist x) { return x.empty(); }


}; // Slist
