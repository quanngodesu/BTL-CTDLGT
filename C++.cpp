#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
using namespace std;

class Event {
    private:
        string name;
        int time;
        int priority;
    public:
        Event(string n, int t, int p) {
            name = n;
            time = t;
            priority = p;
        }

        string getName() const{
            return name;
        }
        int getTime() const{
            return time;
        }
        int getPriority() const{
            return priority;
        }

        void setName(string n) {
            name = n;
        }
        void setTime(int t) {
            time = t;
        }
        void setPriority(int p) {
            priority = p;
        }

        static void display(const vector<Event>& events) {
            if (events.empty()) {
                cout << "No events to display." << endl;
                return;
            }
            cout << "+-----------------------------------------------+" << endl;
            cout << "|   Name         |   Time (Hours)  |  Priority  |" << endl;
            cout << "+-----------------------------------------------+" << endl;
            for (const auto& event : events) {
                printf("| %-14s | %-15d | %-10d |\n",
                       event.getName().c_str(),
                       event.getTime(),
                       event.getPriority());
            }
            cout << "+-----------------------------------------------+" << endl;
        }
};

int main() {
    Event event1("Di choi voi ny", 2200, 1);
    Event event2("Hoc XSTK", 2000, 2);

    vector<Event> events = {event1, event2};

    Event::display(events);

    return 0;
}
