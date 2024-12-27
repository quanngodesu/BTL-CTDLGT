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
	while(1){
		if((newEvent.timeStart < q->events[i].timeEnd) && (newEvent.timeEnd > q->events[i].timeStart)){
			return 1;
		}
		if(i == q->rear){
			break;
		}
		i = (i+1) % MAX_EVENTS;
	}
	return 0;
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
    while(isInvalidTime(newEvent.timeStart)){
    	printf("Invalid time. Please try again.\n");
    	printf("Enter event time start (HHMM): ");
        scanf("%d", &newEvent.timeStart);
	}
    do{
		printf("Enter event time end (HHMM): ");
        scanf("%d", &newEvent.timeEnd);
        if(newEvent.timeEnd < newEvent.timeStart){
        	printf("Error. The end time should be later than the start time. Please try again.\n");
		}
		if(isInvalidTime(newEvent.timeEnd)){
			printf("Invalid time. Please try again.\n");
		}
	} while(newEvent.timeEnd < newEvent.timeStart || isInvalidTime(newEvent.timeEnd));
    
    printf("Enter event priority: ");
    scanf("%d", &newEvent.priority);
    getchar(); 

    printf("Enter reminder: ");
    fgets(newEvent.reminder, MAX_REMINDER, stdin);
    newEvent.reminder[strcspn(newEvent.reminder, "\n")] = 0; 

    enqueue(q, newEvent);
    printf("Event added to queue.\n");
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

void handleConflict(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty, no events to check for conflicts.\n");
        return;
    }

    int i, j;
    int eventsCount = totalEventNum;  

    for (i = q->front; i != q->rear; i = (i + 1) % MAX_EVENTS) {
        for (j = (i + 1) % MAX_EVENTS; j != q->rear; j = (j + 1) % MAX_EVENTS) {
            if ((q->events[i].timeStart < q->events[j].timeEnd) && 
                (q->events[i].timeEnd > q->events[j].timeStart)) {
                printf("Conflict detected between event %s (%d-%d) and event %s (%d-%d).\n",
                       q->events[i].name, q->events[i].timeStart, q->events[i].timeEnd,
                       q->events[j].name, q->events[j].timeStart, q->events[j].timeEnd);

                int choice;
                do {
                    printf("Handle Conflicts Menu:\n");
                    printf("1. Keep event %s\n", q->events[i].name);
                    printf("2. Keep event %s\n", q->events[j].name);
                    printf("3. Remove both events\n");
                    printf("4. Back to Main Menu\n");
                    printf("Enter your choice: ");
                    scanf("%d", &choice);

                    if (choice < 1 || choice > 4) {
                        printf("Invalid choice. Please select a valid option.\n");
                    }
                } while (choice < 1 || choice > 4);

                switch (choice) {
                    case 1:
                        printf("Event %s will be kept.\n", q->events[i].name);
                        deleteEvents(q, j);  
                        break;
                    case 2:
                        printf("Event %s will be kept.\n", q->events[j].name);
                        deleteEvents(q, i); 
						i = q->front; 
                        break;
                    case 3:
                        printf("Both events will be removed.\n");
                        deleteEvents(q, i);  
                        deleteEvents(q, j); 
						i = q->front;  
                        break;
                    case 4:
                        printf("Returning to main menu.\n");
                        return;
                    default:
                        break;
                }
                i = q->front;  
                break;
            }
            j = (j + 1) % MAX_EVENTS;
        }
        if(j == q ->rear){
        	i = (i + 1) % MAX_EVENTS;
		}
    }
}

void editEvents(Queue *q){
	if(isQueueEmpty(q)){
		printf("No events to edit.\n");
		return;
	}
	int eventNum;
	displayEvents(q);
	do{
		printf("Enter event number to edit: ");
	    scanf("%d", &eventNum);
	    getchar();
	    if(eventNum <= 0 || eventNum > totalEventNum || isQueueEmpty(q)){
		    printf("Invalid event number.\n");
	    }
	}while(eventNum <= 0 || eventNum > MAX_EVENTS || isQueueEmpty(q));
	
	Event *event = &q->events[eventNum-1];
	int choice;
	do{
		printf("\nEdit Menu: \n");
	    printf("1. Edit Name\n");
	    printf("2. Edit Start Time\n");
	    printf("3. Edit End Time\n");
	    printf("4. Edit Priority\n");
	    printf("5. Edit Reminder\n");
	    printf("6. Delete\n");
	    printf("7. Back to Main Menu\n");
	    printf("Enter your choice: ");
	    scanf("%d", &choice);
	    getchar();
	    
	    switch(choice){
	    	case 1:
	    		printf("Enter new event name: ");
	    		fgets(event->name, MAX_NAME, stdin);
	    		event->name[strcspn(event->name, "\n")] = 0;
	    		printf("Successfully editted.\n");
	    		break;
	    		
	    	case 2:
	    		printf("Enter new start time: ");
	    		scanf("%d", &event->timeStart);
	    		while(event->timeStart < 0 || event->timeStart > 2400){
	    			printf("Invalid time. Please try again.\n");
	    			printf("Enter new start time: ");
	    		    scanf("%d", &event->timeStart);
				}
				printf("Successfully editted.\n");
				break;
				
			case 3:
				printf("Enter new end time: ");
	    		scanf("%d", &event->timeEnd);
	    		while(event->timeEnd < 0 || event->timeEnd > 2400 || event->timeStart > event->timeEnd){
	    			printf("Invalid time. Please try again.\n");
	    			printf("Enter new end time: ");
	    		    scanf("%d", &event->timeEnd);
				}
				printf("Successfully editted.\n");
				break;
				
			case 4:
				printf("Enter new priority: ");
				scanf("%d", &event->priority);
				printf("Successfully editted.\n");
				break;
				
			case 5:
				printf("Enter new reminder: ");
                fgets(event->reminder, MAX_REMINDER, stdin);
                event->reminder[strcspn(event->reminder, "\n")] = 0;
                printf("Successfully editted.\n");
                break;
                
            case 6:
            	deleteEvents(q, eventNum - 1);
            	break;
            case 7:
            	printf("Edit cancelled.\n");
            	return;
            	
            default:
            	printf("Invalid choice. Please try again.\n");
		}
	}while(choice != 6);

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
        		handleConflict(&eventQueue);
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
