#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
};

struct TreeNode* create_bst_node(int val) {
    struct TreeNode* node = malloc(sizeof(*node));
    node->left = node->right = 0;
    node->val = val;
    return node;
}

void insert_bst_node(struct TreeNode* root, int val) {
    struct TreeNode* node = root;

    while (node != 0) {
        if (node->val < val) {
            if (node->right == 0) {
                node->right = create_bst_node(val);
                return;
            }
            node = node->right;
        } else {
            if (node->left == 0) {
                node->left = create_bst_node(val);
                return;
            }
            node = node->left;
        }
    }

    root = create_bst_node(val);
}

void traverse_inorder(struct TreeNode* root) {
    if (root == 0) return;
    traverse_inorder(root->left);
    printf("%d ", root->val);
    traverse_inorder(root->right);
}

void free_tree(struct TreeNode* root) {
    if (root == 0) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

long long get_microseconds() {
    struct timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec;
}

int main(int argc, char *argv[]) {
    srand(time(0));

    const int NODE_COUNT = 1000;
    const int NODE_MIDPOINT = NODE_COUNT / 2;

    // Keep a boolean table of used integers
    int table[NODE_COUNT];
    memset(table, 0, sizeof(table));

    long long start_time = get_microseconds();

    // Create the root node at the midpoint of the range
    struct TreeNode* root = create_bst_node(NODE_MIDPOINT);
    table[NODE_MIDPOINT] = 1;

    printf("Starting to populate BST randomly (%d nodes):\n", NODE_COUNT);
    for (int i = 0; i != NODE_COUNT - 1; ++i) {
        int randomNumber;
        do {
            randomNumber = rand() % NODE_COUNT;
        } while (table[randomNumber] != 0);
        table[randomNumber] = 1;

        printf("%d ", randomNumber);
        insert_bst_node(root, randomNumber);
    }
    printf("\n");

    long long end_time = get_microseconds();
    printf("microseconds elapsed: %lld\n", end_time - start_time);

    printf("\nValidate BST with inorder traversal:\n");
    traverse_inorder(root);
    printf("\n");

    free_tree(root);
    return 0;
}

