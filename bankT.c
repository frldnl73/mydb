#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bptree.h"
#include "util.h"
#include <stdio.h>

/*
 * Key must be an integer at the beginning of the structure
 */
struct bank_table {
    int bank_abi;
    char bank_desc[20];
};

/*
 * Node Layout data elements
 */
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

static bool keyEl1_greater_keyEl2 (void* key1, void* key2);
static bool keyEl1_equal_keyEl2 (void* key1, void* key2);
static bool key1_greater_keyEl2 (void* key1, void* key2);
static bool key1_equal_keyEl2 (void* key1, void* key2);

static const int ABI_OFFSET  = 0;
static const int ABI_SIZE  = size_of_attribute(struct bank_table, bank_abi);
static const int KEY_SIZE  = ABI_SIZE;
static const int DATA_SIZE = size_of_attribute(struct bank_table, bank_desc);
static const int ELEMENT_SIZE = KEY_SIZE + DATA_SIZE;
static const short TREE_ORDER = 0;

static void* g_element;
static void* g_key1;
static void* g_key2;
static int g_table_id=0;

void bank_open_db(const char* filename, bool readOnly) {
    if (g_table_id>0) {
      printf("There is an open connection; open_db is not allowed\n");
      exit(EXIT_FAILURE);
    }

    g_element = (void*) malloc (ELEMENT_SIZE);
    g_key1 = (void*) malloc (KEY_SIZE);
    g_key2 = (void*) malloc (KEY_SIZE);

    if (TREE_ORDER == 0) {
        g_table_id = mybptree_openBTree(filename, ELEMENT_SIZE, mybptree_get_max_order(ELEMENT_SIZE, KEY_SIZE), KEY_SIZE, readOnly);
    } else {
        g_table_id = mybptree_openBTree(filename, ELEMENT_SIZE, TREE_ORDER, KEY_SIZE, readOnly);
    }

    mybptree_set_fn_key_test(g_table_id, &keyEl1_greater_keyEl2, &keyEl1_equal_keyEl2, &key1_greater_keyEl2, &key1_equal_keyEl2);
}

void bank_close_db() {
    mybptree_closeBTree(g_table_id);
    
    g_table_id=0;
    free(g_element);
    free(g_key1);
    free(g_key2);
    g_element=NULL;
    g_key1=NULL;
    g_key2=NULL;
}

int bank_insert(struct bank_table* data) {
    void* element = data;
    return mybptree_insertBTree (g_table_id, element);
}

int bank_erase(int abi) {
    memcpy (g_key1+ABI_OFFSET, &abi, ABI_SIZE);
 
    return mybptree_eraseBTree (g_table_id, g_key1, KEY_SIZE);
}

struct bank_table* bank_search_key (int abi) {
    memcpy (g_key1+ABI_OFFSET, &abi, ABI_SIZE);
    void* el = mybptree_search_key (g_table_id, g_key1, ABI_SIZE);
    if (el) {
        memcpy (g_element, el, ELEMENT_SIZE);
        return g_element;
    } else {
        return NULL;
    }
}

int bank_update (int abi, struct bank_table d) {
    memcpy (g_key1+ABI_OFFSET, &abi, ABI_SIZE);
    return mybptree_update_element (g_table_id, g_key1, KEY_SIZE, &d, ELEMENT_SIZE);
}

int bank_traverse (const char* filename) {
    if (TREE_ORDER == 0) {
        return myutil_traverseBTree(filename, ELEMENT_SIZE, myutil_get_max_order(ELEMENT_SIZE, KEY_SIZE), KEY_SIZE);
    } else {
        return myutil_traverseBTree(filename, ELEMENT_SIZE, TREE_ORDER, KEY_SIZE);
    }
}

bool keyEl1_greater_keyEl2 (void* key1, void* key2) {
    int abi1 = *(int*)(key1+ABI_OFFSET);
    int abi2 = *(int*)(key2+ABI_OFFSET);

    return (abi1>abi2);
}

bool keyEl1_equal_keyEl2 (void* key1, void* key2) {
    int abi1 = *(int*)(key1+ABI_OFFSET);
    int abi2 = *(int*)(key2+ABI_OFFSET);

    return (abi1 == abi2);    
}

bool key1_greater_keyEl2 (void* key1, void* key2) {
    int abi1 = *(int*)(key1+ABI_OFFSET);
    int abi2 = *(int*)(key2+ABI_OFFSET);

    return (abi1>abi2);
}

bool key1_equal_keyEl2 (void* key1, void* key2) {
    int abi1 = *(int*)(key1+ABI_OFFSET);
    int abi2 = *(int*)(key2+ABI_OFFSET);

    return (abi1 == abi2);    
}

