# Red-Black tree
This is an implementation of a self-balancing binary search tree. The tree is using a
generic element type. This implementation is focused on performance instead of an elegant
solution that would be easier to understand. Note that all operations are in-place destructive
and use no recursion (except for some debugging functions).

## Performance
Red–black trees guarantee search, insertion and deletion in logarithmic time. This can especially be
useful when an application has to provide worst-case guarantees, which can not be made by a lot of other
data structures. For *n* as total number of elements in the tree:

| Operation |   Average  | Worst case |
|-----------|:----------:|:----------:|
|  Space    |   O(*n*)   |   O(*n*)   |
|  Search   | O(log *n*) | O(log *n*) |
|  Insert   | O(log *n*) | O(log *n*) |
|  Delete   | O(log *n*) | O(log *n*) |

## Memory
An element will be stored in a node which means the number of nodes is equivalent to the number of elements.
Only during a delete operation, there may a double black null node which holds no element. At the moment this
implementation is using a nested class to represent a node. This is nice for encapsulation and separating
logic between a node and a tree. The downside is that the code of a node cannot change the root pointer of
the tree. To solve this issue each node contains a pointer to the tree. For further memory optimisations, it is
possible to get rid of this pointer by breaking the encapsulation of the nested node class.

## Generic elements
In order to use any type and provide type safety at the same time, the implementation is using a template. For some
applications, it makes sense to store a key-value pair in each node. This tree only stores one type, however modifying
it in a way to store a pair should not be very difficult. The functions are implemented in the header file because
a template is used. Another solution would be to keep the implementation separated and explicitly instantiate all
the template types that are needed. The second solution should be preferred for large projects.

## Visualization
The tree can be visualized with the dump function. The dump will generate a graph and png file with the `Graphviz`-Tool.
This can be useful for a better understanding of the data structure and for debugging purposes. Example of the tree visualization:
![Tree-Dump-Example](https://raw.githubusercontent.com/wiki/Henrik-Peters/Red-Black-Tree/images/tree-example.png)
