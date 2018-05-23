#include "_test_common.h"
#include <string.h>

/*
case 1: removing a red node

case 2: remoing a black node but has a red child.

case 3: removing a black node and has a black child.
case 31: parent, sibling, sibling's left child, sibling's right child are all black.
                B
              /   \
            node   B
                 /  \
                B   B

case 32: parent is red, sibling, sibling's left child, sibling's right child are all black.
                R
              /   \
            node   B
                 /  \
                B   B

case 33: sibling is red. (so parent, sibling's left child, sibling's right child are and only can be black)
                B
              /   \
            node   R
                 /  \
                B   B

case 34: sibling's right child is red. (so sibling is black)
                X
              /   \
            node   B
                 /  \
                X   R

case 35: sibling's right child is black, but sibling's left child is red. (so sibling is black)
                X
              /   \
            node   B
                 /  \
                R   B
*/

typedef struct _node_info_t{
  tnode_t* node;
  int left;
  int right;
  int parent;
  bool is_red;
}_node_info_t;

typedef struct _tnode_array_t {
  LargestIntegralType* nodes; //array of tnode_t's pointer.
  int nodes_cnt;
}_tnode_array_t;

static
rbnode_t* _get_node_by_num(const _node_info_t* node_info, const int node_info_cnt, const int num) {
  int i;

  if (-1 == num) {
    return NULL;
  }

  for (i = 0; i < node_info_cnt; i++) {
    if (num == node_info[i].node->num) {
      return &node_info[i].node->node;
    }
  }

  assert_false("can not find node by num");
  return NULL;
}

static
struct rbtree_t* _make_tree(const _node_info_t* node_info, const int node_info_cnt) {
  int i;
  rbnode_t* root = NULL;
  struct rbtree_t* tree;

  for (i = 0; i < node_info_cnt; i++) {
    rbnode_t* node = &node_info[i].node->node;
    rbnode_t* left = _get_node_by_num(node_info, node_info_cnt, node_info[i].left);
    rbnode_t* right = _get_node_by_num(node_info, node_info_cnt, node_info[i].right);
    rbnode_t* parent = _get_node_by_num(node_info, node_info_cnt, node_info[i].parent);

    if (NULL == parent) {
      assert(NULL == root);
      root = node;
    }

    _set_left_child(node, left);
    _set_right_child(node, right);
    _set_parent(node, parent);

    if (node_info[i].is_red) {
      _set_red(node);
    }
    else {
      _set_black(node);
    }
  }

  assert_true(NULL != root);
  tree = create_rbtree(_alloc_tnode, _free_tnode, _tnode_compare, _tvalue_compare);
  assert_true(NULL != tree);

  _set_root(tree, root);
  return tree;
}

static _tnode_array_t
_make_expect_remaining(const _node_info_t* node_infos, const int node_infos_cnt, const int* rm_nums, const int rm_nums_cnt) {
  int i, remain_index;
  const int remaining_cnt = node_infos_cnt - rm_nums_cnt;
  _tnode_array_t tnode_arr = {NULL, 0};

  assert(node_infos_cnt >= rm_nums_cnt);
  if (0 == remaining_cnt) {
    return tnode_arr;
  }
  tnode_arr.nodes = calloc(1, sizeof(*(tnode_arr.nodes)) * remaining_cnt);
  tnode_arr.nodes_cnt = remaining_cnt;

  remain_index = 0;
  for (i = 0; i < node_infos_cnt; i++) {
    if (_is_in_set(node_infos[i].node->num, rm_nums, rm_nums_cnt)) {
      continue;
    }

    tnode_arr.nodes[remain_index] = (LargestIntegralType)(uintptr_t)node_infos[i].node;
    remain_index++;
  }

  return tnode_arr;
}

static void*
_realloc(void* ptr, size_t oldsize, size_t newsize) {
  void* res = calloc(1, newsize);

  if (NULL == ptr) {
    return res;
  }

  memcpy(res, ptr, oldsize);
  free(ptr);
  return res;
}

static void
__get_remain_traversal(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  _tnode_array_t* remain_arr = (_tnode_array_t*)arg;
  const size_t oldsize = sizeof(*(remain_arr->nodes)) * remain_arr->nodes_cnt;
  const size_t newsize = oldsize + sizeof(remain_arr->nodes[0]);

  /*can not use realloc here.
    we have define UNIT_TESTING so free is redefind test_free,
    but realloc is not redefined, this will make crash. */
  remain_arr->nodes = _realloc(remain_arr->nodes, oldsize, newsize);
  assert(NULL != remain_arr->nodes);
  remain_arr->nodes[remain_arr->nodes_cnt] = (LargestIntegralType)(uintptr_t)tnode;
  remain_arr->nodes_cnt++;
}

static _tnode_array_t
_get_remaining(struct rbtree_t* tree) {
  _tnode_array_t tnode_arr = {NULL, 0};

_in_order_traversal(tree, __get_remain_traversal, &tnode_arr);
return tnode_arr;
}

static bool
__find_node(LargestIntegralType node, const _tnode_array_t* remain) {
  int i;

  for (i = 0; i < remain->nodes_cnt; i++) {
    if (node == remain->nodes[i]) {
      return true;
    }
  }

  return false;
}

static void
_assert_remaining(const _tnode_array_t* expect_remain, const _tnode_array_t* remain) {
  int i;
  assert_int_equal(expect_remain->nodes_cnt, remain->nodes_cnt);

  for (i = 0; i < expect_remain->nodes_cnt; i++) {
    assert_true(__find_node(expect_remain->nodes[i], remain));
  }
}

static void
__assert_order_traversal(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  int* num = (int*)arg;

  assert_true(*num < tnode->num);
  *num = tnode->num;
}

static void
_assert_orderliness(struct rbtree_t* tree) {
  int num = 0;
  _in_order_traversal(tree, __assert_order_traversal, &num);
}

static
void _test_rbtree_remove(const _node_info_t* node_infos, const int node_infos_cnt, const int* rm_nodes, const int rm_nodes_cnt, const int black_cnt_after_remove) {
  int i;
  _tnode_array_t remaining;
  _tnode_array_t expect_remaining = _make_expect_remaining(node_infos, node_infos_cnt, rm_nodes, rm_nodes_cnt);
  struct rbtree_t* test_tree = _make_tree(node_infos, node_infos_cnt);

  for (i = 0; i < rm_nodes_cnt; i++) {
    rbnode_t* rm_node = _get_node_by_num(node_infos, node_infos_cnt, rm_nodes[i]);
    rbt_remove2(test_tree, rm_node);
  }
  remaining = _get_remaining(test_tree);

  _assert_remaining(&expect_remaining, &remaining);
  _assert_orderliness(test_tree);
  _assert_rbtree_rule(test_tree, black_cnt_after_remove);

  if (remaining.nodes) {
    free(remaining.nodes);
  }
  if (expect_remaining.nodes) {
    free(expect_remaining.nodes);
  }
  destroy_rbtree(test_tree);
}

void test_rbtree_remove_left_red_leaf(void** state) {
  /*           4B
            /     \
           2B     6R
          / \    /
         1R 3R  5B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 6, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, -1, 4, true},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_leaf__case31(void** state) {
  /*           4B
            /     \
           2B     5R
          / \      \
         1B 3B     6B
                    \
                    7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 5, -1, false},
    {_create_tnode(5), -1, 6, 4, true},
    {_create_tnode(6), -1, 7, 5, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_leaf__case32(void** state) {
  /*           4B
            /     \
           2R     5R
          / \      \
         1B 3B     6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, true},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 5, -1, false},
    {_create_tnode(5), -1, 6, 4, true},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_leaf__case33(void** state) {
  /*           6B
            /     \
           2B     7B
          /  \     \
         1B  4R    8B
             /\
            3B 5B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 4, 6, false},
    {_create_tnode(3), -1, -1, 4, false},
    {_create_tnode(4), 3, 5, 2, true},
    {_create_tnode(5), -1, -1, 4, false},
    {_create_tnode(6), 2, 7, -1, false},
    {_create_tnode(7), -1, 8, 6, false},
    {_create_tnode(8), -1, -1, 7, false},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_leaf__case34(void** state) {
  /*           5B
            /     \
           2B     6B
          /  \     \
         1B  3B    7B
              \
              4R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 5, false},
    {_create_tnode(3), -1, 4, 2, false},
    {_create_tnode(4), -1, -1, 3, true},
    {_create_tnode(5), 2, 6, -1, false},
    {_create_tnode(6), -1, 7, 5, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_leaf(void** state) {
  /*           4B
            /     \
           2B     6R
          / \    /
         1R 3R  5B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 6, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, -1, 4, true},
  };
  const int rm_nodes[] = {3};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_leaf__case31(void** state) {
  /*           4B
            /     \
          3R      6B
         /       /  \
        2B      5B  7B
       /
      1B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, true},
    {_create_tnode(4), 3, 6, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_nodes[] = {7};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_leaf__case32(void** state) {
  /*           3B
            /     \
          2R      5R
         /       /  \
        1B      4B  6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, true},
    {_create_tnode(3), 2, 5, -1, false},
    {_create_tnode(4), -1, -1, 5, false},
    {_create_tnode(5), 4, 6, 3, true},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_nodes[] = {6};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_leaf__case33(void** state) {
  /*           4B
            /     \
          3B      8B
         /       /  \
        2B      6R  9B
               / \
              5B  7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 8, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 8, true},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 9, 4, false},
    {_create_tnode(9), -1, -1, 8, false},
  };
  const int rm_nodes[] = {9};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_leaf__case34(void** state) {
  /*           4B
            /     \
          3R      8R
         /       /  \
        2B      6B  9B
               / \
              5R 7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, true},
    {_create_tnode(4), 3, 8, -1, false},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 8, false},
    {_create_tnode(7), -1, -1, 6, true},
    {_create_tnode(8), 6, 9, 4, true},
    {_create_tnode(9), -1, -1, 8, false},
  };
  const int rm_nodes[] = {9};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_red__case1(void** state) {
  /*           5B
            /     \
          4R      6B
         /
        2B
       / \
      1R 3R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, -1, 5, true},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_num = 4;
  //const int sub_num = 3;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case2(void** state) {
  /*           5B
            /     \
          4R      6B
         /
        2B
       /
      1R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, -1, 4, false},
    {_create_tnode(4), 2, -1, 5, true},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_num = 4;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case31(void** state) {
  /*              8B
               /     \
             7R      9B
            /         \
          4B          10B
        /    \         \
       2B    6B        11B
      / \    /
    1B  3B  5B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 7, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, -1, 4, false},
    {_create_tnode(7), 4, -1, 8, true},
    {_create_tnode(8), 7, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, -1, 10, false},
  };
  const int rm_num = 7;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case32(void** state) {
  /*            9B
              /   \
            8R    10B
           /       \
          1B       11B
           \        \
           5R       12B
         /   \
       3B    7B
      / \    /
    2B  4B  6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 5, 8, false},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, 4, 5, false},
    {_create_tnode(4), -1, -1, 3, false},
    {_create_tnode(5), 3, 7, 1, true},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 1, -1, 9, true},
    {_create_tnode(9), 8, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case33(void** state) {
  /*              8B
               /     \
             7R      9R
            /         \
          4B          10B
        /    \         \
       2R    6B        11B
      / \
    1B  3B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, true},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 7, false},
    //{_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), -1, -1, 4, false},
    {_create_tnode(7), 4, -1, 8, true},
    {_create_tnode(8), 7, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, true},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, -1, 10, false},
  };
  const int rm_num = 7;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case34(void** state) {
  /*              9B
               /     \
             8R      10B
            /         \
          5B          11B
        /    \         \
       3B    7B        12B
      / \    /
    2R  4B  6B
   /
  1B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, true},
    {_create_tnode(3), 2, 4, 5, false},
    {_create_tnode(4), -1, -1, 3, false},
    {_create_tnode(5), 3, 7, 8, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 5, -1, 9, true},
    {_create_tnode(9), 8, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case35(void** state) {
  /*              9B
               /     \
             8R      10B
            /         \
          5B          11B
        /    \         \
       2B    7B        12B
      / \    /
    1B  4R  6B
       /
      3B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 4, 5, false},
    {_create_tnode(3), -1, -1, 4, false},
    {_create_tnode(4), 3, -1, 2, true},
    {_create_tnode(5), 2, 7, 8, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 5, -1, 9, true},
    {_create_tnode(9), 8, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_red__case1(void** state) {
  /*           5B
            /     \
          1R      6B
            \
            3B
           / \
          2R 4R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 5, true},
    {_create_tnode(2), -1, -1, 3, true},
    {_create_tnode(3), 2, 4, 1, false},
    {_create_tnode(4), -1, -1, 3, true},
    {_create_tnode(5), 1, 6, -1, false},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case2(void** state) {
  /*           5B
            /     \
          1R      6B
            \
            3B
             \
             4R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 5, true},
    {_create_tnode(3), -1, 4, 1, false},
    {_create_tnode(4), -1, -1, 3, true},
    {_create_tnode(5), 1, 6, -1, false},
    {_create_tnode(6), -1, -1, 5, false},
  };
  const int rm_num = 1;
  //const int sub_num = 3;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case31(void** state) {
  /*                8B
            /             \
           1R             12B
             \            / \
             4B         11R  13B
           /   \        /     \
          2B    6B    10B     14B
           \   / \    /
           3B 5B 7B  9B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 8, true},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 1, 12, -1, false},
    {_create_tnode(9), -1, -1, 10, false},
    {_create_tnode(10), 9, -1, 11, false},
    {_create_tnode(11), 10, -1, 12, true},
    {_create_tnode(12), 11, 13, 8, false},
    {_create_tnode(13), -1, 14, 12, false},
    {_create_tnode(14), -1, -1, 13, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case32(void** state) {
  /*                11B
               /          \
             1R            12B
               \            \
                8B          13B
              /   \          \
            4R    9B         14B
          /   \     \
        2B     6B   10B
         \    / \
         3B  5B 7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 8, 11, true},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 8, true},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 4, 9, 1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, -1, 9, false},
    {_create_tnode(11), 1, 12, -1, false},
    {_create_tnode(12), -1, 13, 11, false},
    {_create_tnode(13), -1, 14, 12, false},
    {_create_tnode(14), -1, -1, 13, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case33(void** state) {
  /*            7B
            /        \
           1R        8B
             \         \
             3B        9B
           /   \
          2B    5R
               / \
              4B 6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 7, true},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, 5, 1, false},
    {_create_tnode(4), -1, -1, 5, false},
    {_create_tnode(5), 4, 6, 3, true},
    {_create_tnode(6), -1, -1, 5, false},
    {_create_tnode(7), 1, 8, -1, false},
    {_create_tnode(8), -1, 9, 7, false},
    {_create_tnode(9), -1, -1, 8, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case34(void** state) {
  /*            9B
            /        \
           1R        10B
             \         \
             4B        11B
           /    \       \
          2B     6B     12B
           \    / \
           3B  5B 7R
                   \
                   8B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 9, true},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, 8, 6, true},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 1, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case35(void** state) {
  /*            9B
            /        \
           1R        10B
             \         \
             4B        11B
           /    \       \
          2B     7B     12B
           \    / \
           3B  5R 8B
                \
                6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 9, true},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 7, 1, false},
    {_create_tnode(5), -1, 6, 7, true},
    {_create_tnode(6), -1, -1, 5, false},
    {_create_tnode(7), 5, 8, 4, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 1, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_red__case1(void** state) {
  /*           3B
            /     \
          1R       6R
            \     /
            2B  4B
                 \
                 5R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, 3, true},
    {_create_tnode(2), -1, -1, 1, false},
    {_create_tnode(3), 1, 6, -1, false},
    {_create_tnode(4), -1, 5, 6, false},
    {_create_tnode(5), -1, -1, 4, true},
    {_create_tnode(6), 4, -1, 3, true},
  };
  const int rm_num = 6;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case2(void** state) {
  /*           4B
            /      \
          1R        9R
            \       /
            3B     6B
           /     /   \
          2B    5B   8B
                    /
                   7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 4, true},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 1, false},
    {_create_tnode(4), 1, 9, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 8, 9, false},
    {_create_tnode(7), -1, -1, 8, true},
    {_create_tnode(8), 7, -1, 6, false},
    {_create_tnode(9), 6, -1, 4, true},
  };
  const int rm_num = 9;
  //const int sub_num = 8;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case31(void** state) {
  /*             5B
            /         \
          1R         12R
            \        /
            4B      9B
           /     /     \
          3B    7B     11B
         /     / \     /
        2B    6B 8B  10B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 5, true},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 1, false},
    {_create_tnode(5), 1, 12, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 12, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, -1, 9, false},
    {_create_tnode(12), 9, -1, 5, true},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case32(void** state) {
  /*               5B
            /            \
          1R             16R
            \            /
            4B          9B
           /        /       \
          3B      7B        13R
         /       / \     /      \
        2B      6B 8B  11B      15B
                      /  \      /
                    10B  12B  14B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 5, true},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 1, false},
    {_create_tnode(5), 1, 16, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9),  7, 13, 16, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, 12, 13, false},
    {_create_tnode(12), -1, -1, 11, false},
    {_create_tnode(13), 11, 15, 9, true},
    {_create_tnode(14), -1, -1, 15, false},
    {_create_tnode(15), 14, -1, 13, false},
    {_create_tnode(16), 9, -1, 5, true},
  };
  const int rm_num = 16;
  //const int sub_num = 15;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case33(void** state) {
  /*           4B
            /       \
          3R        10R
         /          /
       2B          8B
      /         /     \
    1B         6R     9B
              / \
             5B 7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, true},
    {_create_tnode(4), 3, 10, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 8, true},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 9, 10, false},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 8, -1, 4, true},
  };
  const int rm_num = 10;
  //const int sub_num = 9;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case34(void** state) {
  /*           5B
            /       \
          4R        13R
         /          /
       3B         10B
      /         /     \
    2B         8B     12B
    /         / \     /
   1B        7R 9B  11B
            /
           6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, true},
    {_create_tnode(5), 4, 13, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 8, true},
    {_create_tnode(8), 7, 9, 10, false},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 8, 12, 13, false},
    {_create_tnode(11), -1, -1, 12, false},
    {_create_tnode(12), 11, -1, 10, false},
    {_create_tnode(13), 10, -1, 5, true},
  };
  const int rm_num = 13;
  //const int sub_num = 12;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case35(void** state) {
  /*            4B
            /       \
          3B        12R
         /          /
       2B          9B
      /         /     \
    1B         6B     11B
              / \     /
             5B 8R  10B
               /
              7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 12, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 8, 9, false},
    {_create_tnode(7), -1, -1, 8, false},
    {_create_tnode(8), 7, -1, 6, true},
    {_create_tnode(9), 6, 11, 12, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, -1, 9, false},
    {_create_tnode(12), 9, -1, 4, true},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_red_case1(void** state) {
  /*           3B
            /     \
          1R      4R
            \      \
            2B     6B
                  / \
                 5R  7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, 3, true},
    {_create_tnode(2), -1, -1, 1, false},
    {_create_tnode(3), 1, 4, -1, false},
    {_create_tnode(4), -1, 6, 3, true},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, true},
  };
  const int rm_num = 4;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case2(void** state) {
  /*           3B
            /     \
          1R      4R
            \      \
            2B     5B
                    \
                     6R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, 3, true},
    {_create_tnode(2), -1, -1, 1, false},
    {_create_tnode(3), 1, 4, -1, false},
    {_create_tnode(4), -1, 5, 3, true},
    {_create_tnode(5), -1, 6, 4, false},
    {_create_tnode(6), -1, -1, 5, true},
  };
  const int rm_num = 4;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case31(void** state) {
  /*          4B
            /   \
          3B    5R
          /       \
        2B         8B
        /        /   \
       1B      6B    10B
                \    /  \
                7B  9B  11B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 5, -1, false},
    {_create_tnode(5), -1, 8, 4, true},
    {_create_tnode(6), -1, 7, 8, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 10, 5, false},
    {_create_tnode(9), -1, -1, 10, false},
    {_create_tnode(10), 9, 11, 8, false},
    {_create_tnode(11), -1, -1, 10, false},
  };
  const int rm_num = 5;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case32(void** state) {
  /*          4B
            /     \
          3B      5R
          /         \
        2B          12B
        /          /
       1B        8R
               /    \
              6B    10B
               \    /  \
               7B  9B  11B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 5, -1, false},
    {_create_tnode(5), -1, 12, 4, true},
    {_create_tnode(6), -1, 7, 8, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 10, 12, true},
    {_create_tnode(9), -1, -1, 10, false},
    {_create_tnode(10), 9, 11, 8, false},
    {_create_tnode(11), -1, -1, 10, false},
    {_create_tnode(12), 8, -1, 5, false},
  };
  const int rm_num = 5;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case33(void** state) {
  /*          4B
            /   \
          3R    5R
          /       \
        2B         7B
        /        /   \
       1B      6B     9R
                     /  \
                    8B  10B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, true},
    {_create_tnode(4), 3, 5, -1, false},
    {_create_tnode(5), -1, 7, 4, true},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 9, 5, false},
    {_create_tnode(8), -1, -1, 9, false},
    {_create_tnode(9), 8, 10, 7, true},
    {_create_tnode(10), -1, -1, 9, false},
  };
  const int rm_num = 5;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case34(void** state) {
  /*          4B
            /   \
          3R    5R
          /       \
        2B         7B
        /        /   \
       1B      6B     9B
                     /  \
                    8R  10R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, true},
    {_create_tnode(4), 3, 5, -1, false},
    {_create_tnode(5), -1, 7, 4, true},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 9, 5, false},
    {_create_tnode(8), -1, -1, 9, true},
    {_create_tnode(9), 8, 10, 7, false},
    {_create_tnode(10), -1, -1, 9, true},
  };
  const int rm_num = 5;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case35(void** state) {
  /*          4B
            /   \
          3B    5R
          /       \
        2B         8B
        /        /    \
       1B      6B      11B
                \     /  \
                7B  10R  12B
                   /
                  9B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 5, -1, false},
    {_create_tnode(5), -1, 8, 4, true},
    {_create_tnode(6), -1, 7, 8, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 11, 5, false},
    {_create_tnode(9), -1, -1, 10, false},
    {_create_tnode(10), 9, -1, 11, true},
    {_create_tnode(11), 10, 12, 8, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 5;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_red__case1(void** state) {
  /*           5B
            /     \
          4B      6B
         /         \
       2B          7B
      /  \
     1R  3R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 7, 5, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_num = 4;
  //const int sub_num = 3;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case2(void** state) {
  /*           6B
            /     \
          5B      7B
         /         \
       2R          8B
      /  \
     1B  4B
         /
        3R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 4, 5, true},
    {_create_tnode(3), -1, -1, 4, true},
    {_create_tnode(4), 3, -1, 2, false},
    {_create_tnode(5), 2, -1, 6, false},
    {_create_tnode(6), 5, 7, -1, false},
    {_create_tnode(7), -1, 8, 6, false},
    {_create_tnode(8), -1, -1, 7, false},
  };
  const int rm_num = 5;
  //const int sub_num = 4;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case31(void** state) {
  /*            8B
             /     \
           7B      9B
          /         \
         4B         10B
      /     \        \
     2B     6B       11B
    / \    /          \
   1B 3B  5B          12B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 7, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, -1, 4, false},
    {_create_tnode(7), 4, -1, 8, false},
    {_create_tnode(8), 7, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 7;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case32(void** state) {
  /*            8B
             /     \
           7B      9B
          /         \
         4R         10B
      /     \        \
     2B     6B       11B
    / \    /
   1B 3B  5B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 7, true},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, -1, 4, false},
    {_create_tnode(7), 4, -1, 8, false},
    {_create_tnode(8), 7, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, -1, 10, false},
  };
  const int rm_num = 7;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case33(void** state) {
  /*            7B
             /     \
           6B      8B
          /         \
         4B         9B
      /     \        \
     2R     5B       10B
    / \
   1B 3B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, true},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 5, 6, false},
    {_create_tnode(5), -1, -1, 4, false},
    {_create_tnode(6), 4, -1, 7, false},
    {_create_tnode(7), 6, 8, -1, false},
    {_create_tnode(8), -1, 9, 7, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, -1, 9, false},
  };
  const int rm_num = 6;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case34(void** state) {
  /*            9B
             /     \
           8B      10B
          /         \
         5B         11B
      /     \        \
     3B     7B       12B
    / \     /         \
   2R 4B   6B         13B
  /
 1B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, true},
    {_create_tnode(3), 2, 4, 5, false},
    {_create_tnode(4), -1, -1, 3, false},
    {_create_tnode(5), 3, 7, 8, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 5, -1, 9, false},
    {_create_tnode(9), 8, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, 13, 11, false},
    {_create_tnode(13), -1, -1, 12, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case35(void** state) {
  /*            9B
             /     \
           8B      10B
          /         \
         5B         11B
      /     \        \
     2B     7B       12B
    /  \    /         \
   1B  4R  6B         13B
      /
     3B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 4, 5, false},
    {_create_tnode(3), -1, -1, 4, false},
    {_create_tnode(4), 3, -1, 2, true},
    {_create_tnode(5), 2, 7, 8, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 5, -1, 9, false},
    {_create_tnode(9), 8, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, 13, 11, false},
    {_create_tnode(13), -1, -1, 12, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_red__case1(void** state) {
  /*           5B
            /     \
          1B      6B
           \       \
           3B      7B
          /  \
         2R  4R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 5, false},
    {_create_tnode(2), -1, -1, 3, true},
    {_create_tnode(3), 2, 4, 1, false},
    {_create_tnode(4), -1, -1, 3, true},
    {_create_tnode(5), 1, 6, -1, false},
    {_create_tnode(6), -1, 7, 5, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case2(void** state) {
  /*           6B
            /     \
          1B      7B
           \       \
           4R      8B
          /  \
         2B  5B
          \
          3R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 6, false},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 5, 1, true},
    {_create_tnode(5), -1, -1, 4, false},
    {_create_tnode(6), 1, 7, -1, false},
    {_create_tnode(7), -1, 8, 6, false},
    {_create_tnode(8), -1, -1, 7, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case31(void** state) {
  /*           8B
            /     \
          1B      9B
           \       \
            4B     10B
          /    \     \
         2B     6B   11B
          \    / \     \
          3B  5B 7B   12B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 8, false},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 1, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case32(void** state) {
  /*           8B
            /     \
          1B      9B
           \       \
            4R     10B
          /    \     \
         2B     6B   11B
          \    / \
          3B  5B 7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 8, false},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, 1, true},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 1, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, -1, 10, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case33(void** state) {
  /*           7B
            /     \
          1B      8B
           \       \
            3B     9B
          /    \     \
         2B     5R   10B
               / \
              4B 6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 7, false},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, 5, 1, false},
    {_create_tnode(4), -1, -1, 5, false},
    {_create_tnode(5), 4, 6, 3, true},
    {_create_tnode(6), -1, -1, 5, false},
    {_create_tnode(7), 1, 8, -1, false},
    {_create_tnode(8), -1, 9, 7, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, -1, 9, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case34(void** state) {
  /*           7B
            /     \
          1B      8B
           \       \
            3B     9B
          /    \     \
         2B     5B   10B
               / \
              4R 6R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 3, 7, false},
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, 5, 1, false},
    {_create_tnode(4), -1, -1, 5, true},
    {_create_tnode(5), 4, 6, 3, false},
    {_create_tnode(6), -1, -1, 5, true},
    {_create_tnode(7), 1, 8, -1, false},
    {_create_tnode(8), -1, 9, 7, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, -1, 9, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case35(void** state) {
  /*           9B
            /     \
          1B      10B
           \       \
            4B     11B
          /    \     \
         2B     7B   12B
          \    / \    \
          3B  5R 8B   13B
               \
               6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 4, 9, false},
    {_create_tnode(2), -1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 7, 1, false},
    {_create_tnode(5), -1, 6, 7, true},
    {_create_tnode(6), -1, -1, 5, false},
    {_create_tnode(7), 5, 8, 4, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 1, 10, -1, false},
    {_create_tnode(10), -1, 11, 9, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, 13, 11, false},
    {_create_tnode(13), -1, -1, 12, false},
  };
  const int rm_num = 1;
  //const int sub_num = 2;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_red__case1(void** state) {
  /*           3B
            /     \
           2B     7B
          /       /
         1B      5B
                /  \
               4R  6R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, 7, -1, false},
    {_create_tnode(4), -1, -1, 5, true},
    {_create_tnode(5), 4, 6, 7, false},
    {_create_tnode(6), -1, -1, 5, true},
    {_create_tnode(7), 5, -1, 3, false},
  };
  const int rm_num = 7;
  //const int sub_num = 6;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case2(void** state) {
  /*           3B
            /     \
           2B     8B
          /       /
         1B      5R
                /  \
               4B  7B
                  /
                 6R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, 8, -1, false},
    {_create_tnode(4), -1, -1, 5, false},
    {_create_tnode(5), 4, 7, 8, true},
    {_create_tnode(6), -1, -1, 7, true},
    {_create_tnode(7), 6, -1, 5, false},
    {_create_tnode(8), 5, -1, 3, false},
  };
  const int rm_num = 8;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case31(void** state) {
  /*           5B
            /     \
           4B     12B
          /       /
         3B      9B
        /      /   \
      2B     7B    11B
     /      / \     /
    1B     6B 8B  10B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 12, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 12, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, -1, 9, false},
    {_create_tnode(12), 9, -1, 5, false},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case32(void** state) {
  /*           5B
            /     \
           4B     12B
          /       /
         3B      9R
        /      /   \
      2B     7B    11B
            / \     /
           6B 8B  10B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 12, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 12, true},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, -1, 9, false},
    {_create_tnode(12), 9, -1, 5, false},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case33(void** state) {
  /*           5B
            /     \
           4B     12B
          /       /
         3B      9B
        /      /   \
      2B     7R    11B
            / \
           6B 8B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 12, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 9, true},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 12, false},
    {_create_tnode(11), -1, -1, 9, false},
    {_create_tnode(12), 9, -1, 5, false},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case34(void** state) {
  /*           5B
            /     \
           4B     12B
          /       /
         3B      9B
        /      /   \
      2B     7B    11B
            / \
           6R 8R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 12, -1, false},
    {_create_tnode(6), -1, -1, 7, true},
    {_create_tnode(7), 6, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, true},
    {_create_tnode(9), 7, 11, 12, false},
    {_create_tnode(11), -1, -1, 9, false},
    {_create_tnode(12), 9, -1, 5, false},
  };
  const int rm_num = 12;
  //const int sub_num = 11;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case35(void** state) {
  /*           5B
            /     \
           4B     13B
          /       /
         3B     10B
        /      /   \
      2B     7B    12B
      /     / \    /
    1B     6B 8R  11B
               \
               9B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 13, -1, false},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, 8, 10, false},
    {_create_tnode(8), -1, 9, 7, true},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 7, 12, 13, false},
    {_create_tnode(11), -1, -1, 12, false},
    {_create_tnode(12), 11, -1, 10, false},
    {_create_tnode(13), 10, -1, 5, false},
  };
  const int rm_num = 13;
  //const int sub_num = 12;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_red__case1(void** state) {
  /*           3B
            /     \
           2B     4B
          /        \
         1B        6B
                  /  \
                 5R  7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, 4, -1, false},
    {_create_tnode(4), -1, 6, 3, false},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, true},
  };
  const int rm_num = 4;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case2(void** state) {
  /*           3B
            /     \
           2B     4B
          /        \
         1B        7R
                  /  \
                 5B  8B
                  \
                  6R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, 4, -1, false},
    {_create_tnode(4), -1, 7, 3, false},
    {_create_tnode(5), -1, 6, 7, false},
    {_create_tnode(6), -1, -1, 5, true},
    {_create_tnode(7), 5, 8, 4, true},
    {_create_tnode(8), -1, -1, 7, false},
  };
  const int rm_num = 4;
  //const int sub_num = 5;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case31(void** state) {
  /*          5B
            /    \
           4B    6B
          /       \
         3B        9B
        /       /     \
       2B      7B     11B
       /        \     / \
      1B        8B  10B 12B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 9, 5, false},
    {_create_tnode(7), -1, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 6, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, 12, 9, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 6;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case32(void** state) {
  /*          5B
            /    \
           4B    6B
          /       \
         3B        9R
        /       /     \
       2B      7B     11B
                \     / \
                8B  10B 12B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 9, 5, false},
    {_create_tnode(7), -1, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 6, true},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, 12, 9, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 6;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case33(void** state) {
  /*          5B
            /    \
           4B    6B
          /       \
         3B        9B
        /       /     \
       2B      7B     11R
                      / \
                    10B 12B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(2), -1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 9, 5, false},
    {_create_tnode(7), -1, -1, 9, false},
    {_create_tnode(9), 7, 11, 6, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, 12, 9, true},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_num = 6;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case34(void** state) {
  /*          5B
            /    \
           4B    6B
          /       \
         3B        9B
        /       /     \
       2B      7B     11B
       /        \     / \
      1B        8B  10B 12R
                          \
                          13B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 9, 5, false},
    {_create_tnode(7), -1, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 11, 6, false},
    {_create_tnode(10), -1, -1, 11, false},
    {_create_tnode(11), 10, 12, 9, false},
    {_create_tnode(12), -1, 13, 11, true},
    {_create_tnode(13), -1, -1, 12, false},
  };
  const int rm_num = 6;
  //const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case35(void** state) {
  /*          5B
            /    \
           4B    6B
          /       \
         3B        9B
        /       /     \
       2B      7B     12B
       /        \     / \
      1B        8B  10R 13B
                      \
                      11B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, -1, 5, false},
    {_create_tnode(5), 4, 6, -1, false},
    {_create_tnode(6), -1, 9, 5, false},
    {_create_tnode(7), -1, 8, 9, false},
    {_create_tnode(8), -1, -1, 7, false},
    {_create_tnode(9), 7, 12, 6, false},
    {_create_tnode(10), -1, 11, 12, true},
    {_create_tnode(11), -1, -1, 10, false},
    {_create_tnode(12), 10, 13, 9, false},
    {_create_tnode(13), -1, -1, 12, false},
  };
  const int rm_num = 6;
  ////const int sub_num = 7;
  const int rm_nodes[] = {rm_num};
  const int black_cnt_after_remove = 5;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_fullnode__substitued_by_red(void** state) {
  /*                 8B
                 /       \
               4R        9B
           /       \       \
         2B         6B     10R
        /  \      /  \
      1R   3R    5R   7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 6, 8, true},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, true},
    {_create_tnode(8), 4, 9, -1, false},
    {_create_tnode(9), -1, 10, 8, false},
    {_create_tnode(10), -1, -1, 9, true},
  };
  const int rm_nodes[] = {4};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_red_fullnode__substitued_by_black(void** state) {
  /*                10B
                 /       \
               5R        11B
           /       \       \
         3B         8B     12B
        /  \      /  \
      1B   4B    7R   9B
       \        /
       2R      6B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, 3, false},
    {_create_tnode(2), -1, -1, 1, true},
    {_create_tnode(3), 1, 4, 5, false},
    {_create_tnode(4), -1, -1, 3, false},
    {_create_tnode(5), 3, 8, 10, true},
    {_create_tnode(6), -1, -1, 7, false},
    {_create_tnode(7), 6, -1, 8, true},
    {_create_tnode(8), 7, 9, 5, false},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 5, 11, -1, false},
    {_create_tnode(11), -1, 12, 10, false},
    {_create_tnode(12), -1, -1, 11, false},
  };
  const int rm_nodes[] = {5};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_fullnode__substitued_by_red(void** state) {
  /*                 8B
                 /       \
               4B         9B
           /       \       \
         2B         6B     11B
        /  \      /  \     /
      1R   3R    5R   7R  10R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 6, 8, false},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, true},
    {_create_tnode(8), 4, 9, -1, false},
    {_create_tnode(9), -1, 11, 8, false},
    {_create_tnode(10), -1, -1, 11, true},
    {_create_tnode(11), 10, -1, 9, false},
  };
  const int rm_nodes[] = {4};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_left_black_fullnode__substitued_by_black(void** state) {
  /*                10B
                 /       \
               5B        11B
           /       \       \
         3B         8B     13B
        /  \      /  \     /
      1B   4B    7B   9B  12B
       \
       2R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, 3, false},
    {_create_tnode(2), -1, -1, 1, true},
    {_create_tnode(3), 1, 4, 5, false},
    {_create_tnode(4), -1, -1, 3, false},
    {_create_tnode(5), 3, 8, 10, false},
    {_create_tnode(7), -1, -1, 8, false},
    {_create_tnode(8), 7, 9, 5, false},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 5, 11, -1, false},
    {_create_tnode(11), -1, 13, 10, false},
    {_create_tnode(12), -1, -1, 13, false},
    {_create_tnode(13), 12, -1, 11, false},
  };
  const int rm_nodes[] = {5};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_fullnode__substitued_by_red(void** state) {
  /*                    3B
                  /           \
                2B            8B
               /          /       \
              1B         6R        11R
                       /  \       /   \
                    4B   7B     9B    13B
                                 \    /
                                10R  12R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, 8, -1, false},
    {_create_tnode(4), -1, -1, 6, false},
    {_create_tnode(6), 4, 7, 8, true},
    {_create_tnode(7), -1, -1, 6, false},
    {_create_tnode(8), 6, 11, 3, false},
    {_create_tnode(9), -1, 10, 11, false},
    {_create_tnode(10), -1, -1, 9, true},
    {_create_tnode(11), 9, 13, 8, true},
    {_create_tnode(12), -1, -1, 13, true},
    {_create_tnode(13), 12, -1, 11, false},
  };
  const int rm_nodes[] = {11};
  const int black_cnt_after_remove = 3;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_red_fullnode__substitued_by_black(void** state) {
  /*                    4B
                  /           \
                3B            10R
               /          /       \
              2B        7B        13B
             /        /   \      /   \
            1B       5B   8B    11B   15B
                      \    \     \    /
                      6B   9B   12B  14B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 10, -1, false},
    {_create_tnode(5), -1, 6, 7, false},
    {_create_tnode(6), -1, -1, 5, false},
    {_create_tnode(7), 5, 8, 10, false},
    {_create_tnode(8), -1, 9, 7, false},
    {_create_tnode(9), -1, -1, 8, false},
    {_create_tnode(10), 7, 13, 4, true},
    {_create_tnode(11), -1, 12, 13, false},
    {_create_tnode(12), -1, -1, 11, false},
    {_create_tnode(13), 11, 15, 10, false},
    {_create_tnode(14), -1, -1, 15, false},
    {_create_tnode(15), 14, -1, 13, false},
  };
  const int rm_nodes[] = {10};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_fullnode__substitued_by_red(void** state) {
  /*                    4B
                  /           \
                3B            9B
               /          /       \
              2B        6B        13R
             /         /  \       /   \
            1B       5B   8R    12B   14B
                        /      /       \
                       7B     10R       15B
                               \
                               11B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 9, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 8, 9, false},
    {_create_tnode(7), -1, -1, 8, false},
    {_create_tnode(8), 7, -1, 6, true},
    {_create_tnode(9), 6, 13, 4, false},
    {_create_tnode(10), -1, 11, 12, true},
    {_create_tnode(11), -1, -1, 10, false},
    {_create_tnode(12), 10, -1, 13, false},
    {_create_tnode(13), 12, 14, 9, true},
    {_create_tnode(14), -1, 15, 13, false},
    {_create_tnode(15), -1, -1, 14, false},
  };
  const int rm_nodes[] = {9};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_right_black_fullnode__substitued_by_black(void** state) {
  /*                    4B
                  /           \
                3B            9B
               /          /       \
              2B        6B        13R
             /         /  \       /   \
            1B       5B   8B    12B   14B
                               /       \
                              10B       15B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, -1, 3, false},
    {_create_tnode(3), 2, -1, 4, false},
    {_create_tnode(4), 3, 9, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 8, 9, false},
    {_create_tnode(8), -1, -1, 6, false},
    {_create_tnode(9), 6, 13, 4, false},
    {_create_tnode(10), -1, -1, 12, false},
    {_create_tnode(12), 10, -1, 13, false},
    {_create_tnode(13), 12, 14, 9, true},
    {_create_tnode(14), -1, 15, 13, false},
    {_create_tnode(15), -1, -1, 14, false},
  };
  const int rm_nodes[] = {9};
  const int black_cnt_after_remove = 4;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_root__without_child(void** state) {
  /*                    1B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, -1, false},
  };
  const int rm_num = 1;
  struct rbtree_t* tree = _make_tree(node_infos, countof(node_infos));

  rbt_remove(tree, (void*)rm_num);

  assert_true(_empty_tree(tree));

  destroy_rbtree(tree);
}

void test_rbtree_remove_root__has_only_left_child__substitued_by_red(void** state) {
  /*                     2B
                        /
                       1R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, -1, -1, false},
  };
  const int rm_nodes[] = {2};
  const int black_cnt_after_remove = 1;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_root__has_only_right_child__substitued_by_red(void** state) {
  /*                     1B
                          \
                          2R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, 2, -1, false},
    {_create_tnode(2), -1, -1, 1, true},
  };
  const int rm_nodes[] = {1};
  const int black_cnt_after_remove = 1;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_root__has_all_childs__substitued_by_red(void** state) {
  /*                        4B
                         /    \
                       2B     6B
                      / \    / \
                    1R  3R  5R  7R
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, true},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, true},
    {_create_tnode(4), 2, 6, -1, false},
    {_create_tnode(5), -1, -1, 6, true},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, true},
  };
  const int rm_nodes[] = {4};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}

void test_rbtree_remove_root__has_all_childs__substitued_by_black(void** state) {
  /*                        4B
                         /    \
                       2B     6B
                      / \    / \
                    1B  3B  5B  7B
  */
  const _node_info_t node_infos[] = {
    {_create_tnode(1), -1, -1, 2, false},
    {_create_tnode(2), 1, 3, 4, false},
    {_create_tnode(3), -1, -1, 2, false},
    {_create_tnode(4), 2, 6, -1, false},
    {_create_tnode(5), -1, -1, 6, false},
    {_create_tnode(6), 5, 7, 4, false},
    {_create_tnode(7), -1, -1, 6, false},
  };
  const int rm_nodes[] = {4};
  const int black_cnt_after_remove = 2;

  _test_rbtree_remove(node_infos, countof(node_infos), rm_nodes, countof(rm_nodes), black_cnt_after_remove);
}
