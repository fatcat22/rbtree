#include "_test_common.h"

rbnode_t* _rbnode_alloc_never_called(void* val) {
    assert_false("this function should never be called");
    return NULL;
}
void _rbnode_free_never_called(rbnode_t* node) {
    assert_false("this function should never be called");
}

int _creation_node_compare(const rbnode_t* n1, const rbnode_t* n2) {
    return 1;
}

int _creation_value_compare(void* val, const rbnode_t* node) {
    return 1;
}

void test_rbtree_create_assert(void** state) {
    expect_assert_failure(create_rbtree(NULL, _rbnode_free_never_called, _creation_node_compare, _creation_value_compare));
    expect_assert_failure(create_rbtree(_rbnode_alloc_never_called, NULL, _creation_node_compare, _creation_value_compare));
    expect_assert_failure(create_rbtree(_rbnode_alloc_never_called, _rbnode_free_never_called, NULL, _creation_value_compare));
    expect_assert_failure(create_rbtree(_rbnode_alloc_never_called, _rbnode_free_never_called, _creation_node_compare, NULL));
}

void test_rbtree_destroy_assert(void** state) {
    expect_assert_failure(destroy_rbtree(NULL));
}

void test_rbtree_create_destroy(void** state) {
    struct rbtree_t* tree = create_rbtree(_rbnode_alloc_never_called, _rbnode_free_never_called, _creation_node_compare, _creation_value_compare);
    assert_true(NULL != tree);

    assert_true(_empty_tree(tree));
    destroy_rbtree(tree);
}
