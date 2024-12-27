#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 100
#define MAX_NAME 50
#define MAX_REMINDER 50

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
int isInvalidTime(int time){
	return (time < 0 || time > 2400);
}

int isConflict(Queue *q, Event newEvent) {
    int i = q->front;
    while (1) {
        if ((newEvent.timeStart < q->events[i].timeEnd) && (newEvent.timeEnd > q->events[i].timeStart)) {
            return 1; // Conflict detected
        }
        if (i == q->rear) {
            break;
        }
        i = (i + 1) % MAX_EVENTS;
    }
    return 0; // No conflict
}


int totalEventNum;
void displayEvents(Queue *q) {  
    if (isQueueEmpty(q)) {
        printf("No events to display.\n");
        return;
    }

    printf("+---+--------------------------------+----------------+------------+--------------------------------+\n");
    printf("|No |              Name              |      Time      |  Priority  |             Reminder           |\n");
    printf("+---+--------------------------------+----------------+------------+--------------------------------+\n");

    int i = q->front;
    int eventNum = 1;
    while (1) {
        char timeStartStr[10];
        char timeEndStr[10];
        formatTime(q->events[i].timeStart, timeStartStr);
        formatTime(q->events[i].timeEnd, timeEndStr);

        printf("|%-3d| %-30s | %s -> %s | %-10d | %-30s |\n",
               eventNum, q->events[i].name, timeStartStr, timeEndStr, q->events[i].priority, q->events[i].reminder);

        if (i == q->rear) {
            break;
        }
        i = (i + 1) % MAX_EVENTS;
        eventNum ++;
    }
    totalEventNum = eventNum;

    printf("+---+--------------------------------+----------------+------------+--------------------------------+\n");
}

void deleteEvents(Queue *q, int index) {
    if (isQueueEmpty(q)) {
        printf("No events to delete.\n");
        return;
    }
    if (index < 0 || index >= MAX_EVENTS) {
        printf("Invalid index.\n");
        return;
    }
    int i = index;
    while (i != q->rear) {
        int nextIndex = (i + 1) % MAX_EVENTS;
        q->events[i] = q->events[nextIndex];
        i = nextIndex;
    }
    if (q->front == q->rear) {
        q->front = q->rear = -1; 
    } else {
        q->rear = (q->rear - 1 + MAX_EVENTS) % MAX_EVENTS;  
    }

    totalEventNum--; 
    printf("Event deleted successfully.\n");
}

// Function to input and validate event time
int inputTime(const char *prompt) {
    int time;
    do {
        printf("%s (HHMM): ", prompt);
        scanf("%d", &time);
        if (isInvalidTime(time)) {
            printf("Invalid time. Please try again.\n");
        }
    } while (isInvalidTime(time));
    return time;
}

// Function to create a new event
Event createEvent() {
    Event newEvent;

    printf("Enter event name: ");
    fgets(newEvent.name, MAX_NAME, stdin);
    newEvent.name[strcspn(newEvent.name, "\n")] = 0;

    newEvent.timeStart = inputTime("Enter event time start");
    do {
        newEvent.timeEnd = inputTime("Enter event time end");
        if (newEvent.timeEnd < newEvent.timeStart) {
            printf("Error: End time must be later than start time. Please try again.\n");
        }
    } while (newEvent.timeEnd < newEvent.timeStart);

    printf("Enter event priority: ");
    scanf("%d", &newEvent.priority);

    getchar(); 
    printf("Enter reminder: ");
    fgets(newEvent.reminder, MAX_REMINDER, stdin);
    newEvent.reminder[strcspn(newEvent.reminder, "\n")] = 0;

    return newEvent;
}

void editEvents(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("No events to edit.\n");
        return;
    }

    int eventNum;
    displayEvents(q);
    do {
        printf("Enter event number to edit: ");
        scanf("%d", &eventNum);
        getchar();
        if (eventNum <= 0 || eventNum > totalEventNum || isQueueEmpty(q)) {
            printf("Invalid event number.\n");
        }
    } while (eventNum <= 0 || eventNum > totalEventNum || isQueueEmpty(q));

    Event *event = &q->events[eventNum - 1];
    int choice;
    do {
        printf("\nEdit Menu: \n");
        printf("1. Edit Name\n");
        printf("2. Edit Start Time\n");
        printf("3. Edit End Time\n");
        printf("4. Edit Priority\n");
        printf("5. Edit Reminder\n");
        printf("6. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter new event name: ");
                fgets(event->name, MAX_NAME, stdin);
                event->name[strcspn(event->name, "\n")] = 0;
                printf("Successfully edited.\n");
                break;

            case 2:
                do {
                    printf("Enter new start time: ");
                    scanf("%d", &event->timeStart);
                    getchar();
                    if (isInvalidTime(event->timeStart)) {
                        printf("Invalid time. Please try again.\n");
                    } else if (isConflict(q, *event)) {
                        printf("Conflict detected. Please re-enter a valid time.\n");
                    }
                } while (isInvalidTime(event->timeStart) || isConflict(q, *event));
                printf("Successfully edited.\n");
                break;

            case 3:
                do {
                    printf("Enter new end time: ");
                    scanf("%d", &event->timeEnd);
                    getchar();
                    if (isInvalidTime(event->timeEnd) || event->timeStart > event->timeEnd) {
                        printf("Invalid time. Please try again.\n");
                    } else if (isConflict(q, *event)) {
                        printf("Conflict detected. Please re-enter a valid time.\n");
                    }
                } while (isInvalidTime(event->timeEnd) || event->timeStart > event->timeEnd || isConflict(q, *event));
                printf("Successfully edited.\n");
                break;

            case 4:
                printf("Enter new priority: ");
                scanf("%d", &event->priority);
                getchar();
                printf("Successfully edited.\n");
                break;

            case 5:
                printf("Enter new reminder: ");
                fgets(event->reminder, MAX_REMINDER, stdin);
                event->reminder[strcspn(event->reminder, "\n")] = 0;
                printf("Successfully edited.\n");
                break;

            case 6:
                printf("Edit cancelled.\n");
                return;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
}

void sortEventsByPriority(Queue *q) {
    for (int i = 0; i < totalEventNum - 1; i++) {
        for (int j = i + 1; j < totalEventNum; j++) {
            if (q->events[i].priority > q->events[j].priority) {
                Event temp = q->events[i];
                q->events[i] = q->events[j];
                q->events[j] = temp;
            }
        }
    }
}

void addEvents(Queue *q) {
    if (isQueueFull(q)) {
        printf("Queue is full. Cannot add more events.\n");
        return;
    }

    Event newEvent = createEvent();

    if (isConflict(q, newEvent)) {
        printf("Conflict detected. Please re-enter the event details.\n");
        addEvents(q); // Recursive call to re-enter event details
    } else {
        enqueue(q, newEvent);
        printf("Event added successfully.\n");
    }
}


int main() {
    Queue eventQueue;
    initializeQueue(&eventQueue);
    int choice;
    int index;

    do {
        printf("\nMenu:\n");
        printf("1. Add Events\n");
        printf("2. Display Events\n");
        printf("3. Delete Events\n");
        printf("4. Edit Events\n");
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
        		printf("Enter event number you would like to delete: ");
        		scanf("%d", &index);
        		deleteEvents(&eventQueue, index);
        		break;
        	
        	case 4:
        		editEvents(&eventQueue);
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