#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int value;
    struct Node * left;
    struct Node * right;
} Node;

Node* create_tree(int root_value) {
    Node *root = malloc(sizeof(Node));
    if (root == NULL) {
        return NULL;
    } 
    root->value = root_value;
    root->left = NULL;
    root->right = NULL;
    return root;
}

void free_tree(Node* root) {
    free(root);
}

void insert_inorder(Node *root, int value) {
    if (root == NULL) {
        return;
    }
    if (value <= root->value) {
        if (root->left == NULL) {
            Node *node = create_tree(value);
            root->left = node;
            return;
        }
        insert_inorder(root->left, value);
        return;
    }
    if (root->right == NULL) {
        Node *node = create_tree(value);
        root->right = node;
        return;
    }
    insert_inorder(root->right, value);
}

void print_tree(Node * root) {
    if (root == NULL) {
        return;
    }
    print_tree(root->left);
    printf("%d\n", root->value);
    print_tree(root->right);
}

int main(int argc, const char * argv[]) {
    Node *root = create_tree(12);

    insert_inorder(root, 34);
    insert_inorder(root, 1);
    insert_inorder(root, -5);
    
    print_tree(root);

    free_tree(root); 
}