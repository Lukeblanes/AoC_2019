#include <stdint.h>
#include <stdlib.h>

#define CHILDREN_SIZE 1000

typedef struct Node{
    uint32_t childrenSize;
    char* element;
    struct Node **children;
} Node;

typedef struct{
    Node *root;
} Tree;

void nodes_print(Node* node)
{
    if(node == NULL)
    {
        printf("Somethings went wrong while printing nodes\n");
    }
    else{
        if(node->childrenSize > 0)
        {
            printf("Children of %s : ", node->element);
            for(uint32_t i = 0; i < node->childrenSize; i++)
                printf("%s, ", node->children[i]->element);
            printf("\n");
        }
        for(uint32_t i = 0; i < node->childrenSize; i++)
            nodes_print(node->children[i]);
    }
}

void tree_print(Tree *parent)
{
    printf("------- PRINT TREE -------\n");
    nodes_print(parent->root);
    printf("\n--------------------------\n");
}

Node* node_create(const char* str)
{
    Node* node;
    node = malloc(sizeof *node);
    node->childrenSize = 0;
    node->element = malloc(strlen(str) + 1);
    strcpy(node->element, str);

    node->children = malloc((sizeof *node) * CHILDREN_SIZE) ;
    for(uint32_t i = 0; i < CHILDREN_SIZE; ++i)
    {
        node->children[i] = NULL;
    }
    return node;
}

void node_add_child(Node **base, Node **child)
{
    if((*base)->childrenSize == CHILDREN_SIZE)
    {
        printf("Too many children, increase CHILDREN_SIZE\n");
        exit(-1);
    }
    else{
        (*base)->children[(*base)->childrenSize] = *child;
        (*base)->childrenSize++;
    }
}

Tree* tree_init()
{
    Tree *tree = (Tree*)malloc(sizeof *tree);
    tree->root = node_create("ROOT");
    return tree;
}

// returns the father of node that contains to_find
Node* node_find_father(Node* current, const char* to_find)
{
    Node* to_return = NULL;
    for(uint32_t i = 0; i < current->childrenSize; i++)
    {
        if(strcmp(current->children[i]->element, to_find))
        {
            to_return = current;
            break;
        }
        else
        {
            to_return = node_find_father(current->children[i], to_find);
        }
    }

    return to_return;
}

// returns the node that contains to_find
Node* node_find(Node* current, const char* to_find)
{
    Node* to_return = NULL;
    if(strcmp(current->element, to_find) == 0)
    {
        to_return = current;
    }
    else
    {
        for(uint32_t i = 0; i < current->childrenSize; i++)
        {
            to_return = node_find(current->children[i], to_find);
            if(to_return != NULL) // if we found our orbit don't look at other children
                break;
        }
    }
    return to_return;//t es
}

// doesn't destroy node, it just stops pointing to one of it's children
void node_remove_child(Node** current, Node *to_remove)
{
    for(uint32_t i = 0; i < (*current)->childrenSize; ++i)
    {
        if((*current)->children[i] == to_remove)
        {
            (*current)->children[i] = (*current)->children[(*current)->childrenSize-1];
            (*current)->childrenSize--;
            break;
        }
    }
}

void tree_add(Tree** tree, const char* baseStr, const char* childStr)
{
    Node* base = node_find((*tree)->root, baseStr);
    Node* child = node_find((*tree)->root, childStr);
    if( base == NULL && child == NULL) 
    {
        base = node_create(baseStr);
        node_add_child(&((*tree)->root), &base);
        child = node_create(childStr);
        node_add_child(&base, &child);
    }
    else if ( base != NULL && child != NULL)
    {
        Node* new_father = node_find_father((*tree)->root, child->element);
        node_remove_child(&new_father, child);
        node_add_child(&base, &child);
    }
    else if(base != NULL && child == NULL)
    {
        child = node_create(childStr);
        node_add_child(&base, &child);
    }
    else if(base == NULL && child != NULL)
    {
        base = node_create(baseStr);
        Node* new_father = node_find_father((*tree)->root, child->element);
        node_remove_child(&new_father, child);
        node_add_child(&new_father, &base);
        node_add_child(&base, &child);
    }
}

uint32_t node_get_distance(Node *current, const char* to_find)
{
    uint32_t to_return = 100000;
    if(strcmp(current->element, to_find) == 0)
    {
        return 0;
    }
    else
    {
        int ret = 0;
        for(uint32_t i = 0; i < current->childrenSize; i++)
        {
            ret = 1 + node_get_distance(current->children[i], to_find);
            if( ret < to_return)
            {
                to_return = ret;
            }
        }
    }
    return to_return;
}

uint32_t node_get_minimum_distance(Node *current, const char* origin, const char* dest, uint32_t min)
{
    uint32_t a, b, current_min;

    for(uint32_t i = 0; i < current->childrenSize; i++)
    {
        a = node_get_distance(current->children[i], "SAN");
        b = node_get_distance(current->children[i], "YOU");
        current_min = a+b;

        if(current_min < min)
            min = current_min;

        min = node_get_minimum_distance(current->children[i], origin, dest, min);
    }
    return min;
}

uint32_t tree_get_minumum_transfers(Tree *tree, const char* origin, const char* dest)
{
    uint32_t initial_min= 1000000;
    return node_get_minimum_distance(tree->root, origin, dest, initial_min) - 2; // -2 since we start from ROOT
}

uint32_t node_get_orbits(Node* current, uint32_t value)
{
    uint32_t sum = 0;
    for(uint32_t i = 0; i < current->childrenSize; i++)
    {
        sum += node_get_orbits(current->children[i], value + 1);
    }
    return value + sum;
}

uint32_t tree_get_orbits(Tree* current)
{
    uint32_t sum = 0;
    for(uint32_t i = 0; i < current->root->childrenSize ; i++)
    {
        sum += node_get_orbits(current->root->children[i], 0);
    }
    return sum;
}