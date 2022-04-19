#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
//#include <scapegoat.h>

#define LOG32(n) (log(n) / log(3.0/2.0))
/*We need to establish the characteristcs of a binary tree
             50(data)
      ┌───────┴───────┐
    40(Left Child)    60(Right Child)
  ┌───┴───┐       ┌───┴───┐
  35      42     54       70
┌─┴─┐   ┌─┴─┐   ┌─┴─┐   ┌─┴─┐
0   0   0   0   0   0   0   0
*/


typedef struct Node
{
    int data;
    int n, q;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

//Checks for size of tree
int size(Node* root)
{
    if (root == NULL)
    {
        return 0;
    }
    return size(root->left) + size(root->right) + 1;
}
//Checks the depth of the tree
int depth(Node* root)
{
    if (root == NULL) return -1;
    else
    {
        int depthRight = depth(root->right);
        int depthLeft = depth(root->left);
        if (depthRight > depthLeft)
        {
            return (depthRight + 1);
        }
        else
        {
            return (depthLeft + 1);
        }
    }
}

/*
This function has now made a Node, where it establishes the left and
right node as NULL and inputs the first data as the main node
            Data
      ┌───────┴───────┐
     NULL           NULL
*/

Node* addNode(int data)
{
    Node* newNode = malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Out of memory");
        exit(EXIT_FAILURE);
    }
    if (newNode != NULL)
    {
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->parent = NULL;
        newNode->data = data;
        newNode->q = 0;
        newNode->n = 0;
    }
    return newNode;
}

//Searches for the given value provided by the user
int search(Node* root, int data)
{

    while (root != NULL)
    {
        if (data < root->data)
        {
            root = root->left;
        }
        else if (data > root->data)
        {
            root = root->right;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}
Node* sgSearch(Node* root, int data)
{
    if (root == NULL || root->data == data)
    {
        return root;
    }
    else if (root->data < data)
    {
        return sgSearch(root->right, data);
    }
    else
    {
        return sgSearch(root->left, data);
    }
}

//Insert function for binary search tree
Node* insert(Node* root, int data)
{
    if (root == NULL) return addNode(data);
    else if (data > root->data)
    {
        root->right = insert(root->right, data);
        root->right->parent = root;
    }
    else
    {
        root->left = insert(root->left, data);
        root->left->parent = root;
    }
    return root;
}

//A call to flatten(x, y) receives a pointer to a tree, and one to the head of a list
//It flattens x into a list of nodes, and appends y to the end of the list
//End result is a list along the right children
Node* flatten(Node* x, Node* y)
{
    if (x == NULL)
    {
        return y;
    }
    else
    {
        x->right = flatten(x->right, y);
        return flatten(x->left, x);
    }
}
/*
We've constructed a linked list with nodes in non-decreasing order
Assume we are building a tree of n nodes using a list, x, of atleast
n+1 nodes.
A tree is made recursively by using first half of the list to construct ride side
Then using the second half to build the right half
*/

Node* buildTree(int n, Node* x)
{
    if (n == 0)
    {
        x->left = NULL;
        return x;
    }
    Node* r, * s;
    r = buildTree(ceil((double)((n - 1) / 2.0)), x);
    s = buildTree(floor(((n - 1) / 2.0)), r->right);
    r->right = s->left;
    s->left = r;
    return s;
}
/*
Rebuild-tree accepts a pointer to the scapegoat node, and the
size, n, of that subtree. It returns the root of the rebuilt subtree, ready to reconnect.
*/
Node* rebuild(int n, Node* s)
{
    Node* w, * z;

    z = flatten(s, w); //Converts exsisting tree into list of n items
    buildTree(n, z); //Build needs one extra node, w, at the end of the list
    //create dumby node w? I was told I need to allocate space for w
    //but don't know how to, or have it point to some memory address.
    return w->left;
}

Node* scapeInsert(Node* root, int data)
{
    Node* u;
    Node* w;
    root = insert(root, data); //BST insertion
    int d = depth(root);
    if (search(root, data)) //Checks if the insertion went thru
    {
        root->q++; //overestimate of n
        root->n++; //number of nodes in the tree
        u = sgSearch(root, data);
        if (d > LOG32(root->q)) //checks if the depth of node of the tree is greater than LOG32(q)
        {
            w = u->parent;
            while (3 * size(w) <= 2 * size(w->parent)) //Walks up the tree to find that the tree rooted accounts for more than 2/3 of parents tree
            {
                w = w->parent;
            }
            return rebuild(size(w->parent), w->parent); //rebuilds tree subrooted at w.parent
        }

    }
    return root;
}
//The node that is furthest to the left of the root
Node* minData(Node* root)
{
    if (root == NULL)
    {
        return 0;
    }
    while (root->left != NULL)
    {
        root = root->left;
    }
    return root;
}

//3 cases: No children, 1 child, and 2 children
Node* delete(Node* root, int data)
{
    if (root == NULL) return root;
    else if (data < root->data)
        root->left = delete(root->left, data);
    else if (data > root->data)
        root->right = delete(root->right, data);
    else
    {
        //Case 1: No child
        if (root->left == NULL && root->right == NULL)
        {
            free(root);
            root = NULL;
        }
        //Case 2: One child
        else if (root->left == NULL)
        {
            Node* temp = root;
            root = root->right;
            free(temp);
        }
        else if (root->right == NULL)
        {
            Node* temp = root;
            root = root->left;
            free(temp);
        }
        //Case 3: 2 Children
        else
        {
            Node* temp = minData(root->right);
            root->data = temp->data;
            root->right = delete(root->right, temp->data);
        }
    }
    return root;
}

//Recursive Pre-Traversal Function
void preOrder(Node* root)
{
    if (root == NULL)
    {
        return;
    }
    printf("%d ", root->data);
    preOrder(root->left);
    preOrder(root->right);
}
//Post order traversal - Recursive
void postOrder(Node* root)
{
    if (root == NULL)
        printf("Root is null");
    return;

    printf("left\n");
    postOrder(root->left);
    printf("right\n");
    postOrder(root->right);
    printf("value = %d\n", root->data);

}
void inOrder(Node* root)
{
    if (root == NULL) {
        return;
    }
    inOrder(root->left);
    printf("Current node %d ", root->data);
    if (root->parent == NULL) {
        printf("NULL \n");
    }
    else {
        printf("Parent: %d\n", root->parent->data);
    }
    inOrder(root->right);
}
/*In order traversal - Recursive
void inOrder(Node *root)
{
    if (root==NULL) return;

    inOrder(root->left);
    printf("%d", root->data);
    inOrder(root->right);

}*/

//The tree will be emptied when called
void emptyTree(Node* root)
{
    if (root == NULL) return;
    emptyTree(root->left);
    emptyTree(root->right);
    free(root);

}

int main()
{

    Node* root;
    root = scapeInsert(NULL, 8);
    scapeInsert(root, 7);
    scapeInsert(root, 9);
    scapeInsert(root, 10);
    scapeInsert(root, 6);
    scapeInsert(root, 2);
    scapeInsert(root, 1);
    scapeInsert(root, 5);
    scapeInsert(root, 0);
    scapeInsert(root, 3);
    scapeInsert(root, 4); //This is the node that will activate rebuild tree

    inOrder(root);
}