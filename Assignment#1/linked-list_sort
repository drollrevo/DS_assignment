#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Transaction structure
struct Transaction {
    string customerID;
    string product;
    string category;
    float price;
    string date;
    string paymentMethod;
    Transaction* next;
};

int parseDate(const string& date) {
    int day, month, year;
    char slash;
    stringstream ss(date);
    ss >> day >> slash >> month >> slash >> year;
    return year * 10000 + month * 100 + day;
}

// Insert node at end
void insert(Transaction*& head, string id, string product, string category, float price, string date, string method) {
    Transaction* newNode = new Transaction{id, product, category, price, date, method, nullptr};
    if (!head) {
        head = newNode;
        return;
    }
    Transaction* temp = head;
    while (temp->next)
        temp = temp->next;
    temp->next = newNode;
}

// Print the list
void printList(Transaction* head) {
    while (head) {
        cout << head->customerID << " | " << head->date << " | " << head->product << " | $" << head->price << endl;
        head = head->next;
    }
}

// ------- Sorting Algorithms --------

// Merge Sort
void splitList(Transaction* source, Transaction*& front, Transaction*& back) {
    Transaction* slow = source;
    Transaction* fast = source->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    front = source;
    back = slow->next;
    slow->next = nullptr;
}

Transaction* merge(Transaction* a, Transaction* b) {
    if (!a) return b;
    if (!b) return a;
    Transaction* result;
    if (parseDate(a->date) < parseDate(b->date)) {
        result = a;
        result->next = merge(a->next, b);
    } else {
        result = b;
        result->next = merge(a, b->next);
    }
    return result;
}

void mergeSort(Transaction*& head) {
    if (!head || !head->next) return;
    Transaction *a, *b;
    splitList(head, a, b);
    mergeSort(a);
    mergeSort(b);
    head = merge(a, b);
}

// Insertion Sort
void insertionSort(Transaction*& head) {
    Transaction* sorted = nullptr;
    Transaction* current = head;
    while (current) {
        Transaction* next = current->next;
        if (!sorted || parseDate(current->date) < parseDate(sorted->date)) {
            current->next = sorted;
            sorted = current;
        } else {
            Transaction* temp = sorted;
            while (temp->next && parseDate(temp->next->date) <= parseDate(current->date)) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    head = sorted;
}

// Bubble Sort
void bubbleSort(Transaction* head) {
    if (!head) return;
    bool swapped;
    Transaction* ptr;
    do {
        swapped = false;
        ptr = head;
        while (ptr->next) {
            if (parseDate(ptr->date) > parseDate(ptr->next->date)) {
                swap(ptr->customerID, ptr->next->customerID);
                swap(ptr->product, ptr->next->product);
                swap(ptr->category, ptr->next->category);
                swap(ptr->price, ptr->next->price);
                swap(ptr->date, ptr->next->date);
                swap(ptr->paymentMethod, ptr->next->paymentMethod);
                swapped = true;
            }
            ptr = ptr->next;
        }
    } while (swapped);
}

// Selection Sort
void selectionSort(Transaction* head) {
    for (Transaction* i = head; i && i->next; i = i->next) {
        Transaction* minNode = i;
        for (Transaction* j = i->next; j; j = j->next) {
            if (parseDate(j->date) < parseDate(minNode->date)) {
                minNode = j;
            }
        }
        if (minNode != i) {
            swap(i->customerID, minNode->customerID);
            swap(i->product, minNode->product);
            swap(i->category, minNode->category);
            swap(i->price, minNode->price);
            swap(i->date, minNode->date);
            swap(i->paymentMethod, minNode->paymentMethod);
        }
    }
}

// --------- Main Program ----------
int main() {
    Transaction* head = nullptr;

    // Sample data
    insert(head, "CUST4434", "Mouse", "Automotive", 483.15, "09/05/2023", "Bank Transfer");
    insert(head, "CUST6046", "Headphones", "Books", 681.74, "01/05/2022", "PayPal");
    insert(head, "CUST5197", "Gaming Console", "Automotive", 1665.82, "07/07/2022", "Debit Card");
    insert(head, "CUST8621", "Smartwatch", "Books", 81.03, "25/12/2022", "Cash on Delivery");

    cout << "Before sorting:\n";
    printList(head);

    int choice;
    cout << "\nChoose a sorting method:\n";
    cout << "1. Merge Sort\n";
    cout << "2. Insertion Sort\n";
    cout << "3. Bubble Sort\n";
    cout << "4. Selection Sort\n";
    cout << "Enter your choice (1-4): ";
    cin >> choice;

    switch (choice) {
        case 1:
            mergeSort(head);
            cout << "\nAfter sorting by date (Merge Sort):\n";
            break;
        case 2:
            insertionSort(head);
            cout << "\nAfter sorting by date (Insertion Sort):\n";
            break;
        case 3:
            bubbleSort(head);
            cout << "\nAfter sorting by date (Bubble Sort):\n";
            break;
        case 4:
            selectionSort(head);
            cout << "\nAfter sorting by date (Selection Sort):\n";
            break;
        default:
            cout << "Invalid choice. No sorting applied.\n";
            return 1;
    }

    printList(head);
    return 0;
}
