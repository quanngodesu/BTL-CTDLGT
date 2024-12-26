#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Định nghĩa cấu trúc sự kiện
typedef struct Event {
    int eventId;
    char description[100];
    int priority;  // Mức độ ưu tiên (số càng nhỏ, mức độ ưu tiên càng cao)
    time_t eventTime;  // Thời gian của sự kiện
    struct Event *next;
} Event;

// Queue (Hàng đợi)
typedef struct Queue {
    Event *front, *rear;
} Queue;

// Cấu trúc của cây AVL (Balanced Binary Search Tree)
typedef struct AVLNode {
    Event *event;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

// Hàm tạo sự kiện mới
Event* createEvent(int eventId, const char *description, int priority, time_t eventTime) {
    Event *newEvent = (Event *)malloc(sizeof(Event));
    newEvent->eventId = eventId;
    snprintf(newEvent->description, 100, "%s", description);
    newEvent->priority = priority;
    newEvent->eventTime = eventTime;
    newEvent->next = NULL;
    return newEvent;
}

// Hàm tạo Queue
Queue* createQueue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Hàm thêm sự kiện vào Queue
void enqueue(Queue *queue, Event *newEvent) {
    if (queue->rear == NULL) {
        queue->front = queue->rear = newEvent;
        return;
    }
    queue->rear->next = newEvent;
    queue->rear = newEvent;
}

// Hàm lấy sự kiện ra khỏi Queue
Event* dequeue(Queue *queue) {
    if (queue->front == NULL) {
        return NULL;
    }
    Event *temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    return temp;
}

// Hàm tạo một node của cây AVL
AVLNode* createAVLNode(Event *event) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    node->event = event;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Hàm tính chiều cao của một node
int height(AVLNode *node) {
    return (node == NULL) ? 0 : node->height;
}

// Hàm tính độ cân bằng của một node
int balanceFactor(AVLNode *node) {
    return (node == NULL) ? 0 : height(node->left) - height(node->right);
}

// Hàm quay phải (Right Rotation)
AVLNode* rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + ((height(y->left) > height(y->right)) ? height(y->left) : height(y->right));
    x->height = 1 + ((height(x->left) > height(x->right)) ? height(x->left) : height(x->right));

    return x;
}

// Hàm quay trái (Left Rotation)
AVLNode* leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + ((height(x->left) > height(x->right)) ? height(x->left) : height(x->right));
    y->height = 1 + ((height(y->left) > height(y->right)) ? height(y->left) : height(y->right));

    return y;
}

// Hàm cân bằng cây AVL
AVLNode* balanceAVLNode(AVLNode *node, Event *event) {
    int balance = balanceFactor(node);

    // Nếu cây lệch trái
    if (balance > 1 && event->priority < node->left->event->priority) {
        return rightRotate(node);
    }

    // Nếu cây lệch phải
    if (balance < -1 && event->priority > node->right->event->priority) {
        return leftRotate(node);
    }

    // Nếu lệch trái phải
    if (balance > 1 && event->priority > node->left->event->priority) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Nếu lệch phải trái
    if (balance < -1 && event->priority < node->right->event->priority) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Hàm chèn một sự kiện vào cây AVL
AVLNode* insertAVL(AVLNode *node, Event *event) {
    if (node == NULL) {
        return createAVLNode(event);
    }

    if (event->priority < node->event->priority) {
        node->left = insertAVL(node->left, event);
    } else {
        node->right = insertAVL(node->right, event);
    }

    node->height = 1 + ((height(node->left) > height(node->right)) ? height(node->left) : height(node->right));

    return balanceAVLNode(node, event);
}

// Hàm duyệt cây AVL để in các sự kiện
void inOrderAVL(AVLNode *root) {
    if (root != NULL) {
        inOrderAVL(root->left);
        printf("Event ID: %d, Description: %s, Priority: %d, Time: %s",
               root->event->eventId, root->event->description, root->event->priority, ctime(&root->event->eventTime));
        inOrderAVL(root->right);
    }
}

// Hàm hiển thị menu và xử lý lựa chọn của người dùng
void displayMenu() {
    printf("\n=====================================\n");
    printf(" 1. Thêm sự kiện\n");
    printf(" 2. Sửa sự kiện\n");
    printf(" 3. Xóa sự kiện\n");
    printf(" 4. Xem lịch sự kiện\n");
    printf(" 5. Nhắc nhở sự kiện\n");
    printf(" 6. Thoát\n");
    printf("=====================================\n");
    printf("Chọn lựa chọn (1-6): ");
}

int main() {
    // Tạo Queue và cây AVL
    Queue *queue = createQueue();
    AVLNode *avlTree = NULL;

    int choice;
    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                // Thêm sự kiện
                time_t eventTime;
                time(&eventTime);
                eventTime += 10; // Giả sử sự kiện sau 10 giây

                Event *event = createEvent(1, "Test Event", 2, eventTime);
                enqueue(queue, event);
                avlTree = insertAVL(avlTree, event);
                printf("Sự kiện đã được thêm vào hệ thống.\n");
                break;
            }
            case 2: {
                // Sửa sự kiện (Thêm logic sửa ở đây)
                printf("Sửa sự kiện chưa được triển khai.\n");
                break;
            }
            case 3: {
                // Xóa sự kiện (Thêm logic xóa ở đây)
                printf("Xóa sự kiện chưa được triển khai.\n");
                break;
            }
            case 4: {
                // Xem lịch sự kiện
                printf("Lịch sự kiện:\n");
                inOrderAVL(avlTree);
                break;
            }
            case 5: {
                // Nhắc nhở sự kiện
                reminder(queue, avlTree);
                break;
            }
            case 6:
                printf("Thoát chương trình.\n");
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }
    } while (choice != 6);

    return 0;
}
