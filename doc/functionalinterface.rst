Functional Interface
====================

empty
-----

.. code-block:: c++

  empty(x)

Returns true if the list x is empty, false otherwise.


Default Constructor
-------------------

.. code-block:: c++

  slist<T>()


Is an empty list to which can be added elements of type T.

singleton
---------

.. code-block:: c++

  singleton(v)

Returns a unique list of one element v.

slist_from_container
--------------------

.. code-block:: c++

  slist_from_container (C)

Requires C be an STL container with a begin() method returning
an input iterator and an end() method returning an end iterator.
Returns a list of all the elements in the container in the sequence
found from the iterator.

slist_from_iterator_range
-------------------------

.. code-block:: c++

  slist_from_iterator (begin,end)

Requires begin, end be valid iterators for a sequence.
Returns all the values in the range of the begin iterator
up to but excluding the end iterator.

size
----

.. code-block:: c++

 size(x)

Returns the length of the list x.

begin
-----

.. code-block:: c++

 begin(x)

Returns copy of the list handle.

end
---

.. code-block:: c++

 end(x)

Returns empty list.

operator ==
-----------

.. code-block:: c++

  x == y

Requires x or y be an empty list or both must be
handles to the same list. Returns true if x and y are
both empty or both denote the same node of a list.

If x and y are non-empty, equality implies the x and
y are the same length, inequaly implies either the
lists have distinct lengths or are not the same list.

Note that this function will return false, even if the
contents of two list are equal. It is designed to be
used to support the use of list handles as iterators.

Another way to define this function is that it has a precondition
that x and y must be the same list, or, one must be a sublist
of the other. Given this pre-condition, the operator returns
true if an only if the lists are the same length.

Operationally, this function just compares the pointers
stored in the list handles.


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
  *x


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

If x is unique, returns x otherwise returns a unique copy of x.

map
---

.. code-block:: c++

  map (f,x)

Returns a unique list with elements the result of applying
f to each element of x.

fold_left
---------

.. code-block:: c++

  fold_left (f,init,x)

Uses f to fold each value of x starting at the front into init.
Returns final result. f must accept two arguments,
the first of type U, the type of init, and the second
of type T, the type of the elements of x.

zip
---

.. code-block:: c++

  zip(x,y)

Precondition, x and y have the same length. Returns a list of
pairs of corresponding element from x and y.

.. code-block:: c++

  unzip(x,y)







