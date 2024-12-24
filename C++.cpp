#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <sstream>
using namespace std;

class Event {
    private:
        string name;
        int hours;
        int minutes;
        int priority;
        string reminder;

    public:
        Event(string n, int h, int m, int p, string r) {
            name = n;
            hours = h;
            minutes = m;
            priority = p;
            reminder = r;
        }

        string getName() const {
            return name;
        }

        int getHours() const {
            return hours;
        }

        int getMinutes() const {
            return minutes;
        }

        int getPriority() const {
            return priority;
        }

        string getReminder() const {
            return reminder;
        }

        void setName(string n) {
            name = n;
        }

        void setTime(int h, int m) {
            hours = h;
            minutes = m;
        }

        void setPriority(int p) {
            priority = p;
        }

        void setReminder(string r) {
            reminder = r;
        }

        static string formatTime(int hours, int minutes) {
            return to_string(hours) + " " + (minutes < 10 ? "0" + to_string(minutes) : to_string(minutes));
        }

        static void display(const vector<Event>& events) {
            if (events.empty()) {
                cout << "No events to display." << endl;
                return;
            }
            cout << "+---------------------------------------------------------------------------+" << endl;
            cout << "|      Name      |       Time      |  Priority  |          Reminder         |" << endl;
            cout << "+---------------------------------------------------------------------------+" << endl;
            for (const auto& event : events) {
                printf("| %-14s | %-15s | %-10d | %-25s |\n",
                       event.getName().c_str(),
                       formatTime(event.getHours(), event.getMinutes()).c_str(),
                       event.getPriority(),
                       event.getReminder().c_str());
            }
            cout << "+---------------------------------------------------------------------------+" << endl;
        }
};

int main() {
    
    string name, reminder;
    int hour, minute, priority;

    
    Event event1("Di choi voi ny", 22, 0, 1, "Ve nha truoc 23 00");
    Event event2("Hoc XSTK", 20, 0, 2, "Lam BTVN");

    vector<Event> events = {event1, event2};

    Event::display(events);

    return 0;
}
