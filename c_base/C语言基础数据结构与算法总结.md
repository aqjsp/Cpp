# C语言基础数据结构与算法

## 1、数组

### 1.1、基本原理

在C语言中，数组是一种线性数据结构，由相同数据类型的元素按一定顺序排列而成。它们在内存中被分配成一个连续的块，并通过下标来访问各个元素。

数组可以用一维、二维或多维方式定义。一维数组由一个有限元素序列组成，可以通过索引访问其中的元素；二维数组由多个一维数组组成，每个一维数组表示二维数组中的一行或一列；多维数组则是由多个二维数组或一维数组组成的数据结构。

在C语言中，可以使用以下语法来定义一个数组：

```C
datatype array_name[array_size];
```

其中，`datatype` 表示数组中元素的数据类型，`array_name` 是数组的名称，`array_size` 表示数组中元素的数量。

例如，下面是一个包含5个整数的一维数组的定义：

```C
int numbers[5];
```

可以使用下标来访问数组中的元素，下标从0开始。例如，访问数组中第一个元素的语法如下：

```C
numbers[0] = 10;
```

这将把值10存储在数组的第一个元素中。同样，可以使用循环来遍历整个数组并访问其中的每个元素。

### 1.2、实现

数组的实现依赖于底层的内存分配和访问方式。由于数组中的元素在内存中是连续存储的，因此可以通过指针对数组进行操作。数组名是数组第一个元素的地址，因此可以使用指针来访问数组中的元素。

以下是一个简单的数组实现示例，展示了如何创建数组、对数组进行赋值和访问

```C
#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5};  // 创建一个大小为5的整型数组并初始化
    int i;

    for (i = 0; i < 5; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);  // 访问数组元素并打印
    }

    return 0;
}
```

输出：

```C
arr[0] = 1
arr[1] = 2
arr[2] = 3
arr[3] = 4
arr[4] = 5
```

以上代码创建了一个大小为5的整型数组，并初始化为1到5的连续整数。然后通过for循环依次访问数组的每个元素并打印输出。

## 2、链表

### 2.1、基本原理

链表是一种线性数据结构，它由若干个节点构成，每个节点包含两个部分：数据域和指针域。数据域用于存储节点的数据，指针域用于指向下一个节点。链表中的节点不一定是连续存储的，它们通过指针连接在一起，因此链表具有动态性和灵活性。链表的特点是可以快速插入和删除元素，但查找操作需要遍历整个链表。

### 2.2、实现

链表有多种实现方式，常见的有单向链表、双向链表和循环链表。下面以单向链表为例，介绍链表的实现方法。

定义链表节点结构体：

```C
typedef struct ListNode {
    int val;            // 数据元素struct ListNode* next;   // 指向下一个节点的指针
} ListNode;
```

定义链表结构体：

```C
typedef struct LinkedList {
    ListNode* head;     // 头节点指针int size;           // 链表长度
} LinkedList;
```

链表的基本操作包括插入、删除和查找。

插入操作：

```C
void insert(LinkedList* list, int val, int index) {
    // 新建节点
    ListNode* new_node = (ListNode*) malloc(sizeof(ListNode));
    new_node->val = val;
    new_node->next = NULL;

    // 插入节点
    if (index <= 0) {
        new_node->next = list->head;
        list->head = new_node;
    } else {
        ListNode* cur = list->head;
        for (int i = 0; i < index - 1 && cur != NULL; i++) {
            cur = cur->next;
        }
        if (cur != NULL) {
            new_node->next = cur->next;
            cur->next = new_node;
        }
    }

    list->size++;
}
```

删除操作：

```C
void delete(LinkedList* list, int index) {
    if (list->head == NULL) {
        return;
    }

    // 删除头节点if (index == 0) {
        ListNode* cur = list->head;
        list->head = cur->next;
        free(cur);
    } else {
        ListNode* cur = list->head;
        for (int i = 0; i < index - 1 && cur != NULL; i++) {
            cur = cur->next;
        }
        if (cur != NULL && cur->next != NULL) {
            ListNode* tmp = cur->next;
            cur->next = tmp->next;
            free(tmp);
        }
    }

    list->size--;
}
```

查找操作：

```C
int find(LinkedList* list, int val) {
    ListNode* cur = list->head;
    int index = 0;
    while (cur != NULL && cur->val != val) {
        cur = cur->next;
        index++;
    }
    if (cur != NULL) {
        return index;
    } else {
        return -1;
    }
}
```

这是一个简单的单向链表的实现，还有很多细节需要考虑，例如链表的初始化、销毁等操作，具体实现可以根据需求进行扩展和修改。

## 3、栈（Stack）

### 3.1、基本原理

栈是一种线性数据结构，具有后进先出的特点，可以理解为一种特殊的数组，只能在栈顶进行插入和删除操作。

### 3.2、实现

**数组实现栈**

数组实现栈需要定义一个数组和一个变量来表示栈顶的位置。栈顶的位置初始值为-1，表示栈为空。每次入栈时，将栈顶位置加1，并将要入栈的元素存储在栈顶位置上；每次出栈时，将栈顶位置减1，并返回栈顶元素。如果栈顶位置小于0，则表示栈为空。

以下是一个简单的数组实现栈的示例代码：

```C
#define MAX_SIZE 10 // 定义栈的最大容量

int stack[MAX_SIZE]; // 定义栈的数组
int top = -1; // 定义栈顶位置

// 判断栈是否为空
int is_empty() {
    return top < 0;
}

// 判断栈是否已满
int is_full() {
    return top >= MAX_SIZE - 1;
}

// 入栈
void push(int value) {
    if (is_full()) {
        printf("Stack overflow!\n");
        return;
    }
    stack[++top] = value;
}

// 出栈
int pop() {
    if (is_empty()) {
        printf("Stack underflow!\n");
        return -1;
    }
    return stack[top--];
}

// 获取栈顶元素
int get_top() {
    if (is_empty()) {
        printf("Stack is empty!\n");
        return -1;
    }
    return stack[top];
}
```

**链表实现栈**

链表实现栈需要定义一个链表的头结点，每次入栈时将新的元素插入到链表的头部，每次出栈时删除链表头部的元素。头结点的next指针指向链表的第一个元素，表示栈顶的位置。如果头结点的next指针为空，则表示栈为空。

以下是一个简单的链表实现栈的示例代码：

```C
typedef struct node {
    int data;
    struct node* next;
} Node;

Node* top = NULL; // 定义栈顶指针

// 判断栈是否为空
int is_empty() {
    return top == NULL;
}

// 入栈
void push(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = top;
    top = new_node;
}

// 出栈
int pop() {
    if (is_empty()) {
        printf("Stack underflow!\n");
        return -1;
    }
    Node* temp = top;
    int value = temp->data;
    top = top->next;
    free(temp);
    return value;
}

// 获取栈顶元素
int get_top() {
    if (is_empty()) {
        printf("Stack is empty!\n");
        return -1;
    }
    return top->data;
}
```

## **4、**队列

### 4.1、基本原理

队列是一种先进先出（First-In-First-Out，FIFO）的线性数据结构，它可以在队尾插入元素，在队头删除元素。队列具有“先进先出”的特点，就像我们平时排队一样，先来的先离开，后来的后离开。

### 4.2、实现

**数组实现**

C语言中可以通过结构体来实现队列。一个队列需要两个指针，一个指向队头，一个指向队尾，以及一个数组来存储队列中的元素。队头指针指向队列中的第一个元素，队尾指针指向队列中的最后一个元素。在插入元素时，先将元素插入到队尾指针指向的位置，然后将队尾指针后移一位。在删除元素时，先将队头指针后移一位，然后返回队头指针指向的元素。

以下是C语言实现队列的示例代码：

```C
#define MAX_QUEUE_SIZE 100

typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front;  // 队首指针
    int rear;   // 队尾指针
} Queue;

// 初始化队列
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

// 判断队列是否为空
int isQueueEmpty(Queue *q) {
    return q->front == q->rear;
}

// 判断队列是否已满
int isQueueFull(Queue *q) {
    return q->rear == MAX_QUEUE_SIZE;
}

// 入队
void enqueue(Queue *q, int x) {
    if (isQueueFull(q)) {
        printf("Queue is full.\n");
        return;
    }
    q->data[q->rear++] = x;
}

// 出队
int dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty.\n");
        return -1;
    }
    int x = q->data[q->front++];
    return x;
}
```

以上代码中，`Queue`结构体包含一个整型数组`data`，以及两个指针`front`和`rear`，分别指向队首和队尾。`initQueue`函数用于初始化队列，将队首和队尾指针都指向数组的第一个元素。`isQueueEmpty`和`isQueueFull`函数分别用于判断队列是否为空和已满。`enqueue`函数用于入队，将元素插入到队列的队尾，同时更新队尾指针。`dequeue`函数用于出队，从队列的队首取出元素，同时更新队首指针。需要注意的是，出队时需要判断队列是否为空，如果为空则返回-1。

**链表实现**

```C
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

// 初始化队列
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

// 判断队列是否为空
int isEmpty(Queue* q) {
    return q->front == NULL;
}

// 入队
void enqueue(Queue* q, int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// 出队
int dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty\n");
        return -1;
    }

    Node* temp = q->front;
    int data = temp->data;

    if (q->front == q->rear) {
        q->front = q->rear = NULL;
    } else {
        q->front = q->front->next;
    }

    free(temp);
    return data;
}

// 打印队列
void printQueue(Queue* q) {
    if (isEmpty(q)) {
        printf("Empty queue\n");
        return;
    }

    Node* temp = q->front;

    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }

    printf("\n");
}

int main() {
    Queue q;
    initQueue(&q);

    enqueue(&q, 1);
    enqueue(&q, 2);
    enqueue(&q, 3);

    printQueue(&q);

    dequeue(&q);
    printQueue(&q);

    dequeue(&q);
    printQueue(&q);

    dequeue(&q);
    printQueue(&q);

    dequeue(&q);

    return 0;
}
```

这个队列的实现基于链表，有 `Node` 和 `Queue` 两个结构体。其中 `Node` 表示链表中的一个节点，包含数据和指向下一个节点的指针；`Queue` 表示队列，包含指向队首和队尾节点的指针。

队列的初始化操作 `initQueue` 会将队列的头指针和尾指针都设置为 `NULL`。

入队操作 `enqueue` 会新建一个节点，将数据存入节点中，并将节点加入到队列的尾部。如果队列为空，头指针和尾指针都指向新节点。

出队操作 `dequeue` 会将队列的头节点取出来，返回节点中的数据，并将头指针指向下一个节点。如果队列为空，则会输出错误信息并返回 `-1`。

打印队列的操作 `printQueue` 会依次遍历链表中的节点，输出节点中的数据。如果队列为空，则输出 "Empty queue"。

## 5、二叉树

### 5.1、基本原理

二叉树是一种常见的数据结构，它由一个根节点以及左右两个子树组成，每个子树也是一个二叉树。二叉树中的每个节点最多有两个子节点，左子节点和右子节点。它们的顺序是有意义的，即左子节点小于父节点，右子节点大于父节点。

### 5.2、实现

在C语言中，可以使用结构体和指针来实现二叉树。二叉树结构体通常包含三个成员变量：值、左子节点和右子节点。

以下是一个二叉树的结构体定义及其基本操作的实现。

```C
// 二叉树结构体定义
typedef struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 创建一个新节点
TreeNode* createNode(int val) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->val = val;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 在二叉搜索树中插入节点
TreeNode* insertNode(TreeNode* root, int val) {
    if (root == NULL) {
        return createNode(val);
    }
    if (val < root->val) {
        root->left = insertNode(root->left, val);
    } else {
        root->right = insertNode(root->right, val);
    }
    return root;
}

// 在二叉树中查找节点
TreeNode* findNode(TreeNode* root, int val) {
    if (root == NULL) {
        return NULL;
    }
    if (val == root->val) {
        return root;
    }
    if (val < root->val) {
        return findNode(root->left, val);
    } else {
        return findNode(root->right, val);
    }
}

// 删除二叉搜索树中的节点
TreeNode* deleteNode(TreeNode* root, int val) {
    if (root == NULL) {
        return NULL;
    }
    if (val < root->val) {
        root->left = deleteNode(root->left, val);
    } else if (val > root->val) {
        root->right = deleteNode(root->right, val);
    } else {
        // 当前节点就是要删除的节点
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        } else {
            // 当前节点有两个子节点
            TreeNode* temp = root->right;
            while (temp->left != NULL) {
                temp = temp->left;
            }
            root->val = temp->val;
            root->right = deleteNode(root->right, temp->val);
        }
    }
    return root;
}
```

二叉树的遍历可以分为三种：前序遍历、中序遍历和后序遍历。其中，前序遍历的遍历顺序是先遍历根节点，然后遍历左子树，最后遍历右子树；中序遍历的遍历顺序是先遍历左子树，然后遍历根节点，最后遍历右子树；后序遍历的遍历顺序是先遍历左子树，然后遍历右子树，最后遍历根节点。

#### 递归实现：

```C
#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

// 前序遍历
void preorderTraversal(struct TreeNode* root){
    if(root == NULL) return;
    printf("%d ", root->val);
    preorderTraversal(root->left);
    preorderTraversal(root->right);
}

// 中序遍历
void inorderTraversal(struct TreeNode* root){
    if(root == NULL) return;
    inorderTraversal(root->left);
    printf("%d ", root->val);
    inorderTraversal(root->right);
}

// 后序遍历
void postorderTraversal(struct TreeNode* root){
    if(root == NULL) return;
    postorderTraversal(root->left);
    postorderTraversal(root->right);
    printf("%d ", root->val);
}

int main() {
    // 构建一个二叉树
    struct TreeNode* root = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->val = 1;
    root->left = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->left->val = 2;
    root->right = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->right->val = 3;
    root->left->left = NULL;
    root->left->right = NULL;
    root->right->left = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->right->left->val = 4;
    root->right->right = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->right->right->val = 5;
    root->right->left->left = NULL;
    root->right->left->right = NULL;
    root->right->right->left = NULL;
    root->right->right->right = NULL;
    
    printf("前序遍历：");
    preorderTraversal(root);
    printf("\n中序遍历：");
    inorderTraversal(root);
    printf("\n后序遍历：");
    postorderTraversal(root);
    printf("\n");
    
    return 0;
}
```

以上代码中，我们定义了一个`TreeNode`结构体，表示二叉树的节点。其中，`val`表示节点的值，`left`和`right`分别表示该节点的左右子节点。我们通过递归的方式实现了三种遍历方式，分别为前序遍历、中序遍历和后序遍历。最后，我们在`main`函数中构建了一个二叉树，并对其进行了三种遍历，输出了结果。

#### 非递归实现：

先序遍历：

```C
void pre_order_non_recursive(Node* root) {
    if (root == NULL) {
        return;
    }
    stack<Node*> s;
    s.push(root);
    while (!s.empty()) {
        Node* cur = s.top();
        s.pop();
        printf("%d ", cur->data);
        if (cur->right != NULL) {
            s.push(cur->right);
        }
        if (cur->left != NULL) {
            s.push(cur->left);
        }
    }
}
```

中序遍历：

```C
void in_order_non_recursive(Node* root) {
    stack<Node*> s;
    Node* cur = root;
    while (cur != NULL || !s.empty()) {
        while (cur != NULL) {
            s.push(cur);
            cur = cur->left;
        }
        cur = s.top();
        s.pop();
        printf("%d ", cur->data);
        cur = cur->right;
    }
}
```

后序遍历：

```C
void post_order_non_recursive(Node* root) {
    if (root == NULL) {
        return;
    }
    stack<Node*> s;
    s.push(root);
    Node* last_visited = NULL;
    while (!s.empty()) {
        Node* cur = s.top();
        if ((cur->left == NULL && cur->right == NULL) || (last_visited != NULL && (last_visited == cur->left || last_visited == cur->right))) {
            printf("%d ", cur->data);
            s.pop();
            last_visited = cur;
        } else {
            if (cur->right != NULL) {
                s.push(cur->right);
            }
            if (cur->left != NULL) {
                s.push(cur->left);
            }
        }
    }
}
```

这些非递归实现的代码利用了栈的特性，将待访问节点放入栈中，按照一定的顺序遍历整个树。在遍历的过程中，通过判断栈顶元素的左右子节点是否被访问过，来决定是否访问该节点，以此达到遍历整个树的目的。

## 6、哈希表

### 6.1、基本原理

哈希表是一种基于哈希函数实现的数据结构，它可以支持高效的插入、查找和删除操作。它的基本原理是将数据映射到哈希表的槽位（slot）中，这个映射过程是通过哈希函数实现的。哈希函数将数据映射到槽位的过程应该是快速且随机的，这样可以尽量避免哈希冲突（即不同的数据映射到了同一个槽位中）。

### 6.2、实现

哈希表的实现一般有两种方式：拉链法和开放地址法。

- 拉链法：将哈希表的每个槽位都作为链表的头节点，哈希函数将数据映射到的槽位上的数据会被插入到对应链表中。这种方法处理哈希冲突的方式是将冲突的数据插入到链表中。插入和查找的时间复杂度都是 O(1)，但由于链表的缘故，需要额外的空间存储链表节点。
- 开放地址法：当哈希冲突发生时，哈希表的某个槽位已经被占用，此时可以向后探查其他槽位，直到找到空闲的槽位为止。这种方法处理哈希冲突的方式是在哈希表中寻找另一个可用的槽位。插入和查找的时间复杂度都是 O(1)，但是在哈希表的负载因子比较大的情况下，探查槽位的时间可能会比较长。

下面是一个简单的拉链法哈希表的实现示例：

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 1000

/* 哈希表的节点结构 */
struct Node {
    char key[20];  // 存储键值
    int value;     // 存储值
    struct Node *next;  // 指向下一个节点的指针
};

/* 哈希表结构 */
struct HashTable {
    struct Node *table[MAX_TABLE_SIZE];  // 哈希表
    int size;                            // 哈希表中节点数目
};

/* 初始化哈希表 */
void initHashTable(struct HashTable *hashTable) {
    int i;
    for (i = 0; i < MAX_TABLE_SIZE; i++) {
        hashTable->table[i] = NULL;
    }
    hashTable->size = 0;
}

/* 计算哈希值 */
int hash(char *key) {
    int hash = 0;
    while (*key) {
        hash += *key++;
    }
    return hash % MAX_TABLE_SIZE;
}

/* 添加节点 */
void put(struct HashTable *hashTable, char *key, int value) {
    int index = hash(key);
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    strcpy(node->key, key);
    node->value = value;
    node->next = hashTable->table[index];
    hashTable->table[index] = node;
    hashTable->size++;
}

/* 查找节点 */
struct Node *get(struct HashTable *hashTable, char *key) {
    int index = hash(key);
    struct Node *node = hashTable->table[index];
    while (node != NULL && strcmp(node->key, key) != 0) {
        node = node->next;
    }
    return node;
}

/* 删除节点 */
void removeNode(struct HashTable *hashTable, char *key) {
    int index = hash(key);
    struct Node *prev = NULL;
    struct Node *node = hashTable->table[index];
    while (node != NULL && strcmp(node->key, key) != 0) {
        prev = node;
        node = node->next;
    }
    if (node == NULL) {
        return;
    }
    if (prev == NULL) {
        hashTable->table[index] = node->next;
    } else {
        prev->next = node->next;
    }
    free(node);
    hashTable->size--;
}

/* 打印哈希表 */
void printHashTable(struct HashTable *hashTable) {
    int i;
    for (i = 0; i < MAX_TABLE_SIZE; i++) {
        printf("%d: ", i);
        struct Node *node = hashTable->table[i];
        while (node != NULL) {
            printf("(%s, %d) ", node->key, node->value);
            node = node->next;
        }
        printf("\n");
    }
}
```

## 7、汉诺塔：

### 7.1、递归实现

```C++
#include <stdio.h>

void hanoi(int n, char A, char B, char C)
{
    if (n == 1) {
        printf("Move disk %d from %c to %c\n", n, A, C);
    } else {
        hanoi(n - 1, A, C, B);
        printf("Move disk %d from %c to %c\n", n, A, C);
        hanoi(n - 1, B, A, C);
    }
}

int main()
{
    int n = 3;
    hanoi(n, 'A', 'B', 'C');
    return 0;
}
```

其中，`hanoi`函数表示汉诺塔的移动过程，`n`表示当前移动的盘子数量，`A`、`B`、`C`表示三根柱子的名称。当只有一个盘子时，直接将其从A柱子移动到C柱子即可；否则，将前`n-1`个盘子从A柱子移动到B柱子，然后将第`n`个盘子从A柱子移动到C柱子，最后将前`n-1`个盘子从B柱子移动到C柱子。

### 7.2、非递归实现

```C++
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct {
    int disk;  // 当前盘子大小
    int from;  // 当前所在柱子
    int to;    // 目标柱子
    int aux;   // 辅助柱子
} Move;

typedef struct {
    Move data[MAX_SIZE];
    int top;
} Stack;

void initStack(Stack *s) {
    s->top = -1;
}

int isEmpty(Stack *s) {
    return s->top == -1;
}

int isFull(Stack *s) {
    return s->top == MAX_SIZE - 1;
}

int push(Stack *s, Move item) {
    if (isFull(s)) {
        return 0;
    }
    s->data[++s->top] = item;
    return 1;
}

Move pop(Stack *s) {
    if (isEmpty(s)) {
        Move error = { -1, -1, -1, -1 };
        return error;
    }
    return s->data[s->top--];
}

void move(int disk, int from, int to, int aux) {
    Stack s;
    initStack(&s);
    Move firstMove = { disk, from, to, aux };
    push(&s, firstMove);
    while (!isEmpty(&s)) {
        Move currentMove = pop(&s);
        int currentDisk = currentMove.disk;
        int currentFrom = currentMove.from;
        int currentTo = currentMove.to;
        int currentAux = currentMove.aux;
        if (currentDisk == 1) {
            printf("Move disk %d from %d to %d\n", currentDisk, currentFrom, currentTo);
        } else {
            Move newMove1 = { currentDisk - 1, currentFrom, currentAux, currentTo };
            Move newMove2 = { 1, currentFrom, currentTo, currentAux };
            Move newMove3 = { currentDisk - 1, currentAux, currentTo, currentFrom };
            push(&s, newMove3);
            push(&s, newMove2);
            push(&s, newMove1);
        }
    }
}

int main() {
    int n = 3;
    int from = 1, to = 3, aux = 2;
    move(n, from, to, aux);
    return 0;
}
```

该程序的实现方式和递归实现类似，但是使用了栈来保存每一次移动的状态，并在栈不为空时不断地弹出栈顶元素进行移动。在移动过程中，如果当前盘子数为1，则直接输出移动步骤；否则，根据汉诺塔的规则，将大盘子移动到辅助柱子上，然后将最后一个盘子移动到目标柱子上，最后将所有的盘子从辅助柱子上移动到目标柱子上。

## 8、斐波那契数列

可以使用递归函数来实现斐波那契数列。递归函数是指在函数体内调用函数本身的函数。在实现斐波那契数列时，可以定义一个递归函数，该函数将调用自身两次以生成下一个数字，并在基本情况下返回1或0。

### 8.1、递归实现

```C++
#include <stdio.h>

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    int n, i;
    printf("Enter the number of terms: ");
    scanf("%d", &n);

    printf("Fibonacci Series: ");
    for (i = 0; i < n; i++) {
        printf("%d ", fibonacci(i));
    }
    return 0;
}
```

在此代码中，函数fibonacci()是递归函数，它将调用自身两次以生成下一个数字，并在n <= 1的情况下返回n。在主函数中，我们获取了要生成的斐波那契数列的项数，并使用for循环调用递归函数fibonacci()来生成数列。

### 8.2、非递归实现

```C++
#include <stdio.h>

int fibonacci(int n) {
    int f0 = 0, f1 = 1, fn = n;

    if (n <= 1) {
        return n;
    } else {
        for (int i = 2; i <= n; i++) {
            fn = f0 + f1;
            f0 = f1;
            f1 = fn;
        }
        return fn;
    }
}

int main() {
    int n = 10;
    printf("The %d-th fibonacci number is: %d\n", n, fibonacci(n));
    return 0;
}
```

代码的思路是利用一个循环，从第二项开始计算斐波那契数列的每一项，通过更新前两项的值来得到当前项的值，直到计算到第 n 项，最后返回第 n 项的值。该算法的时间复杂度为 O(n)，空间复杂度为 O(1)。

## 9、杨辉三角

### 9.1、递归实现

```C++
#include <stdio.h>

int pascal(int row, int col) {
    if (row == col || col == 0) { // 杨辉三角的左右两边都是1
        return 1;
    } else { // 杨辉三角的每个数等于它上方两个数之和
        return pascal(row - 1, col - 1) + pascal(row - 1, col);
    }
}

int main() {
    int n;
    printf("请输入杨辉三角的行数：");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            printf("%d ", pascal(i, j));
        }
        printf("\n");
    }
    return 0;
}
```

该程序首先输入一个正整数n表示要输出的杨辉三角的行数，然后通过两重循环遍历每一行的每一个位置，调用递归函数pascal计算出该位置上的数值，并将其输出到控制台上。递归函数pascal的实现采用了杨辉三角的定义，当当前位置处于杨辉三角的左右两边时，返回1，否则返回该位置上方两个数之和。

### 9.2、非递归实现

```C++
#include <stdio.h>

void printPascalTriangle(int rows) {
    int triangle[rows][rows];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j <= i; j++) {
            if (j == 0 || j == i) {
                triangle[i][j] = 1;
            } else {
                triangle[i][j] = triangle[i - 1][j - 1] + triangle[i - 1][j];
            }
            printf("%d ", triangle[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int rows;
    printf("Enter the number of rows: ");
    scanf("%d", &rows);
    printf("Pascal's Triangle:\n");
    printPascalTriangle(rows);
    return 0;
}
```

该算法通过二维数组存储杨辉三角的每一个元素，从而避免了递归算法中的重复计算。