typedef struct {
    char name[100];
    int priority;
} Event;

typedef struct {
    Event events[100];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

void push(PriorityQueue *pq, Event event) {
    if (pq->size >= 100) {
        printf("Hàng đợi đầy!\n");
        return;
    }

    int i = pq->size - 1;
    while (i >= 0 && pq->events[i].priority > event.priority) {
        pq->events[i + 1] = pq->events[i];
        i--;
    }

    pq->events[i + 1] = event;
    pq->size++;
}

Event pop(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("Hàng đợi trống!\n");
        Event emptyEvent = {"", -1};
        return emptyEvent;
    }

    Event event = pq->events[0];

    for (int i = 1; i < pq->size; i++) {
        pq->events[i - 1] = pq->events[i];
    }

    pq->size--;
    return event;
}

int isEmpty(PriorityQueue *pq) {
    return pq->size == 0;
}

void printQueue(PriorityQueue *pq) {
    printf("Các sự kiện trong hàng đợi:\n");
    for (int i = 0; i < pq->size; i++) {
        printf("Sự kiện: %s, Độ ưu tiên: %d\n", pq->events[i].name, pq->events[i].priority);
    }
}

int main() {
    PriorityQueue pq;
    initQueue(&pq);

    Event e1 = {"Thi giữa kỳ", 1};
    Event e2 = {"Giờ học Lý", 3};
    Event e3 = {"Chào cờ", 2};

    push(&pq, e1);
    push(&pq, e2);
    push(&pq, e3);

    printQueue(&pq);

    Event event = pop(&pq);
    printf("Xử lý sự kiện: %s, Độ ưu tiên: %d\n", event.name, event.priority);

    printQueue(&pq);

    return 0;
}
