#include "_test_common.h"

typedef struct _insert_acc_t {
  const LargestIntegralType *node_nums;
  size_t node_nums_cnt;
  int node_cnt;
}_insert_acc_t;

static void _tnode_access(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  _insert_acc_t* ta = (_insert_acc_t*)arg;

  assert_in_set(tnode->num, ta->node_nums, ta->node_nums_cnt);
  ta->node_cnt++;
}

void _test_rbtree_insert(const LargestIntegralType* node_nums, const int node_nums_cnt, const int assert_black_cnt) {
  int i;
  _insert_acc_t ia = {node_nums, node_nums_cnt, 0};
  struct rbtree_t* tree = NULL;

  tree = create_rbtree(_alloc_tnode, _free_tnode, _tnode_compare, _tvalue_compare);
  assert_true(NULL != tree);
  assert_true(sizeof(uintptr_t) <= sizeof(LargestIntegralType));
  for (i = 0; i < node_nums_cnt; i++) {
    assert_true(rbt_insert(tree, (void*)(int)node_nums[i]));
  }

  _pre_order_traversal(tree, _tnode_access, &ia);
  assert_int_equal(node_nums_cnt, ia.node_cnt);

  _assert_rbtree_rule(tree, assert_black_cnt);

  destroy_rbtree(tree);
}

void test_rbtree_ascending_insert(void** state) {
  const LargestIntegralType node_nums[] = {
    1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 18,
  };

  _test_rbtree_insert(node_nums, countof(node_nums), 3);
}

void test_rbtree_descending_insert(void** state) {
  const LargestIntegralType node_nums[] = {
    18, 17, 16, 15, 14, 13, 12, 11, 10,
    9, 8, 7, 6, 5, 4, 3, 2, 1,
  };

  _test_rbtree_insert(node_nums, countof(node_nums), 3);
}
