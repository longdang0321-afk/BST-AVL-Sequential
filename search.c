#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"

/* ===== 공통 트리 함수 ===== */

/* 실제 실행한 데이터 비교 연산(==, <)을 각각 한 번으로 센다. */
int compareData(int key, int data, long *comparisons)
{
    (*comparisons)++;
    if (key == data)
        return 0;
    (*comparisons)++;
    if (key < data)
        return -1;
    return 1;
}

Node *createNode(int key)
{
    Node *node = malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->data = key;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

int maxInt(int a, int b)
{
    return a > b ? a : b;
}

/* BST와 AVL의 탐색 알고리즘은 같다. */
int treeSearch(const Node *root, int key, long *comparisons)
{
    while (root != NULL) {
        int result = compareData(key, root->data, comparisons);
        if (result == 0)
            return 1;
        root = result < 0 ? root->left : root->right;
    }
    return 0;
}

void freeTree(Node *root)
{
    if (root == NULL)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* ===== 배열 삽입 및 순차 탐색 ===== */

int sequentialSearch(const int array[], int length, int key, long *comparisons)
{
    for (int i = 0; i < length; i++) {
        (*comparisons)++;
        if (key == array[i])
            return 1;
    }
    return 0;
}

void insertArray(int array[], int *length, int key, long *comparisons)
{
    if (!sequentialSearch(array, *length, key, comparisons))
        array[(*length)++] = key;
}

/* ===== BST 삽입 및 높이 ===== */

/* BST는 출력할 때 재귀적으로 실제 높이를 구한다. */
int bstHeight(const Node *root)
{
    if (root == NULL)
        return 0;
    return 1 + maxInt(bstHeight(root->left), bstHeight(root->right));
}

Node *insertBST(Node *root, int key, long *comparisons)
{
    if (root == NULL)
        return createNode(key);

    int result = compareData(key, root->data, comparisons);
    if (result < 0)
        root->left = insertBST(root->left, key, comparisons);
    else if (result > 0)
        root->right = insertBST(root->right, key, comparisons);
    /* 같은 값이면 현재 노드를 그대로 반환한다. */
    return root;
}

/* ===== AVL 삽입 및 회전 ===== */

/* AVL 노드에 저장한 높이: 빈 트리 0, 루트만 있으면 1. */
int avlHeight(const Node *root)
{
    return root == NULL ? 0 : root->height;
}

static void updateHeight(Node *root)
{
    root->height = 1 + maxInt(avlHeight(root->left), avlHeight(root->right));
}

static int balanceFactor(const Node *root)
{
    return root == NULL ? 0 : avlHeight(root->left) - avlHeight(root->right);
}

static Node *rotateRight(Node *root)
{
    Node *newRoot = root->left;
    Node *middle = newRoot->right;
    newRoot->right = root;
    root->left = middle;
    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

static Node *rotateLeft(Node *root)
{
    Node *newRoot = root->right;
    Node *middle = newRoot->left;
    newRoot->left = root;
    root->right = middle;
    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

Node *insertAVL(Node *root, int key, long *comparisons)
{
    if (root == NULL)
        return createNode(key);

    int result = compareData(key, root->data, comparisons);
    if (result < 0)
        root->left = insertAVL(root->left, key, comparisons);
    else if (result > 0)
        root->right = insertAVL(root->right, key, comparisons);
    else
        return root;

    /* 높이 및 균형 검사는 데이터 비교 횟수에 포함하지 않는다. */
    updateHeight(root);
    int balance = balanceFactor(root);
    if (balance > 1) {
        if (balanceFactor(root->left) < 0)       /* LR */
            root->left = rotateLeft(root->left);
        return rotateRight(root);              /* LL 또는 LR의 두 번째 회전 */
    }
    if (balance < -1) {
        if (balanceFactor(root->right) > 0)     /* RL */
            root->right = rotateRight(root->right);
        return rotateLeft(root);               /* RR 또는 RL의 두 번째 회전 */
    }
    return root;
}

/* ===== 실험 진행 및 결과 출력 ===== */

static void printValues(const int values[], int count)
{
    for (int i = 0; i < count; i++) {
        printf("%4d", values[i]);
        if ((i + 1) % 10 == 0 || i + 1 == count)
            printf("\n");
        else
            printf(" ");
    }
}

/* 천 단위 구분 쉼표를 넣어 문자열로 변환한다. (%'ld는 Windows에서 지원이 불확실하다) */
static void formatWithCommas(long value, char *out)
{
    char digits[32];
    int len, pos = 0;

    snprintf(digits, sizeof(digits), "%ld", value < 0 ? -value : value);
    len = (int)strlen(digits);
    if (value < 0)
        out[pos++] = '-';
    for (int i = 0; i < len; i++) {
        out[pos++] = digits[i];
        if ((len - i - 1) % 3 == 0 && i != len - 1)
            out[pos++] = ',';
    }
    out[pos] = '\0';
}

static void printConstruction(const char *name, long count)
{
    char buf[32];
    formatWithCommas(count, buf);
    printf("%s : %5s\n", name, buf);
}

static void printSearchResult(const char *name, int found, long comparisons)
{
    printf("  %-17s Result: %-9s Comparisons: %ld\n",
           name, found ? "Found" : "Not Found", comparisons);
}

static void printSearchSummary(const char *name, long total)
{
    char buf[32];
    formatWithCommas(total, buf);
    printf("%s\n", name);
    printf("  Total comparisons   : %s\n", buf);
    printf("  Average comparisons : %.2f\n", (double)total / SEARCH_COUNT);
}

void runExperiment(void)
{
    int generated[INSERT_COUNT], keys[SEARCH_COUNT];
    int array[INSERT_COUNT], length = 0;
    Node *bst = NULL, *avl = NULL;
    long arrayBuild = 0, bstBuild = 0, avlBuild = 0;
    long arrayTotal = 0, bstTotal = 0, avlTotal = 0;

    /* 난수 하나를 세 자료구조에 동일한 순서로 처리한다. */
    for (int i = 0; i < INSERT_COUNT; i++) {
        generated[i] = rand() % (MAX_VALUE + 1);
        insertArray(array, &length, generated[i], &arrayBuild);
        bst = insertBST(bst, generated[i], &bstBuild);
        avl = insertAVL(avl, generated[i], &avlBuild);
    }

    printf("\nGenerated values (%d):\n", INSERT_COUNT);
    printValues(generated, INSERT_COUNT);
    printf("\nStored values : %d\n", length);
    printf("Duplicates skipped : %d\n", INSERT_COUNT - length);
    printf("\nConstruction\n");
    printConstruction("Array comparisons", arrayBuild);
    printConstruction("BST comparisons  ", bstBuild);
    printConstruction("AVL comparisons  ", avlBuild);
    printf("\nStructure\n");
    printf("Array length : %d\n", length);
    printf("BST height   : %d\n", bstHeight(bst));
    printf("AVL height   : %d\n", avlHeight(avl));

    /* 삽입을 모두 마친 뒤 탐색용 난수를 추가로 생성한다. */
    for (int i = 0; i < SEARCH_COUNT; i++)
        keys[i] = rand() % (MAX_VALUE + 1);
    printf("\nSearch keys (%d):\n", SEARCH_COUNT);
    printValues(keys, SEARCH_COUNT);

    for (int i = 0; i < SEARCH_COUNT; i++) {
        long arrayCount = 0, bstCount = 0, avlCount = 0;
        int arrayFound = sequentialSearch(array, length, keys[i], &arrayCount);
        int bstFound = treeSearch(bst, keys[i], &bstCount);
        int avlFound = treeSearch(avl, keys[i], &avlCount);

        printf("\nSearch Key : %d\n", keys[i]);
        printSearchResult("Sequential Search", arrayFound, arrayCount);
        printSearchResult("BST Search", bstFound, bstCount);
        printSearchResult("AVL Search", avlFound, avlCount);
        arrayTotal += arrayCount;
        bstTotal += bstCount;
        avlTotal += avlCount;
    }

    printf("\nSearches : %d\n", SEARCH_COUNT);
    printSearchSummary("Sequential Search", arrayTotal);
    printSearchSummary("BST Search", bstTotal);
    printSearchSummary("AVL Search", avlTotal);
    freeTree(bst);
    freeTree(avl);
}
