#include "_test_common.h"
#include "string.h"

typedef struct _enum_test_t {
  int* sorted_values;
  size_t sorted_values_cnt;
  struct rbtree_t* test_tree;
}_enum_test_t;

void setup_enumerate_test_rbtree(void** state) {
  /* create a tree like this:
                            7
                          /   \
                         3     9
                        /\     \
                      2  5      10
                    /   /\
                  1    4  6
  */
  const LargestIntegralType node_nums[] = {
    7,
    3,
    9,
    2,
    5,
    //8,
    10,
    1,
    4,
    6,
    //11,
  };
  const int sorted_values[] = {1, 2, 3, 4, 5, 6, 7, 9, 10};
  _enum_test_t* enum_test;
  int i;

  enum_test = calloc(1, sizeof(*enum_test));
  assert_true(NULL != enum_test);

  enum_test->test_tree = create_rbtree(_alloc_tnode, _free_tnode, _tnode_compare, _tvalue_compare);
  assert_true(NULL != enum_test->test_tree);
  for (i = 0; i < countof(node_nums); i++) {
    assert_true(rbt_insert(enum_test->test_tree, (void*)(int)node_nums[i]));
  }

  enum_test->sorted_values = calloc(1, sizeof(sorted_values));
  assert_true(NULL != enum_test->sorted_values);
  memcpy(enum_test->sorted_values, sorted_values, sizeof(sorted_values));
  enum_test->sorted_values_cnt = countof(sorted_values);

  *state = enum_test;
}

void teardown_enumerate_test_rbtree(void** state) {
  _enum_test_t* enum_test = (_enum_test_t*)*state;

  if (NULL == enum_test) {
    return;
  }

  if (NULL != enum_test->sorted_values) {
    free(enum_test->sorted_values);
  }
  if (NULL != enum_test->test_tree) {
    destroy_rbtree(enum_test->test_tree);
  }

  free(enum_test);
}

void _test_enumeration_del(_enum_test_t* enum_test, const int* rm_nums, const size_t rm_nums_cnt) {
  int i;
  const rbnode_t* node;
  void* enum_h = rbt_begin_enumeration(enum_test->test_tree);

  assert_true(NULL != enum_h);
  for (i = 0, node = rbt_next_node(enum_h);
       node != NULL && i < enum_test->sorted_values_cnt;
       node = rbt_next_node(enum_h), i++)
  {
    const tnode_t* tnode = container_of(node, const tnode_t, node);
    assert_int_equal(enum_test->sorted_values[i], tnode->num);

    if (_is_in_set(tnode->num, rm_nums, rm_nums_cnt)) {
      rbt_remove(enum_test->test_tree, (void*)tnode->num);
    }
  }
  assert_int_equal(enum_test->sorted_values_cnt, i);

  rbt_end_enumeration(enum_h);
}

void _test_enumeration(_enum_test_t* enum_test) {
  _test_enumeration_del(enum_test, NULL, 0);
}

void _rm_from_sorted_nums(_enum_test_t* enum_test, const int* rm_nums, const size_t rm_nums_cnt) {
  int i, j, rm_index;

  for (i = 0; i < rm_nums_cnt; i++) {
    const int rm_num = rm_nums[i];
    assert_true(_is_in_set(rm_num, enum_test->sorted_values, enum_test->sorted_values_cnt));

    for (rm_index = 0; rm_index < enum_test->sorted_values_cnt; rm_index++) {
      if (rm_num == enum_test->sorted_values[rm_index]) {
        break;
      }
    }
    assert_true(rm_index < enum_test->sorted_values_cnt);

    //do remove
    for (j = rm_index; j < enum_test->sorted_values_cnt - 1; j++) {
      enum_test->sorted_values[j] = enum_test->sorted_values[j+1];
    }
    enum_test->sorted_values_cnt--;
  }
}

void _test_rbtree_enumeration_safe(_enum_test_t* enum_test, const int* rm_nums, const size_t rm_nums_cnt) {
  int i;

  for (i = 0; i < rm_nums_cnt; i++) {
    assert_true(_is_in_set(rm_nums[i], enum_test->sorted_values, enum_test->sorted_values_cnt));
  }

  _test_enumeration_del(enum_test, rm_nums, rm_nums_cnt);
  _rm_from_sorted_nums(enum_test, rm_nums, rm_nums_cnt);
  _test_enumeration(enum_test);
}

void test_rbtree_enumeration(void** state) {
  _test_enumeration((_enum_test_t*)*state);
}

void test_rbtree_enumeration_safe_del_first(void** state) {
  const int rm_nums[] = {1};

  _test_rbtree_enumeration_safe((_enum_test_t*)*state, rm_nums, countof(rm_nums));
}

void test_rbtree_enumeration_safe_del_last(void** state) {
  const int rm_nums[] = {10};

  _test_rbtree_enumeration_safe((_enum_test_t*)*state, rm_nums, countof(rm_nums));
}

void test_rbtree_enumeration_safe_del_root(void** state) {
  const int rm_nums[] = {7};

  _test_rbtree_enumeration_safe((_enum_test_t*)*state, rm_nums, countof(rm_nums));
}

void test_rbtree_enumeration_safe_del_two_leaf(void** state) {
  const int rm_nums[] = {4, 10};

  _test_rbtree_enumeration_safe((_enum_test_t*)*state, rm_nums, countof(rm_nums));
}

void test_rbtree_enumeration_safe_del_three_fullnode(void** state) {
  int rm_nums[] = {3, 5, 9};

  _test_rbtree_enumeration_safe((_enum_test_t*)*state, rm_nums, countof(rm_nums));
}
