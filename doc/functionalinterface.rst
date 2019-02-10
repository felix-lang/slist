Interface
=========

empty
-----

.. code-block:: c++

  empty(x)
  x.empty()

Returns true if the list x is empty, false otherwise.


Default Constructor
-------------------

.. code-block:: c++

  slist<T>()


Is an empty list to which can be added elements of type T.

singleton
---------

.. code-block:: c++

  slist(v)

Returns a unique list of one element v.

slist_from_container
--------------------

.. code-block:: c++

  slist_from_container<T> (C)

Requires C be an STL container with a begin() method returning
an input iterator and an end() method returning an end iterator.
Returns a list of all the elements in the container in the sequence
found from the iterator.

slist_from_iterators
--------------------

.. code-block:: c++

  slist_from_iterators<T> (begin,end)

Requires begin, end be valid iterators for a sequence.
Returns all the values in the range of the begin iterator
up to but excluding the end iterator.

size
----

.. code-block:: c++

 size(x)
 x.size()

Returns the length of the list x.

uniq
----

.. code-block:: c++

 uniq(x)

Returns true if x s empty or is the only reference to the underlying list
and all tails thereof. Implies the reference counts of all nodes
of the underlying list are 1.

cons
----

.. code-block:: c++

  cons (h,t)

returns list t with value h added to front. Unique if and only if t is unique.

head
----

.. code-block:: c++

  head (x)


Precondition non-empty list. Returns first value on the list.

tail
----

.. code-block:: c++

  tail (x)

Precondition non-empty list. Returns list with first value removed.
Unique if x is unique, may be unique even if x is not.


join
----

.. code-block:: c++

  join (x,y)
  x + y

Returns the list which is the concatenation of lists x and y.
Unique if y is unique.

rev
---

.. code-block:: c++

  rev (x)

Returns the list reversed. always unique.

copy
----

.. code-block:: c++

  copy (x)

Makes a copy of the list. Always unique.

make_unique
-----------

Returns the list if it is unique, or a copy otherwise.
Result is always unique.


map
---

.. code-block:: c++

  map<U> (f,x)

Returns a list with elements of type U, the result of applying
f to each element of x. Always unique. Cost N allocations.

filter
------

.. code-block:: c++

  filter (f,x)

Returns a sublist of elements of x satifying predicate f(v).
Always unique.

fold_left
---------

.. code-block:: c++

  fold_left (f,init,x)

TODO.
Uses f to fold each value of x starting at the front into init.
Returns final result. f must accept two arguments,
the first of type U, the type of init, and the second
of type T, the type of the elements of x.

zip
---

.. code-block:: c++

  zip(x,y)

TODO.
Precondition, x and y have the same length. Returns a list of
std::pair of corresponding element from x and y.

unzip
-----

.. code-block:: c++

  unzip(x)

TODO.
Splits a list of pairs into a pair of lists. Precondition, the
value type of x must be a std::pair.

begin
-----

.. code-block:: c++

 begin(x)
 x.begin()

Returns list iterator starting at head of list.

 
end
---

.. code-block:: c++

 end(x)
 x.end()

Returns terminal list iterator.



Iterator Interface
==================

operator ==
-----------

.. code-block:: c++

  p == q

operator !=
-----------

.. code-block:: c++

  x == y

operator ++
-----------

.. code-block:: c++

   ++p
 
Preincrement iterator. Precondition not at end.

operator ++(int)
----------------

.. code-block:: c++

   p++
 
Postincrement iterator. Precondition not at end.

operator *
----------

.. code-block:: c++

  *p

Fetch value.  Precondition not at end.

