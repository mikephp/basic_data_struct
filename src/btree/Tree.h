#include"stdlib.h"
#include"malloc.h"
#define BT BTreeNode
#define MS 10
typedef struct BT{
	char data;
	struct BT *left;
	struct BT *right;
}BT;