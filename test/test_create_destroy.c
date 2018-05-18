#include "_test_common.h"

void _rbnode_free_never_called(rbnode_t* node) {
  assert_false("this function should never be called");
}

int _creation_test_compare(const rbnode_t* n1, const rbnode_t* n2) {
  return 1;
}

void test_rbtree_create_assert(void** state) {
  expect_assert_failure(create_rbtree(NULL, _rbnode_free_never_called));
  expect_assert_failure(create_rbtree(_creation_test_compare, NULL));
}

void test_rbtree_destroy_assert(void** state) {
  expect_assert_failure(destroy_rbtree(NULL));
}

void test_rbtree_create_destroy(void** state) {
  struct rbtree_t* tree = create_rbtree(_creation_test_compare, _rbnode_free_never_called);
  assert_true(NULL != tree);

  assert_true(_empty_tree(tree));
  destroy_rbtree(tree);
}
