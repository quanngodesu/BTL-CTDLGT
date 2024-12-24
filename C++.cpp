#include <iostream>
#include <string>
#include <queue>
#include <cstdio>
#include <iomanip>
using namespace std;

class Event {
private:
    string name;
    int time;
    int priority;
    string reminder;
    
    static string formatTime(int time) {
        int hour = time / 100;
        int minute = time % 100;
        return to_string(hour) + " " + (minute < 10 ? "0" : "") + to_string(minute);
    }
public:
    Event(string n, int t, int p, string r) {
        name = n;
        time = t;
        priority = p;
        reminder = r;
    }

    string getName() const {
        return name;
    }
    int getTime() const {
        return time;
    }
    int getPriority() const {
        return priority;
    }
    string getReminder() const {
        return reminder;
    }

    static void display(const queue<Event>& events) {
        if (events.empty()) {
            cout << "No events to display." << endl;
            return;
        }
        cout << "+---------------------------------------------------------------------------+" << endl;
        cout << "|   Name         |       Time      |  Priority  |          Reminder         |" << endl;
        cout << "+---------------------------------------------------------------------------+" << endl;
        
        queue<Event> tempQueue = events;
        while (!tempQueue.empty()) {
            Event event = tempQueue.front();
            tempQueue.pop();
            printf("| %-14s | %-15s | %-10d | %-25s |\n",
                   event.getName().c_str(),
                   formatTime(event.getTime()).c_str(),
                   event.getPriority(),
                   event.getReminder().c_str());
        }

        cout << "+---------------------------------------------------------------------------+" << endl;
    }
};

int main() {
    queue<Event> eventQueue;  
    int choice;

    do {
        cout << "\nMenu:\n";
        cout << "1. Add Event\n";
        cout << "2. Display Events\n";
        cout << "3. Handle Conflicts\n";
        //cout << "4. Add Reminder\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 1) {
            string name, reminder;
            int time, priority;
            
            cout << "Enter event name: ";
            getline(cin, name);
            cout << "Enter event time: ";
            cin >> time;
            cout << "Enter event priority: ";
            cin >> priority;
            cin.ignore();  
            cout << "Enter reminder: ";
            getline(cin, reminder);

            Event newEvent(name, time, priority, reminder);
            eventQueue.push(newEvent);  
            cout << "Event added to queue." << endl;
        }
        else if (choice == 2) {
            Event::display(eventQueue);  
        }
        else if (choice == 3) {
            cout << " " << endl;
        }
        else if (choice == 4) {
            cout << " " << endl;
        }
        else if (choice == 5) {
            cout << "Exiting program." << endl;
        }
        else {
            cout << "Invalid choice, please try again." << endl;
        }
    } while (choice != 5); 
    
    return 0;
}
