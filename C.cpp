#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 100
#define MAX_NAME 50
#define MAX_REMINDER 50

int totalEventNum;
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
    totalEventNum ++;
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
    totalEventNum --;

    return event;
}

void formatTime(int time, char *buffer) {
    int hour = time / 100;
    int minute = time % 100;
    sprintf(buffer, "%02d:%02d", hour, minute);
}
int isInvalidTime(int time) {
    int hour = time / 100;
    int minute = time % 100;
    return (time < 0 || time > 2359 || hour < 0 || hour > 23 || minute < 0 || minute > 59);
}


void sortEventsByTime(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("No events to sort.\n");
        return;
    }

    for (int i = 0; i < totalEventNum - 1; i++) {
        for (int j = i + 1; j < totalEventNum; j++) {
            if (q->events[i].timeStart > q->events[j].timeStart) {
                Event temp = q->events[i];
                q->events[i] = q->events[j];
                q->events[j] = temp;
            }
        }
    }
}


int isConflict(Queue *q, Event newEvent, int skipIndex) {
    if (isQueueEmpty(q)) return -1;
    
    int i = q->front;
    do {
        if (i == skipIndex) {
            if (i == q->rear) break;
            i = (i + 1) % MAX_EVENTS;
            continue;
        }
        
        if ((newEvent.timeStart < q->events[i].timeEnd) &&
            (newEvent.timeEnd > q->events[i].timeStart)) {
            return i;
        }
        
        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
    } while (1);
    return -1;
}

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
    sortEventsByTime(q);
}

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

void editEventName(Event *event) {
    printf("Enter new event name: ");
    fgets(event->name, MAX_NAME, stdin);
    event->name[strcspn(event->name, "\n")] = 0;
    printf("Successfully edited.\n");
}

void editStartTime(Queue *q, int eventIndex) {
    int newStartTime;
    do {
        printf("Enter new start time: ");
        scanf("%d", &newStartTime);
        getchar();
        if (isInvalidTime(newStartTime)) {
            printf("Invalid time. Please try again.\n");
        } else {
            Event tempEvent = q->events[eventIndex];
            tempEvent.timeStart = newStartTime;
            if (isConflict(q, tempEvent, eventIndex) >=0 ) {
                printf("Conflict detected. Please re-enter a valid time.\n");
            } else {
                q->events[eventIndex].timeStart = newStartTime;
                printf("Start time updated successfully.\n");
                return;
            }
        }
    } while (1);
}


void editEndTime(Queue *q, int eventIndex) {
    int newEndTime;
    do {
        printf("Enter new end time: ");
        scanf("%d", &newEndTime);
        getchar();
        if (isInvalidTime(newEndTime)) {
            printf("Invalid time. Please try again.\n");
        } else {
            Event tempEvent = q->events[eventIndex];
            tempEvent.timeEnd = newEndTime;
            if (isConflict(q, tempEvent, eventIndex) >=0 ) {
                printf("Conflict detected. Please re-enter a valid time.\n");
            } else {
                q->events[eventIndex].timeEnd = newEndTime;
                printf("End time updated successfully.\n");
                return;
            }
        }
    } while (1);
}


void editEventPriority(Event *event) {
    printf("Enter new priority: ");
    scanf("%d", &event->priority);
    getchar();
    printf("Successfully edited.\n");
}

void editEventReminder(Event *event) {
    printf("Enter new reminder: ");
    fgets(event->reminder, MAX_REMINDER, stdin);
    event->reminder[strcspn(event->reminder, "\n")] = 0;
    printf("Successfully edited.\n");
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
    	printf("\n");
    	printf("================================\n");
        printf("|          Edit Menu:          |\n");
        printf("|1. Edit Name                  |\n");
        printf("|2. Edit Start Time            |\n");
        printf("|3. Edit End Time              |\n");
        printf("|4. Edit Priority              |\n");
        printf("|5. Edit Reminder              |\n");
        printf("|6. Back to Main Menu          |\n");
        printf("================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        system("cls");

        switch (choice) {
            case 1:
                editEventName(event);
                break;
            case 2:
                editStartTime(q, eventNum - 1);  
                break;
            case 3:
                editEndTime(q, eventNum - 1);  
                break;
            case 4:
                editEventPriority(event);
                break;
            case 5:
                editEventReminder(event);
                break;
            case 6:
                printf("Edit cancelled.\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
    sortEventsByTime(q);
}

void addEvents(Queue *q) {
    if (isQueueFull(q)) {
        printf("Queue is full.\n");
        return;
    }
    
    Event newEvent = createEvent();
    int conflictIndex;
    
    while ((conflictIndex = isConflict(q, newEvent, -1)) >= 0) {
        if (q->events[conflictIndex].priority > newEvent.priority) {
            printf("Conflict with Event %d. Replace it? (1-Yes/0-No): ", conflictIndex + 1);
            int choice;
            scanf("%d", &choice);
            getchar();
            
            if (choice == 1) {
                q->events[conflictIndex] = newEvent;
                printf("Event replaced.\n");
                return;
            }
        }
        printf("Event conflicts with existing event. Please retry.\n");
        newEvent = createEvent();
    }
    
    enqueue(q, newEvent);
    printf("Event added.\n");
    sortEventsByTime(q);
}

int main() {
    Queue eventQueue;
    initializeQueue(&eventQueue);
    int choice;
    int index;
    printf("================================\n");
    printf("|                              |\n");
    printf("|        Time Management       |\n");
    printf("|            System            |\n");
    printf("|                              |\n");
    printf("================================\n");
    printf("Press Enter to continue: ");
    getchar();
    system("cls");

    do {
    	printf("\n");
    	printf("================================\n");
        printf("|          Main Menu:          |\n");
        printf("|1. Add Events                 |\n");
        printf("|2. Display Events             |\n");
        printf("|3. Delete Events              |\n");
        printf("|4. Edit Events                |\n");
        printf("|5. Exit                       |\n");
        printf("================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        system("cls");

        switch(choice){
        	case 1:
        		addEvents(&eventQueue);
        		break;
        	
        	case 2:
        		sortEventsByTime(&eventQueue);
        		displayEvents(&eventQueue);
        		break;
        	
        	case 3:
        		displayEvents(&eventQueue);
        		printf("Enter event number you would like to delete: ");
        		scanf("%d", &index);
        		deleteEvents(&eventQueue, index-1);
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
