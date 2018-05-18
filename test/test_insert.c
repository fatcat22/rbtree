#include "_test_common.h"

typedef struct _insert_acc_t {
  const LargestIntegralType *nodes_set;
  size_t nodes_set_cnt;
  int node_cnt;
}_insert_acc_t;

static void _tnode_access(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  _insert_acc_t* ta = (_insert_acc_t*)arg;

  assert_in_set((LargestIntegralType)(uintptr_t)tnode, ta->nodes_set, ta->nodes_set_cnt);
  ta->node_cnt++;
}

void _test_rbtree_insert(const LargestIntegralType* tnodes, const int tnodes_cnt, const int assert_black_cnt) {
  int i;
  _insert_acc_t ia = {tnodes, tnodes_cnt, 0};
  struct rbtree_t* tree = NULL;

  tree = create_rbtree(_tnode_compare, _free_tnode);
  assert_true(NULL != tree);
  assert_true(sizeof(uintptr_t) <= sizeof(LargestIntegralType));
  for (i = 0; i < tnodes_cnt; i++) {
    assert_true(rbt_insert(tree, &(((struct tnode_t*)(uintptr_t)tnodes[i])->node)));
  }

  _pre_order_traversal(tree, _tnode_access, &ia);
  assert_int_equal(tnodes_cnt, ia.node_cnt);

  _assert_rbtree_rule(tree, assert_black_cnt);

  destroy_rbtree(tree);
}

void test_rbtree_ascending_insert(void** state) {
  const LargestIntegralType tnodes[] = {
    (LargestIntegralType)(uintptr_t)_create_tnode(1),
    (LargestIntegralType)(uintptr_t)_create_tnode(2),
    (LargestIntegralType)(uintptr_t)_create_tnode(3),
    (LargestIntegralType)(uintptr_t)_create_tnode(4),
    (LargestIntegralType)(uintptr_t)_create_tnode(5),
    (LargestIntegralType)(uintptr_t)_create_tnode(6),
    (LargestIntegralType)(uintptr_t)_create_tnode(7),
    (LargestIntegralType)(uintptr_t)_create_tnode(8),
    (LargestIntegralType)(uintptr_t)_create_tnode(9),
    (LargestIntegralType)(uintptr_t)_create_tnode(10),
    (LargestIntegralType)(uintptr_t)_create_tnode(11),
    (LargestIntegralType)(uintptr_t)_create_tnode(12),
    (LargestIntegralType)(uintptr_t)_create_tnode(13),
    (LargestIntegralType)(uintptr_t)_create_tnode(14),
    (LargestIntegralType)(uintptr_t)_create_tnode(15),
    (LargestIntegralType)(uintptr_t)_create_tnode(16),
    (LargestIntegralType)(uintptr_t)_create_tnode(17),
    (LargestIntegralType)(uintptr_t)_create_tnode(18),
  };
  const int node_cnt = sizeof(tnodes)/sizeof(tnodes[0]);

  _test_rbtree_insert(tnodes, node_cnt, 3);
}

void test_rbtree_descending_insert(void** state) {
  const LargestIntegralType tnodes[] = {
    (LargestIntegralType)(uintptr_t)_create_tnode(18),
    (LargestIntegralType)(uintptr_t)_create_tnode(17),
    (LargestIntegralType)(uintptr_t)_create_tnode(16),
    (LargestIntegralType)(uintptr_t)_create_tnode(15),
    (LargestIntegralType)(uintptr_t)_create_tnode(14),
    (LargestIntegralType)(uintptr_t)_create_tnode(13),
    (LargestIntegralType)(uintptr_t)_create_tnode(12),
    (LargestIntegralType)(uintptr_t)_create_tnode(11),
    (LargestIntegralType)(uintptr_t)_create_tnode(10),
    (LargestIntegralType)(uintptr_t)_create_tnode(9),
    (LargestIntegralType)(uintptr_t)_create_tnode(8),
    (LargestIntegralType)(uintptr_t)_create_tnode(7),
    (LargestIntegralType)(uintptr_t)_create_tnode(6),
    (LargestIntegralType)(uintptr_t)_create_tnode(5),
    (LargestIntegralType)(uintptr_t)_create_tnode(4),
    (LargestIntegralType)(uintptr_t)_create_tnode(3),
    (LargestIntegralType)(uintptr_t)_create_tnode(2),
    (LargestIntegralType)(uintptr_t)_create_tnode(1),
  };
  const int node_cnt = sizeof(tnodes)/sizeof(tnodes[0]);

  _test_rbtree_insert(tnodes, node_cnt, 3);
}
