New Additions
---------------

* Tree Parallelism
* Fine-grained locks
* Partial Pathery client-side implementation


Overlapping (not my work)
-------------

* Monte-carlo tree search structure (classes/organization)
* Root Parallelism
* Test games (connect four, nim, etc)

New Perf Results (preliminary)
----------------

* Evaluating on first move of connect 4 on i7 (4 threads) (so too much work isn't that great):
  * 4 root, 1 tree/root --  500k/second (and 76% visit option 3)
  * 1 root, 4 trees/root -- 330k/second (and 90% visit option 3)
  * 4 roots, 4 trees/root -- 390k/second (and 86% visit option 3)