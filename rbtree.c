#include "rbtree.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// If unit testing is enabled override assert with mock_assert().
#ifdef UNIT_TESTING
#include <setjmp.h>
#include <stdarg.h>
#include <google/cmockery.h> //for redefine malloc/calloc/free

#undef assert
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__);
#endif // UNIT_TESTING

#define RBFLAG_COLOR_RED (1 << 0)

typedef struct rbtree_t {
  rbnode_compare_t compare;
  rbnode_free_t rb_free;
  rbnode_t* root;
}rbtree_t;

rbnode_t* _root(rbtree_t* tree) {
  assert(NULL != tree);
  return tree->root;
}

rbnode_t* _parent(rbnode_t* node) {
  assert(NULL != node);
  return node->_parent;
}

rbnode_t* _grandparent(rbnode_t* node) {
  assert(NULL != node);
  if (NULL == _parent(node)) {
    return NULL;
  }
  return _parent(_parent(node));
}

rbnode_t* _left_child(rbnode_t* node) {
  assert(NULL != node);
  return node->_left;
}

rbnode_t* _right_child(rbnode_t* node) {
  assert(NULL != node);
  return node->_right;
}

rbnode_t** _ref_root(rbtree_t* tree) {
  assert(NULL != tree);
  return &(tree->root);
}

rbnode_t** _ref_parent(rbnode_t* node) {
  assert(NULL != node);
  return &(node->_parent);
}

rbnode_t** _ref_left_child(rbnode_t* node) {
  assert(NULL != node);
  return &node->_left;
}

rbnode_t** _ref_right_child(rbnode_t* node) {
  assert(NULL != node);
  return &node->_right;
}

void _set_root(rbtree_t* tree, rbnode_t* node) {
  assert(NULL != tree);
  *_ref_root(tree) = node;
}

void _set_parent(rbnode_t* node, rbnode_t* parent) {
  assert(NULL != node);
  *_ref_parent(node) = parent;
}

void _set_left_child(rbnode_t* node, rbnode_t* child) {
  assert(NULL != node);
  *_ref_left_child(node) = child;
}

void _set_right_child(rbnode_t* node, rbnode_t* child) {
  assert(NULL != node);
  *_ref_right_child(node) = child;
}

bool _is_root(rbtree_t* tree, rbnode_t* node) {
  assert(NULL != tree);
  assert(NULL != node);

  return _root(tree) == node;
}

bool _is_left_child(rbnode_t* node) {
  assert(NULL != _parent(node));
  return _left_child(_parent(node)) == node;
}

bool _is_right_child(rbnode_t* node) {
  assert(NULL != _parent(node));
  return _right_child(_parent(node)) == node;
}

bool _is_leaf(rbnode_t* node) {
  assert(NULL != node);
  return NULL == _left_child(node) && NULL == _right_child(node);
}

rbnode_t* _uncle(rbnode_t* node) {
  assert(NULL != _parent(node));
  assert(NULL != _grandparent(node));

  if (_is_left_child(_parent(node))) {
    return _right_child(_grandparent(node));
  }
  else {
    return _left_child(_grandparent(node));
  }
}

bool _is_red(const rbnode_t* node) {
  if (NULL == node) {
    return false;
  }
  return RBFLAG_COLOR_RED == (node->_flags & RBFLAG_COLOR_RED);
}

bool _is_black(const rbnode_t* node) {
  return !_is_red(node);
}

void _set_red(rbnode_t* node) {
  node->_flags |= RBFLAG_COLOR_RED;
}

void _set_black(rbnode_t* node) {
  node->_flags &= ~RBFLAG_COLOR_RED;
}

bool _empty_tree(rbtree_t* tree) {
  assert(NULL != tree);
  return NULL == _root(tree);
}

int _child_count(rbnode_t* node) {
  int cnt = 0;
  assert(NULL != node);

  if (NULL != _left_child(node)) {
    cnt++;
  }
  if (NULL != _right_child(node)) {
    cnt++;
  }

  return cnt;
}

void _flips_color(rbnode_t* node1, rbnode_t* node2) {
  const bool node1_red = _is_red(node1);
  const bool node2_red = _is_red(node2);

  if (node2_red) {
    _set_red(node1);
  }
  else {
    _set_black(node1);
  }

  if (node1_red) {
    _set_red(node2);
  }
  else {
    _set_black(node2);
  }
}

/*
       parent          parent
        |                |
      node               x
     /  \              /  \
    y   x      =>   node   b
      /  \          /  \
     a    b        y   a
*/
void _rotate_left(rbtree_t* tree, rbnode_t* node) {
  rbnode_t* a;
  rbnode_t* x = _right_child(node);
  rbnode_t* parent = _parent(node);
  const bool is_root = _is_root(tree, node);

  assert(NULL != x);
  a = _left_child(x);

  /*must reset parent first.
  for if reset parent at last, you may not know whther node is left or right correctly.*/
  if (NULL != parent) {
    if (_is_left_child(node)) {
      _set_left_child(parent, x);
    }
    else {
      _set_right_child(parent, x);
    }
  }
  _set_parent(x, parent);

  _set_right_child(node, a);
  if (NULL != a) {
    _set_parent(a, node);
  }

  _set_left_child(x, node);
  _set_parent(node, x);

  if (is_root) {
    _set_root(tree, x);
  }

  _flips_color(node, x);
}

/*
      parent           parent
        |                |
      node               x
     /  \              /  \
    x   y      =>     b   node
  /  \                   /  \
 b    a                 a    y
*/
void _rotate_right(rbtree_t* tree, rbnode_t* node) {
  rbnode_t* a;
  rbnode_t* x = _left_child(node);
  rbnode_t* parent = _parent(node);
  const bool is_root = _is_root(tree, node);

  assert(NULL != x);
  a = _right_child(x);

  /*must reset parent first.
  for if reset parent at last, you may not know whther node is left or right correctly.*/
  if (NULL != parent) {
    if (_is_left_child(node)) {
      _set_left_child(parent, x);
    }
    else {
      _set_right_child(parent, x);
    }
  }
  _set_parent(x, parent);

  _set_left_child(node, a);
  if (NULL != a) {
    _set_parent(a, node);
  }

  _set_right_child(x, node);
  _set_parent(node, x);

  if (is_root) {
    _set_root(tree, x);
  }

  _flips_color(node, x);
}

void _rotate_to_same_side_with_parent(rbtree_t* tree, rbnode_t* son, rbnode_t** _new_son, rbnode_t** _new_parent) {
  rbnode_t* new_son = son;
  rbnode_t* new_parent = _parent(son);

  assert(NULL != son);
  assert(NULL != _parent(son));

  if (_is_left_child(_parent(son)) && _is_right_child(son)) {
    new_parent = son;
    new_son = _parent(son);
    _rotate_left(tree, _parent(son));
  }
  else if (_is_right_child(_parent(son)) && _is_left_child(son)) {
    new_parent = son;
    new_son = _parent(son);
    _rotate_right(tree, _parent(son));
  }

  if (_new_son) {
    *_new_son = new_son;
  }
  if (_new_parent) {
    *_new_parent = new_parent;
  }
}

void _rotate_grandparent(rbtree_t* tree, rbnode_t* node) {
  assert(NULL != node);
  assert(NULL != _parent(node));
  assert(NULL != _grandparent(node));

  if (_is_left_child(_parent(node))) {
    _rotate_right(tree, _grandparent(node));
  }
  else {
    _rotate_left(tree, _grandparent(node));
  }
}

rbnode_t* __get_tree_maximum(rbnode_t* root) {
  rbnode_t* node = root;

  assert(NULL != root);
  while(NULL != _right_child(node)) {
    node  = _right_child(node);
  }

  return node;
}

rbnode_t* __get_tree_minimum(rbnode_t* root) {
  rbnode_t* node = root;
  assert(NULL != root);
  while(NULL != _left_child(node)) {
    node = _left_child(node);
  }

  return node;
}

bool _insert_node(rbtree_t* tree, rbnode_t* new_node) {
  int cmp;
  rbnode_t** insert_pos = _ref_root(tree);
  rbnode_t* parent = NULL;

  while(true) {
    rbnode_t* node = *insert_pos;
    if (NULL == node)
      break;

    cmp = tree->compare(new_node, node);
    if (cmp < 0) {
      insert_pos = _ref_left_child(node);
    }
    else if (cmp > 0) {
      insert_pos = _ref_right_child(node);
    }
    else {
      return false;
    }

    parent = node;
  }

  assert(NULL != insert_pos);
  *insert_pos = new_node;
  _set_parent(new_node, parent);
  return true;
}

/*return NULL if node is a leaf. */
rbnode_t* __find_a_substitutes_node(rbtree_t* tree, rbnode_t* node) {
  assert(NULL != tree);
  assert(NULL != node);
  assert(!_empty_tree(tree))

  if (_is_leaf(node)) {
    return NULL;
  }

  if (NULL != _left_child(node)) {
    return __get_tree_maximum(_left_child(node));
  }
  else {
    return __get_tree_minimum(_right_child(node));
  }
}

void __substitutes_node_but_color(rbtree_t* tree, rbnode_t* node, rbnode_t* sub_node) {
  rbnode_t* parent = _parent(node);
  rbnode_t* left = _left_child(node);
  rbnode_t* right = _right_child(node);

  if (NULL != parent) {
    if (_is_left_child(node)) {
      _set_left_child(parent, sub_node);
    }
    else if (_is_right_child(node)) {
      _set_right_child(parent, sub_node);
    }
  }

  _set_parent(sub_node, parent);
  _set_left_child(sub_node, left);
  _set_right_child(sub_node, right);

  if (NULL != left) {
    _set_parent(left, sub_node);
  }
  if (NULL != right) {
    _set_parent(right, sub_node);
  }

  if (_is_root(tree, node)) {
    _set_root(tree, sub_node);
  }

  if (_is_red(node)) {
      _set_red(sub_node);
  }
  else {
      _set_black(sub_node);
  }
}

/*a nofull node means it has 1 child at most.
*/
void __pickoff_nonfull_node(rbtree_t* tree, rbnode_t* node) {
  rbnode_t* child, *parent;

  assert(_child_count(node) < 2);
  child = _left_child(node) ? _left_child(node) : _right_child(node);
  parent = _parent(node);

  if (_is_root(tree, node)) {
    _set_root(tree, child);
  }
  else if (_is_left_child(node)) {
    _set_left_child(parent, child);
  }
  else {
    assert(_is_right_child(node));
    _set_right_child(parent, child);
  }

  if (NULL != child) {
    _set_parent(child, parent);
  }
}

void _keep_rbtree_rule_for_remove(rbtree_t* tree, rbnode_t* start_node, rbnode_t* start_child, const bool remove_is_red) {
    rbnode_t *node, *child;
    rbnode_t *another_child, *left_grandchild, *right_grandchild;

    if (NULL == start_node) {
        return;
    }
    assert(start_child == _left_child(start_node) || start_child == _right_child(start_node));

    if (remove_is_red) {
        return;
    }
    if (_is_red(start_child)) {
        _set_black(start_child);
        return;
    }

    node = start_node;
    child = start_child;
    while(NULL != node) {
        another_child = child == _left_child(node) ? _right_child(node) : _left_child(node);

        if (NULL == another_child) {
            if (_is_red(node)) {
                _set_black(node);
                break;
            }
            else {
                child = node;
                node = _parent(node);
                continue;
            }
        }
        left_grandchild = _left_child(another_child);
        right_grandchild = _right_child(another_child);

        //node has left and right child from now on.

        if (_is_black(node) &&
            _is_black(another_child) &&
            _is_black(left_grandchild) &&
            _is_black(right_grandchild))
        {
            _set_red(another_child);
            child = node;
            node = _parent(node);
            continue;
        }

        if (_is_red(node) &&
            _is_black(another_child) &&
            _is_black(left_grandchild) &&
            _is_black(right_grandchild))
        {
            _flips_color(node, another_child);
            break;
        }

        if (_is_red(another_child))
        {
            if (_is_left_child(another_child)) {
                _rotate_right(tree, node);
            }
            else {
                _rotate_left(tree, node);
            }
            continue;
        }

        if (_is_left_child(another_child) && _is_red(left_grandchild)) {
            _rotate_right(tree, node);
            _set_black(left_grandchild);
            break;
        }
        if (_is_right_child(another_child) && _is_red(right_grandchild)) {
            _rotate_left(tree, node);
            _set_black(right_grandchild);
            break;
        }

        if (_is_left_child(another_child) && _is_red(right_grandchild)) {
            _rotate_left(tree, another_child);
            continue;
        }
        if (_is_right_child(another_child) && _is_red(left_grandchild)) {
            _rotate_right(tree, another_child);
            continue;
        }

        assert(!"all situation are handled, never reach here");
    }

}

bool _pickoff_node(rbtree_t* tree, rbnode_t* node) {
  rbnode_t *sub_node;
  rbnode_t *parent, *child;
  bool is_red;

  assert(NULL != tree);
  assert(NULL != node);
  assert(!_empty_tree(tree));

  if (_is_leaf(node)) {
      parent = _parent(node);
      is_red = _is_red(node);
      __pickoff_nonfull_node(tree, node);
      _keep_rbtree_rule_for_remove(tree, parent, NULL, is_red);
    return true;
  }

  sub_node = __find_a_substitutes_node(tree, node);
  /*node is not a leaf. so exch_node must not be NULL.*/
  assert(NULL != sub_node);
  /*a substitutes node has 1 child at most.*/
  assert(_child_count(sub_node) < 2);

  parent = _parent(sub_node);
  child = _left_child(sub_node) ? _left_child(sub_node) : _right_child(sub_node);
  is_red = _is_red(sub_node);
  __pickoff_nonfull_node(tree, sub_node);
  __substitutes_node_but_color(tree, node, sub_node);

  _keep_rbtree_rule_for_remove(tree, parent, child, is_red);

  return true;
}

bool _free_node(rbtree_t* tree, rbnode_t* node) {
  if (true != _pickoff_node(tree, node)) {
    return false;
  }
  tree->rb_free(node);

  return true;
}

bool _free_entire_tree(rbtree_t* tree) {
  rbnode_t* node;
  void* handle = rbt_begin_enumeration(tree);
  if (NULL == handle) {
    return false;
  }

  while((node = rbt_next_node(handle)) != NULL) {
      assert(NULL == _left_child(node) || NULL == _right_child(node));
      __pickoff_nonfull_node(tree, node);
      tree->rb_free(node);
  }

  rbt_end_enumeration(handle);

  return true;
}

void _keep_rbtree_rule_for_insert(rbtree_t* tree, rbnode_t* start_node) {
  rbnode_t* node = start_node;

  while(true) {
    /*the node is black, this will never break rbtree rules.*/
    if (_is_black(node)) {
      return;
    }

    /*1. the node is root */
    if (_is_root(tree, node)) {
      _set_black(node);
      return;
    }

    /*2. parent of the node is black */
    if (_is_black(_parent(node))) {
      return;
    }

    /*3. parent of the node is red. now we have two consecutive red node.*/

    /*since the node is red, the node must have a parent and a grandparent. */
    assert(NULL != _parent(node));
    assert(NULL != _grandparent(node));

    /*3.1 uncle is black */
    if (_is_black(_uncle(node))) {
      rbnode_t* son;
      _rotate_to_same_side_with_parent(tree, node, &son, NULL);
      _rotate_grandparent(tree, son);
      return;
    }

    /*3.2 uncle is red */
    assert(true == _is_red(_uncle(node)));
    assert(true == _is_black(_grandparent(node)));
    _set_black(_uncle(node));
    _set_black(_parent(node));
    _set_red(_grandparent(node));
    node = _grandparent(node); /*continue handling*/
  }
}

rbtree_t* create_rbtree(rbnode_compare_t compare, rbnode_free_t rb_free) {
  assert(NULL != compare);
  assert(NULL != rb_free);

  rbtree_t* tree = calloc(1, sizeof(rbtree_t));
  if (NULL == tree) {
    return NULL;
  }

  tree->compare = compare;
  tree->rb_free = rb_free;
  return tree;
}

void destroy_rbtree(rbtree_t* tree) {
  assert(tree != NULL);

  _free_entire_tree(tree);
  free(tree);
}

bool rbt_insert(rbtree_t* tree, rbnode_t* new_node) {
  _set_red(new_node);

  if (true != _insert_node(tree, new_node)) {
    return false;
  }

  _keep_rbtree_rule_for_insert(tree, new_node);
  return true;
}

bool rbt_remove(rbtree_t* tree, rbnode_t* node) {
  assert(!_empty_tree(tree));
  if (_empty_tree(tree)) {
    return false;
  }

  if (true != _free_node(tree, node)) {
    return false;
  }

  //TODO: 保证满足红黑树性质
  return true;
}

struct _rbtree_enum_t {
  rbnode_t* next;
  rbnode_t* current;
};

rbnode_t* _get_next_node(rbnode_t* node) {
  rbnode_t* parent;

  assert(NULL != node);
  if (_right_child(node) != NULL) {
    return __get_tree_minimum(_right_child(node));
  }

  parent = _parent(node);
  while(NULL != parent && _is_right_child(node)) {
    node = parent;
    parent = _parent(node);
  }

  return parent;
}

void* rbt_begin_enumeration(rbtree_t* tree) {
  struct _rbtree_enum_t* rbt_enum;

  assert(NULL != tree);
  rbt_enum = calloc(1, sizeof(struct _rbtree_enum_t));
  if (NULL == rbt_enum) {
    return NULL;
  }

  if (_empty_tree(tree)) {
    return rbt_enum;
  }

  rbt_enum->current = __get_tree_minimum(_root(tree));
  rbt_enum->next = _get_next_node(rbt_enum->current);

  return rbt_enum;
}

rbnode_t* rbt_next_node(void* enum_arg) {
  struct _rbtree_enum_t* rbt_enum = (struct _rbtree_enum_t*)enum_arg;
  rbnode_t* result = rbt_enum->current;

  if (NULL == result) {
    return NULL;
  }

  rbt_enum->current = rbt_enum->next;
  if (NULL != rbt_enum->next) {
    rbt_enum->next = _get_next_node(rbt_enum->next);
  }

  return result;
}

void rbt_end_enumeration(void* enum_arg) {
  assert(NULL != enum_arg);
  free(enum_arg);
}

bool rbt_foreach_safe(rbtree_t* tree, bool (*accessor)(const rbnode_t* node, void* arg), void* arg)
{
  const rbnode_t* node;
  struct _rbtree_enum_t* rbt_enum = rbt_begin_enumeration(tree);
  if (NULL == rbt_enum) {
    return false;
  }

  while((node = rbt_next_node(rbt_enum)) != NULL) {
    if (false == accessor(node, arg)) {
      break;
    }
  }

  rbt_end_enumeration(rbt_enum);

  return true;
}
