#ifndef SEARCH_H
#define SEARCH_H

#define INSERT_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000

/* BST와 AVL에서 공통으로 사용하는 노드 구조체 */
typedef struct Node {
    int data;
    int height;
    struct Node *left;
    struct Node *right;
} Node;

/* 순차 탐색과 배열 삽입 */
int sequentialSearch(const int array[], int length, int key, long *comparisons);
void insertArray(int array[], int *length, int key, long *comparisons);

/* 두 트리가 공통으로 사용하는 함수 */
int compareData(int key, int data, long *comparisons);
Node *createNode(int key);
int maxInt(int a, int b);
int treeSearch(const Node *root, int key, long *comparisons);
void freeTree(Node *root);

/* 일반 이진 탐색 트리 */
Node *insertBST(Node *root, int key, long *comparisons);
int bstHeight(const Node *root);

/* 균형 이진 탐색 트리 */
Node *insertAVL(Node *root, int key, long *comparisons);
int avlHeight(const Node *root);

/* 데이터 생성, 성능 측정 및 결과 출력 */
void runExperiment(void);

#endif
