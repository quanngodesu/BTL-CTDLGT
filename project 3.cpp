#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTS 100

// Cấu trúc thời gian đơn giản hóa
typedef struct {
    int hour, minute;
    int day, month, year;
} Time;

// Cấu trúc sự kiện cơ bản
typedef struct {
    int id;
    char title[50];
    char location[50];
    Time start;
    Time end;
    int priority;    // 1 (cao) đến 5 (thấp)
    int status;      // 0: sắp diễn ra, 1: đang diễn ra, 2: đã kết thúc
    int reminder;    // Số phút trước khi nhắc nhở
} Event;

// Cấu trúc heap quản lý sự kiện
typedef struct {
    Event events[MAX_EVENTS];
    int size;
} EventHeap;

// Khởi tạo heap rỗng
void initHeap(EventHeap* heap) {
    heap->size = 0;
}

// So sánh thời gian
int compareTime(Time t1, Time t2) {
    if (t1.year != t2.year) return t1.year - t2.year;
    if (t1.month != t2.month) return t1.month - t2.month;
    if (t1.day != t2.day) return t1.day - t2.day;
    if (t1.hour != t2.hour) return t1.hour - t2.hour;
    return t1.minute - t2.minute;
}

// Hoán đổi hai sự kiện
void swapEvents(Event* a, Event* b) {
    Event temp = *a;
    *a = *b;
    *b = temp;
}

// Điều chỉnh heap lên (heapify up)
void heapifyUp(EventHeap* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        // So sánh ưu tiên và thời gian
        if (heap->events[index].priority < heap->events[parent].priority ||
            (heap->events[index].priority == heap->events[parent].priority &&
             compareTime(heap->events[index].start, heap->events[parent].start) < 0)) {
            swapEvents(&heap->events[index], &heap->events[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

// Điều chỉnh heap xuống (heapify down)
void heapifyDown(EventHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    // So sánh với node con bên trái
    if (left < heap->size && 
        (heap->events[left].priority < heap->events[smallest].priority ||
         (heap->events[left].priority == heap->events[smallest].priority &&
          compareTime(heap->events[left].start, heap->events[smallest].start) < 0))) {
        smallest = left;
    }
    
    // So sánh với node con bên phải
    if (right < heap->size &&
        (heap->events[right].priority < heap->events[smallest].priority ||
         (heap->events[right].priority == heap->events[smallest].priority &&
          compareTime(heap->events[right].start, heap->events[smallest].start) < 0))) {
        smallest = right;
    }
    
    // Nếu cần thiết, hoán đổi và tiếp tục điều chỉnh xuống
    if (smallest != index) {
        swapEvents(&heap->events[index], &heap->events[smallest]);
        heapifyDown(heap, smallest);
    }
}

// Xóa sự kiện theo ID
int deleteEvent(EventHeap* heap, int id) {
    int found_index = -1;
    
    // Tìm sự kiện cần xóa
    for (int i = 0; i < heap->size; i++) {
        if (heap->events[i].id == id) {
            found_index = i;
            break;
        }
    }
    
    // Nếu không tìm thấy sự kiện
    if (found_index == -1) {
        return 0;
    }
    
    // Thay thế sự kiện cần xóa bằng sự kiện cuối cùng
    heap->events[found_index] = heap->events[heap->size - 1];
    heap->size--;
    
    // Điều chỉnh heap
    if (found_index < heap->size) {
        // Điều chỉnh xuống nếu cần
        heapifyDown(heap, found_index);
        
        // Điều chỉnh lên nếu cần
        heapifyUp(heap, found_index);
    }
    
    return 1;
}
// Thêm sự kiện mới vào heap
int addEvent(EventHeap* heap, Event event) {
    if (heap->size >= MAX_EVENTS) {
        return 0; // Heap đầy
    }    
    heap->events[heap->size] = event;
    heapifyUp(heap, heap->size);
    heap->size++;
    return 1;
}

// Lấy thời gian hiện tại
Time getCurrentTime() {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    Time current = {
        .hour = tm_info->tm_hour,
        .minute = tm_info->tm_min,
        .day = tm_info->tm_mday,
        .month = tm_info->tm_mon + 1,
        .year = tm_info->tm_year + 1900
    };
    return current;
}

// Kiểm tra xung đột thời gian
int hasTimeConflict(Event e1, Event e2) {
    return !(compareTime(e1.end, e2.start) <= 0 || 
             compareTime(e2.end, e1.start) <= 0);
}

// Kiểm tra xung đột với tất cả sự kiện hiện có
int checkConflicts(EventHeap* heap, Event newEvent) {
    for (int i = 0; i < heap->size; i++) {
        if (hasTimeConflict(heap->events[i], newEvent)) {
            if (heap->events[i].priority <= newEvent.priority) {
                printf("Cảnh báo: Xung đột với sự kiện '%s' (ưu tiên cao hơn)\n", 
                       heap->events[i].title);
                return 1;
            }
        }
    }
    return 0;
}

// Cập nhật trạng thái các sự kiện
void updateEventStatus(EventHeap* heap) {
    Time current = getCurrentTime();
    for (int i = 0; i < heap->size; i++) {
        Event* event = &heap->events[i];
        if (compareTime(current, event->start) >= 0 && 
            compareTime(current, event->end) < 0) {
            if (event->status != 1) {
                event->status = 1;
                printf("Sự kiện đang diễn ra: %s\n", event->title);
            }
        } else if (compareTime(current, event->end) >= 0) {
            event->status = 2;
        }
    }
}

// Kiểm tra và hiển thị nhắc nhở
void checkReminders(EventHeap* heap) {
    Time current = getCurrentTime();
    for (int i = 0; i < heap->size; i++) {
        Event event = heap->events[i];
        if (event.status == 0) {
            // Tính số phút đến thời điểm bắt đầu
            int minutesToStart = 
                (event.start.hour - current.hour) * 60 + 
                (event.start.minute - current.minute);
            
            if (minutesToStart <= event.reminder && minutesToStart > 0) {
                printf("\nNHẮC NHỞ: '%s' sẽ bắt đầu sau %d phút\n", 
                       event.title, minutesToStart);
            }
        }
    }
}

// Hiển thị danh sách sự kiện
void displayEvents(EventHeap* heap) {
    printf("\n=== DANH SÁCH SỰ KIỆN ===\n");
    for (int i = 0; i < heap->size; i++) {
        Event e = heap->events[i];
        printf("\nID: %d\n", e.id);
        printf("Tiêu đề: %s\n", e.title);
        printf("Địa điểm: %s\n", e.location);
        printf("Thời gian: %02d:%02d %02d/%02d/%d\n", 
               e.start.hour, e.start.minute, 
               e.start.day, e.start.month, e.start.year);
        printf("Độ ưu tiên: %d\n", e.priority);
        printf("Trạng thái: %s\n", 
               e.status == 0 ? "Sắp diễn ra" : 
               e.status == 1 ? "Đang diễn ra" : "Đã kết thúc");
        printf("------------------------\n");
    }
}

// Hàm main để demo
int main() {
    EventHeap heap;
    initHeap(&heap);
    int choice;
    
    do {
        updateEventStatus(&heap);
        checkReminders(&heap);
        
        printf("\n=== MENU ===\n");
        printf("1. Thêm sự kiện mới\n");
        printf("2. Xem danh sách sự kiện\n");
        printf("3. Kiểm tra nhắc nhở\n");
        printf("4. Xóa sự kiện\n");    // Thêm tùy chọn mới
        printf("0. Thoát\n");
        printf("Lựa chọn: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: {
                Event newEvent;
                printf("Nhập tiêu đề: ");
                scanf(" %[^\n]s", newEvent.title);
                // Nhập các thông tin khác...
                
                if (!checkConflicts(&heap, newEvent)) {
                    addEvent(&heap, newEvent);
                    printf("Đã thêm sự kiện thành công!\n");
                }
                break;
            }
            case 2:
                displayEvents(&heap);
                break;
            case 3:
                checkReminders(&heap);
                break;
            case 4: {
                int id;
                printf("Nhập ID sự kiện cần xóa: ");
                scanf("%d", &id);
                if (deleteEvent(&heap, id)) {
                    printf("Đã xóa sự kiện thành công!\n");
                } else {
                    printf("Không tìm thấy sự kiện với ID đã cho!\n");
                }
                break;
            }
        }
    } while (choice != 0);
    
    return 0;
}