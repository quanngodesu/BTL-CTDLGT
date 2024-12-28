#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_EVENTS 100
#define MAX_NAME 50
#define MAX_REMINDER 50
// Cấu trúc thời gian đơn giản hóa
typedef struct {
    int hour, minute;
    int day, month, year;
} Time;

typedef struct {
    char name[MAX_NAME];
    Time timeStart;
    Time timeEnd;
    int priority;
    int status;
    int reminder;
} Event;

int isValidTime(Time time) {
    if(time.hour < 0 || time.hour > 23) return 0;
    if(time.minute < 0 || time.minute > 59) return 0;
    if(time.day < 1 || time.day > 31) return 0;
    if(time.month < 1 || time.month > 12) return 0;
    if(time.year < 1900) return 0;
    return 1;
}

// So sánh thời gian
int compareTime(Time t1, Time t2) {
    if (t1.year != t2.year) return t1.year - t2.year;
    if (t1.month != t2.month) return t1.month - t2.month;
    if (t1.day != t2.day) return t1.day - t2.day;
    if (t1.hour != t2.hour) return t1.hour - t2.hour;
    return t1.minute - t2.minute;
}

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
    // Create a default empty event
    Event emptyEvent = {"",{0, 0, 0, 0, 0},{0, 0, 0, 0, 0},0,0,0};
    // Check if queue is empty
    if (isQueueEmpty(q)) {
        printf("Queue is empty.\n");
        return emptyEvent;
    }

    // Store the event to be returned
    Event event = q->events[q->front];

    // If this is the last element
    if (q->front == q->rear) {
        // Reset the queue to empty state
        q->front = -1;
        q->rear = -1;
    } else {
        // Move front pointer to next element, considering circular nature
        q->front = (q->front + 1) % MAX_EVENTS;
    }

    return event;
}

int isConflict(Queue *q, Event newEvent, int skipIndex) {
    if (isQueueEmpty(q)) return -1;
    
    int i = q->front;
    do {
        // Bỏ qua event đang được edit (nếu có)
        if (i == skipIndex) {
            if (i == q->rear) break;
            i = (i + 1) % MAX_EVENTS;
            continue;
        }
        
        // Kiểm tra xung đột thời gian
        Event existingEvent = q->events[i];
        
        // Điều kiện xung đột:
        // 1. Sự kiện mới bắt đầu trong khoảng thời gian của sự kiện cũ
        // 2. Sự kiện mới kết thúc trong khoảng thời gian của sự kiện cũ
        // 3. Sự kiện mới bao trùm sự kiện cũ
        if (
            // Kiểm tra start time của event mới nằm trong khoảng thời gian của event cũ
            (compareTime(newEvent.timeStart, existingEvent.timeStart) >= 0 && 
             compareTime(newEvent.timeStart, existingEvent.timeEnd) <= 0) ||
            
            // Kiểm tra end time của event mới nằm trong khoảng thời gian của event cũ 
            (compareTime(newEvent.timeEnd, existingEvent.timeStart) >= 0 &&
             compareTime(newEvent.timeEnd, existingEvent.timeEnd) <= 0) ||
            
            // Kiểm tra event mới bao trùm event cũ
            (compareTime(newEvent.timeStart, existingEvent.timeStart) <= 0 &&
             compareTime(newEvent.timeEnd, existingEvent.timeEnd) >= 0)
        ) {
            return i; // Trả về index của event bị xung đột
        }
        
        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
    } while (1);
    
    return -1; // Không có xung đột
}

// Hàm chuyển status number thành string
const char* getStatusString(int status) {
    switch(status) {
        case 0: return "Not Started";
        case 1: return "In Progress";
        case 2: return "Completed";
        default: return "Unknown";
    }
}
int totalEventNum;
void displayEvents(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("No events to display.\n");
        return;
    }

    printf("+-----+--------------------------------+------------------------------------------+------------+-------------+--------------------------------+\n");
    printf("| No  |             Name               |                  Time                    |  Priority  |   Status    |            Reminder            |\n");
    printf("+-----+--------------------------------+------------------------------------------+------------+-------------+--------------------------------+\n");

    int i = q->front;
    int eventNum = 1;
    while (1) {
        printf("| %-3d | %-30s | %02d:%02d %02d/%02d/%-4d -> %02d:%02d %02d/%02d/%-8d | %-10d | %-11s | %-30d |\n",
               eventNum, q->events[i].name,
               q->events[i].timeStart.hour, q->events[i].timeStart.minute, 
               q->events[i].timeStart.day, q->events[i].timeStart.month, q->events[i].timeStart.year,
               q->events[i].timeEnd.hour, q->events[i].timeEnd.minute, 
               q->events[i].timeEnd.day, q->events[i].timeEnd.month, q->events[i].timeEnd.year,
               q->events[i].priority, getStatusString(q->events[i].status),
               q->events[i].reminder);

        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
        eventNum++;
    }
    totalEventNum = eventNum;

    printf("+-----+--------------------------------+------------------------------------------+------------+-------------+--------------------------------+\n");
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
//Time parseTimeString(char* timeStr) {
//   Time time;
//   char* token;
//   
//   // Lấy giờ
//   token = strtok(timeStr, "/:");
//   time.hour = atoi(token);
//   
//   // Lấy phút
//   token = strtok(NULL, "/:");
//   time.minute = atoi(token);
//   
//   // Lấy ngày
//   token = strtok(NULL, "/:");
//   time.day = atoi(token);
//   
//   // Lấy tháng 
//   token = strtok(NULL, "/:");
//   time.month = atoi(token);
//   
//   // Lấy năm
//   token = strtok(NULL, "/:");
//   time.year = atoi(token);
//
//   return time;
//}

//// Sử dụng:
//char str[] = "14:30/25/12/2024";  // Format: HH:mm/DD/MM/YYYY
//Time time = parseTimeString(str);


//Time inputTime(const char *prompt) {
//    Time time;
//    char timeStr[20];
//    do {
//        printf("%s (HH:mm/DD/MM/YYYY): ", prompt);
//        scanf("%s", timeStr);
//        getchar(); // clear buffer
//        time = parseTimeString(timeStr);
//        if (!isValidTime(time)) {
//            printf("Invalid time format. Please try again.\n");
//        }
//    } while (!isValidTime(time));
//    return time;
//}

Event createEvent() {
    Event newEvent;
    
    printf("Enter event name: ");
    fgets(newEvent.name, MAX_NAME, stdin);
    newEvent.name[strcspn(newEvent.name, "\n")] = 0;

    // Nhập thời gian bắt đầu
    do {
        printf("Enter start time:\n");
        printf("Hour (0-23): "); scanf("%d", &newEvent.timeStart.hour);
        printf("Minute (0-59): "); scanf("%d", &newEvent.timeStart.minute);
        printf("Day (1-31): "); scanf("%d", &newEvent.timeStart.day);
        printf("Month (1-12): "); scanf("%d", &newEvent.timeStart.month);
        printf("Year: "); scanf("%d", &newEvent.timeStart.year);
        
        if (!isValidTime(newEvent.timeStart)) {
            printf("Invalid time. Please try again.\n");
        }
    } while (!isValidTime(newEvent.timeStart));

    // Nhập thời gian kết thúc
    do {
        printf("Enter end time:\n");
        printf("Hour (0-23): "); scanf("%d", &newEvent.timeEnd.hour);
        printf("Minute (0-59): "); scanf("%d", &newEvent.timeEnd.minute);
        printf("Day (1-31): "); scanf("%d", &newEvent.timeEnd.day);
        printf("Month (1-12): "); scanf("%d", &newEvent.timeEnd.month);
        printf("Year: "); scanf("%d", &newEvent.timeEnd.year);
        
        if (!isValidTime(newEvent.timeEnd) || compareTime(newEvent.timeEnd, newEvent.timeStart) <= 0) {
            printf("Invalid time or end time is before start time. Please try again.\n");
        }
    } while (!isValidTime(newEvent.timeEnd) || compareTime(newEvent.timeEnd, newEvent.timeStart) <= 0);

    printf("Enter event priority: ");
    scanf("%d", &newEvent.priority);

    printf("Enter status (0-Not Started, 1-In Progress, 2-Completed): ");
    scanf("%d", &newEvent.status);

    getchar();
    printf("Enter reminder: ");
    scanf("%d", &newEvent.reminder);
    getchar();

    return newEvent;
}

void editEventName(Event *event) {
    printf("Enter new event name: ");
    fgets(event->name, MAX_NAME, stdin);
    event->name[strcspn(event->name, "\n")] = 0;
    printf("Successfully edited.\n");
}

void editStartTime(Queue *q, int eventIndex) {
    Time newTime;
    do {
        printf("Enter new start time:\n");
        printf("Hour (0-23): "); scanf("%d", &newTime.hour);
        printf("Minute (0-59): "); scanf("%d", &newTime.minute);
        printf("Day (1-31): "); scanf("%d", &newTime.day);
        printf("Month (1-12): "); scanf("%d", &newTime.month);
        printf("Year: "); scanf("%d", &newTime.year);
        
        if (!isValidTime(newTime)) {
            printf("Invalid time. Please try again.\n");
            continue;
        }

        Event tempEvent = q->events[eventIndex];
        tempEvent.timeStart = newTime;
        
        if (compareTime(tempEvent.timeEnd, newTime) <= 0) {
            printf("Start time must be before end time. Please try again.\n");
            continue;
        }
        
        if (isConflict(q, tempEvent, eventIndex) >= 0) {
            printf("Conflict detected. Please try again.\n");
        } else {
            q->events[eventIndex].timeStart = newTime;
            printf("Start time updated successfully.\n");
            break;
        }
    } while (1);
}

void editEndTime(Queue *q, int eventIndex) {
    Time newTime;
    do {
        printf("Enter new end time:\n");
        printf("Hour (0-23): "); scanf("%d", &newTime.hour);
        printf("Minute (0-59): "); scanf("%d", &newTime.minute);
        printf("Day (1-31): "); scanf("%d", &newTime.day);
        printf("Month (1-12): "); scanf("%d", &newTime.month);
        printf("Year: "); scanf("%d", &newTime.year);
        
        if (!isValidTime(newTime)) {
            printf("Invalid time. Please try again.\n");
            continue;
        }

        Event tempEvent = q->events[eventIndex];
        tempEvent.timeEnd = newTime;
        
        if (compareTime(newTime, tempEvent.timeStart) <= 0) {
            printf("End time must be after start time. Please try again.\n");
            continue;
        }
        
        if (isConflict(q, tempEvent, eventIndex) >= 0) {
            printf("Conflict detected. Please try again.\n");
        } else {
            q->events[eventIndex].timeEnd = newTime;
            printf("End time updated successfully.\n");
            break;
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
    scanf("%d",&event->reminder);
    printf("Successfully edited.\n");
}

// Add this new function for manual status updates
void updateEventStatus(Event *event) {
    int newStatus;
    do {
        printf("Current status: %s\n", getStatusString(event->status));
        printf("Enter new status (0-Not Started, 1-In Progress, 2-Completed): ");
        scanf("%d", &newStatus);
        getchar();
        
        if (newStatus < 0 || newStatus > 2) {
            printf("Invalid status. Please enter 0, 1, or 2.\n");
        }
    } while (newStatus < 0 || newStatus > 2);
    
    event->status = newStatus;
    printf("Status updated successfully to: %s\n", getStatusString(newStatus));
}

// Modify the editEvents function to include status update option
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
        printf("6. Update Status\n");  // New option
        printf("7. Back to Main Menu\n");
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
                updateEventStatus(event);  // New case
                break;
            case 7:
                printf("Edit cancelled.\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);
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
}

// Lấy thời gian hiện tại và chuyển sang định dạng Time
Time getCurrentTime() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    
    Time currentTime = {
        .hour = tm_now->tm_hour,
        .minute = tm_now->tm_min,
        .day = tm_now->tm_mday,
        .month = tm_now->tm_mon + 1,  // tm_mon đếm từ 0
        .year = tm_now->tm_year + 1900  // tm_year đếm từ 1900
    };
    return currentTime;
}

void checkStatus(Queue *q) {
    if (isQueueEmpty(q)) return;
    
    Time currentTime = getCurrentTime();
    int i = q->front;
    int updated = 0;

    do {
        // Kiểm tra và cập nhật status dựa trên thời gian hiện tại
        if (compareTime(currentTime, q->events[i].timeStart) < 0) {
            // Chưa đến thời gian bắt đầu
            if (q->events[i].status != 0) {
                q->events[i].status = 0; // Not Started
                updated = 1;
            }
        }
        else if (compareTime(currentTime, q->events[i].timeEnd) > 0) {
            // Đã qua thời gian kết thúc
            if (q->events[i].status != 2) {
                q->events[i].status = 2; // Completed
                updated = 1;
            }
        }
        else {
            // Đang trong thời gian diễn ra
            if (q->events[i].status != 1) {
                q->events[i].status = 1; // In Progress
                updated = 1;
            }
        }

        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
    } while (1);

    if (updated) {
        printf("\nStatus has been updated for some events.\n");
        displayEvents(q);
    }
}

void checkReminder(Queue *q) {
    if (isQueueEmpty(q)) return;
    
    Time currentTime = getCurrentTime();
    int i = q->front;
    int reminderShown = 0;

    do {
        // Tính thời gian còn lại đến khi sự kiện bắt đầu (tính bằng phút)
        Time eventStart = q->events[i].timeStart;
        int minutesUntilStart = 
            (eventStart.year - currentTime.year) * 525600 +  // 365 * 24 * 60
            (eventStart.month - currentTime.month) * 43200 + // 30 * 24 * 60
            (eventStart.day - currentTime.day) * 1440 +      // 24 * 60
            (eventStart.hour - currentTime.hour) * 60 +
            (eventStart.minute - currentTime.minute);

        // Kiểm tra nếu thời gian còn lại bằng với thời gian reminder (tính bằng phút)
        if (minutesUntilStart > 0 && minutesUntilStart <= q->events[i].reminder) {
            printf("\n⏰ REMINDER: Event '%s' will start in %d minutes!\n", 
                   q->events[i].name, minutesUntilStart);
            printf("Start time: %02d:%02d %02d/%02d/%d\n",
                   eventStart.hour, eventStart.minute,
                   eventStart.day, eventStart.month, eventStart.year);
            reminderShown = 1;
        }

        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
    } while (1);

    if (reminderShown) {
        printf("\nCurrent time: %02d:%02d %02d/%02d/%d\n",
               currentTime.hour, currentTime.minute,
               currentTime.day, currentTime.month, currentTime.year);
    }
}

// Hàm chính để chạy kiểm tra liên tục
void startRealTimeChecks(Queue *q) {
    printf("Started real-time monitoring...\n");
    printf("Press Ctrl+C to stop\n\n");
    
    while (1) {
        checkStatus(q);
        checkReminder(q);
        _sleep(60); // Đợi 1 phút trước khi kiểm tra lại
    }
}

// Fix the sorting function syntax and logic to use compareTime
void sortEventsByTime(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("No events to sort.\n");
        return;
    }
    
    // Calculate number of events
    int count = 0;
    int i = q->front;
    do {
        count++;
        if (i == q->rear) break;
        i = (i + 1) % MAX_EVENTS;
    } while (1);
    
    // Bubble sort using compareTime
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int idx1 = (q->front + i) % MAX_EVENTS;
            int idx2 = (q->front + j) % MAX_EVENTS;
            
            if (compareTime(q->events[idx1].timeStart, q->events[idx2].timeStart) > 0) {
                Event temp = q->events[idx1];
                q->events[idx1] = q->events[idx2];
                q->events[idx2] = temp;
            }
        }
    }
    printf("Events sorted successfully by start time.\n");
}

// Update main function to include sorting option
int main() {
    Queue eventQueue;
    initializeQueue(&eventQueue);
    int choice;
    int index;

    do {
        // Automatically check and update status before showing menu
        checkStatus(&eventQueue);
        checkReminder(&eventQueue);
        
        printf("\nMain Menu:\n");
        printf("1. Add Events\n");
        printf("2. Display Events\n");
        printf("3. Delete Events\n");
        printf("4. Edit Events\n");
        printf("5. Check Status\n");
        printf("6. Sort Events by Time\n");  // New option
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        system("cls");

        switch(choice) {
            case 1:
                addEvents(&eventQueue);
                break;
            
            case 2:
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
                checkStatus(&eventQueue);
                displayEvents(&eventQueue);
                break;
            
            case 6:  // New case
                sortEventsByTime(&eventQueue);
                displayEvents(&eventQueue);
                break;
                
            case 7:
                printf("Exiting program...\n");
                break;
            
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    return 0;
}
