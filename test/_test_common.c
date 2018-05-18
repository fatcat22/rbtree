#include "_test_common.h"

struct tnode_t* _create_tnode(int num) {
  struct tnode_t* tn = malloc(sizeof(struct tnode_t));
  if (NULL == tn)
    return NULL;

  tn->num = num;
  tn->node._left = NULL;
  tn->node._right = NULL;
  tn->node._parent = NULL;
  return tn;
}

void _free_tnode(rbnode_t* node) {
  assert(node);
  free(container_of(node, struct tnode_t, node));
}

int _tnode_compare(const rbnode_t* n1, const rbnode_t* n2) {
  const struct tnode_t* tn1 = container_of(n1, struct tnode_t, node);
  const struct tnode_t* tn2 = container_of(n2, struct tnode_t, node);

  if (tn1->num < tn2->num) {
    return -1;
  }
  else if (tn1->num == tn2->num) {
    return 0;
  }
  else {
    return 1;
  }
}

int _assert_all_nodes(rbnode_t* root, const LargestIntegralType tnodes[], size_t tnodes_cnt) {
  const struct tnode_t* tn = container_of(root, struct tnode_t, node);

  assert_true(sizeof(root) <= sizeof(LargestIntegralType));
  if (NULL == root)
    return 0;

  assert_in_set((LargestIntegralType)(uintptr_t)tn, tnodes, tnodes_cnt);

  return 1 + _assert_all_nodes(root->_left, tnodes, tnodes_cnt) + _assert_all_nodes(root->_right, tnodes, tnodes_cnt);
}

static void __pre_order_traversal(rbnode_t* root, void(*accessor)(tnode_t* tnode,  int red_cnt, int black_cnt, void* arg), void* arg, int red_cnt, int black_cnt) {
  if (NULL == root) {
    return;
  }

  if (_is_red(root)) {
    red_cnt++;
  }
  else {
    black_cnt++;
  }

  accessor(container_of(root, tnode_t, node), red_cnt, black_cnt, arg);
  __pre_order_traversal(_left_child(root), accessor, arg, red_cnt, black_cnt);
  __pre_order_traversal(_right_child(root), accessor, arg, red_cnt, black_cnt);
}

void _pre_order_traversal(struct rbtree_t* tree, void(*accessor)(tnode_t* tnode,  int red_cnt, int black_cnt, void* arg), void* arg) {
  __pre_order_traversal(_root(tree), accessor, arg, 0, 0);
}

static void __in_order_traversal(rbnode_t* root, void(*accessor)(tnode_t* tnode,  int red_cnt, int black_cnt, void* arg), void* arg, int red_cnt, int black_cnt) {
  if (NULL == root) {
    return;
  }

  __in_order_traversal(_left_child(root), accessor, arg, red_cnt, black_cnt);

  if (_is_red(root)) {
    red_cnt++;
  }
  else {
    black_cnt++;
  }
  accessor(container_of(root, tnode_t, node), red_cnt, black_cnt, arg);

  __in_order_traversal(_right_child(root), accessor, arg, red_cnt, black_cnt);
}

void _in_order_traversal(struct rbtree_t* tree, void(*accessor)(tnode_t* tnode, int red_cnt, int black_cnt, void* arg), void* arg) {
  __in_order_traversal(_root(tree), accessor, arg, 0, 0);
}

bool _is_in_set(const int val, const int* sets, const size_t sets_cnt) {
  size_t i;
  for (i = 0; i < sets_cnt; ++i) {
    if (val == sets[i]) {
      return true;
    }
  }

  return false;
}

typedef struct _assert_rbrule_t{
  struct rbtree_t* tree;
  int assert_black_cnt;
}_assert_rbrule_t;

static void
__assert_rbtree_rule_access(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  _assert_rbrule_t* assert_rbrule = (_assert_rbrule_t*)arg;
  int* assert_black_cnt = &assert_rbrule->assert_black_cnt;
  rbnode_t* node = &tnode->node;

  if (_is_root(assert_rbrule->tree, node)) {
    assert_true(_is_black(node));
  }
  if (_is_red(node)) {
    assert_true(_is_black(_parent(node)));
    assert_true(_is_black(_left_child(node)));
    assert_true(_is_black(_right_child(node)));
  }

  if (_is_leaf(node)) {
    assert_true(black_cnt >= red_cnt);

    if (-1 == *assert_black_cnt) {
      *assert_black_cnt = black_cnt;
    }
    else {
      assert_int_equal(*assert_black_cnt, black_cnt);
    }
  }
}

void _assert_rbtree_rule(struct rbtree_t* tree, int black_cnt) {
  _assert_rbrule_t assert_rbrule = {tree, black_cnt};
  _pre_order_traversal(tree, __assert_rbtree_rule_access, &assert_rbrule);
}
