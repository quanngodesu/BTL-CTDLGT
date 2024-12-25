#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 100
#define MAX_NAME 50
#define MAX_REMINDER 100

typedef struct {
    char name[MAX_NAME];
    int timeStart;
    int timeEnd;
    int priority;
    char reminder[MAX_REMINDER];
} Event;

typedef struct {
    Event events[MAX_EVENTS];
    int front;
    int rear;
} Queue;

void initializeQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

int isQueueEmpty(Queue *q) {
    return q->front == -1;
}

int isQueueFull(Queue *q) {
    return (q->rear + 1) % MAX_EVENTS == q->front;
}

void enqueue(Queue *q, Event event) {
    if (isQueueFull(q)) {
        printf("Queue is full. Cannot add more events.\n");
        return;
    }

    if (isQueueEmpty(q)) {
        q->front = 0;
    }
    q->rear = (q->rear + 1) % MAX_EVENTS;
    q->events[q->rear] = event;
}


Event dequeue(Queue *q) {
    Event event = {"", 0, 0, 0, ""};
    if (isQueueEmpty(q)) {
        printf("Queue is empty.\n");
        return event;
    }

    event = q->events[q->front];
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % MAX_EVENTS;
    }

    return event;
}

void formatTime(int time, char *buffer) {
    int hour = time / 100;
    int minute = time % 100;
    sprintf(buffer, "%02d:%02d", hour, minute);
}
//Add Events
void addEvents(Queue *q){

	if(isQueueFull(q)){
		printf("Queue is full. Cannot add more events.\n");
		return;
	}	
	Event newEvent;
    printf("Enter event name: ");
    fgets(newEvent.name, MAX_NAME, stdin);
    newEvent.name[strcspn(newEvent.name, "\n")] = 0; 

    printf("Enter event time start (HHMM): ");
    scanf("%d", &newEvent.timeStart);
    do{
		printf("Enter event time end (HHMM): ");
        scanf("%d", &newEvent.timeEnd);
        if(newEvent.timeEnd < newEvent.timeStart){
        	printf("Error. The end time should be later than the start time. Please try again.\n");
		}
	} while(newEvent.timeEnd < newEvent.timeStart);
    
    printf("Enter event priority: ");
    scanf("%d", &newEvent.priority);
    getchar(); 

    printf("Enter reminder: ");
    fgets(newEvent.reminder, MAX_REMINDER, stdin);
    newEvent.reminder[strcspn(newEvent.reminder, "\n")] = 0; 

    enqueue(q, newEvent);
    printf("Event added to queue.\n");
}

// Display Events
void displayEvents(Queue *q) {  
    if (isQueueEmpty(q)) {
        printf("No events to display.\n");
        return;
    }

    printf("+-------------------------------------------------------------------------------------+\n");
    printf("|            Name           |      Time      |  Priority  |          Reminder         |\n");
    printf("+-------------------------------------------------------------------------------------+\n");

    int i = q->front;
    while (1) {
        char timeStartStr[10];
        char timeEndStr[10];
        formatTime(q->events[i].timeStart, timeStartStr);
        formatTime(q->events[i].timeEnd, timeEndStr);

        printf("| %-25s | %s -> %s | %-10d | %-25s |\n",
               q->events[i].name, timeStartStr, timeEndStr, q->events[i].priority, q->events[i].reminder);

        if (i == q->rear) {
            break;
        }
        i = (i + 1) % MAX_EVENTS;
    }

    printf("+-------------------------------------------------------------------------------------+\n");
}

int main() {
    Queue eventQueue;
    initializeQueue(&eventQueue);
    int choice;

    do {
        printf("\nMenu:\n");
        printf("1. Add Events\n");
        printf("2. Display Events\n");
        printf("3. Handle Conflicts\n");
        printf("4. Edit Event\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch(choice){
        	case 1:
        		addEvents(&eventQueue);
        		break;
        	
        	case 2:
        		displayEvents(&eventQueue);
        		break;
        	
        	case 3:
        		break;
        	
        	case 4:
        		break;
        		
        	case 5:
        		printf("Exiting program...\n");
        		break;
        	
        	default:
        		printf("Invalid choice. Please try again.\n");
        
		}
    } while (choice != 5);

    return 0;
}
