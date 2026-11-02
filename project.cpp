#include <iostream>
#include <string>
#include <stack>
using namespace std;

// MODULE 1: Generic Doubly Linked List
template <class T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node(T value) {
            data = value;
            prev = nullptr;
            next = nullptr;
        }
    };

    Node* head;
    Node* tail;

public:
    DoublyLinkedList() {
        head = tail = nullptr;
    }

    ~DoublyLinkedList() {
        clear();
    }

    void insert(T value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void remove(T value) {
        Node* temp = head;

        while (temp != nullptr) {
            if (temp->data == value) {
                if (temp->prev)
                    temp->prev->next = temp->next;
                else
                    head = temp->next;

                if (temp->next)
                    temp->next->prev = temp->prev;
                else
                    tail = temp->prev;

                delete temp;
                return;
            }
            temp = temp->next;
        }
    }

    void clear() {
        Node* temp = head;

        while (temp != nullptr) {
            Node* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }

        head = tail = nullptr;
    }

    bool empty() {
        return head == nullptr;
    }

    void display() {
        Node* temp = head;

        while (temp != nullptr) {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    // MODULE 2: Browser History Navigation

    class Iterator {
    public:
        Node* current;

        Iterator(Node* node = nullptr) {
            current = node;
        }

        T& operator*() {
            return current->data;
        }

        Iterator& operator++() {
            if (current)
                current = current->next;
            return *this;
        }

        Iterator& operator--() {
            if (current)
                current = current->prev;
            return *this;
        }

        bool operator!=(const Iterator& other) {
            return current != other.current;
        }

        bool operator==(const Iterator& other) {
            return current == other.current;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }
};

// Browser Tab Class
class Tab {
public:
    DoublyLinkedList<string> history;
    DoublyLinkedList<string>::Iterator current;
    bool hasCurrent;

    Tab(string url) {
        history.insert(url);
        current = history.begin();
        hasCurrent = true;
    }

    void visit(string url) {
        if (hasCurrent) {
            // Delete forward history
            auto temp = current.current->next;

            while (temp != nullptr) {
                auto nextNode = temp->next;
                history.remove(temp->data);
                temp = nextNode;
            }
        }

        history.insert(url);

        // Move current pointer to last page
        current = history.begin();

        while (current.current->next != nullptr) {
            ++current;
        }

        hasCurrent = true;

        cout << "Visited: " << url << endl;
    }

    void back() {
        if (hasCurrent && current.current->prev != nullptr) {
            --current;
            cout << "Current Page: " << *current << endl;
        } else {
            cout << "No previous page available!" << endl;
        }
    }

    void forward() {
        if (hasCurrent && current.current->next != nullptr) {
            ++current;
            cout << "Current Page: " << *current << endl;
        } else {
            cout << "No forward page available!" << endl;
        }
    }

    string getURL() {
        if (hasCurrent)
            return *current;

        return "No Page";
    }

    void showHistory() {
        cout << "Page History: ";
        history.display();
        cout << "Current Page: " << getURL() << endl;
    }
};

// MODULE 3 and 4: Browser Tab Manager

class Browser {
private:
    DoublyLinkedList<Tab*> tabs;
    DoublyLinkedList<Tab*>::Iterator activeTab;
    bool hasActiveTab;

    stack<Tab*> closedTabs;

public:
    Browser() {
        hasActiveTab = false;
    }

    ~Browser() {
        while (!tabs.empty()) {
            Tab* tab = *tabs.begin();
            tabs.remove(tab);
            delete tab;
        }

        while (!closedTabs.empty()) {
            delete closedTabs.top();
            closedTabs.pop();
        }
    }

    void openTab(string url) {
        Tab* newTab = new Tab(url);
        tabs.insert(newTab);

        activeTab = tabs.begin();

        while (activeTab.current->next != nullptr) {
            ++activeTab;
        }

        hasActiveTab = true;

        cout << "New tab opened: " << url << endl;
    }

    void switchTab(int position) {
        if (tabs.empty()) {
            cout << "No tabs available!" << endl;
            return;
        }

        if (position < 1) {
            cout << "Invalid tab number!" << endl;
            return;
        }

        auto temp = tabs.begin();
        int count = 1;

        while (temp != tabs.end() && count < position) {
            ++temp;
            count++;
        }

        if (temp == tabs.end()) {
            cout << "Invalid tab number!" << endl;
            return;
        }

        activeTab = temp;
        hasActiveTab = true;

        cout << "Switched to Tab " << position << endl;
        showCurrentPage();
    }

    void closeTab() {
        if (!hasActiveTab || tabs.empty()) {
            cout << "No active tab to close!" << endl;
            return;
        }

        Tab* closingTab = *activeTab;

        auto nextTab = activeTab;
        ++nextTab;

        if (nextTab != tabs.end()) {
            activeTab = nextTab;
        } else {
            auto previousTab = activeTab;
            --previousTab;

            if (previousTab != activeTab) {
                activeTab = previousTab;
            } else {
                hasActiveTab = false;
            }
        }

        tabs.remove(closingTab);
        closedTabs.push(closingTab);

        cout << "Tab closed successfully!" << endl;

        if (hasActiveTab)
            showCurrentPage();
        else
            cout << "No tabs remaining!" << endl;
    }

    void undoClose() {
        if (closedTabs.empty()) {
            cout << "No closed tabs to restore!" << endl;
            return;
        }

        Tab* restoredTab = closedTabs.top();
        closedTabs.pop();

        tabs.insert(restoredTab);

        activeTab = tabs.begin();

        while (activeTab.current->next != nullptr) {
            ++activeTab;
        }

        hasActiveTab = true;

        cout << "Closed tab restored!" << endl;
        showCurrentPage();
    }

    Tab* getActiveTab() {
        if (!hasActiveTab)
            return nullptr;

        return *activeTab;
    }

    void visitPage(string url) {
        Tab* tab = getActiveTab();

        if (tab)
            tab->visit(url);
        else
            cout << "Open a tab first!" << endl;
    }

    void goBack() {
        Tab* tab = getActiveTab();

        if (tab)
            tab->back();
        else
            cout << "No active tab!" << endl;
    }

    void goForward() {
        Tab* tab = getActiveTab();

        if (tab)
            tab->forward();
        else
            cout << "No active tab!" << endl;
    }

    void showCurrentPage() {
        Tab* tab = getActiveTab();

        if (tab)
            cout << "Active Tab URL: " << tab->getURL() << endl;
        else
            cout << "No active tab!" << endl;
    }

    void showAllTabs() {
        cout << "\n------ OPEN TABS ------\n";

        int count = 1;

        for (auto it = tabs.begin(); it != tabs.end(); ++it) {
            cout << "Tab " << count << ": "
                 << (*it)->getURL();

            if (hasActiveTab && it.current == activeTab.current)
                cout << " <-- ACTIVE";

            cout << endl;
            count++;
        }

        if (count == 1)
            cout << "No open tabs!" << endl;
    }

    void showHistory() {
        Tab* tab = getActiveTab();

        if (tab)
            tab->showHistory();
        else
            cout << "No active tab!" << endl;
    }
};

// MODULE 5: Command Line Interface

int main() {
    Browser browser;
    int choice;
    string url;
    int tabNumber;

    cout << "====================================\n";
    cout << "     BROWSER TAB AND HISTORY MANAGER\n";
    cout << "====================================\n";

    do {
        cout << "\n========== BROWSER MENU ==========\n";
        cout << "1. Open New Tab\n";
        cout << "2. Visit New Page\n";
        cout << "3. Go Back\n";
        cout << "4. Go Forward\n";
        cout << "5. Close Current Tab\n";
        cout << "6. Undo Close Tab\n";
        cout << "7. Switch Tab\n";
        cout << "8. Show All Tabs\n";
        cout << "9. Show Page History\n";
        cout << "10. Show Current Page\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                cout << "Enter website URL: ";
                cin >> url;
                browser.openTab(url);
                break;

            case 2:
                cout << "Enter website URL: ";
                cin >> url;
                browser.visitPage(url);
                break;

            case 3:
                browser.goBack();
                break;

            case 4:
                browser.goForward();
                break;

            case 5:
                browser.closeTab();
                break;

            case 6:
                browser.undoClose();
                break;

            case 7:
                cout << "Enter tab number: ";
                cin >> tabNumber;
                browser.switchTab(tabNumber);
                break;

            case 8:
                browser.showAllTabs();
                break;

            case 9:
                browser.showHistory();
                break;

            case 10:
                browser.showCurrentPage();
                break;

            case 0:
                cout << "Thank you for using Browser Manager!\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    return 0;
}