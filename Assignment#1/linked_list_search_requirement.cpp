#include <iostream>
#include <cmath>
#include <string>
using namespace std;

// ---------- Transaction & Node Structures ----------
struct Transaction {
    string customerID;
    string product;
    double price;
    string date;
    string category;
    string paymentMethod;
};

struct Node {
    Transaction data;
    Node* next;
    Node(Transaction t) : data(t), next(nullptr) {}
};

// ---------- Linked List ----------
class LinkedList {
public:
    Node* head;
    LinkedList() : head(nullptr) {}

    void insert(Transaction t) {
        Node* newNode = new Node(t);
        if (!head) head = newNode;
        else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    int getLength() const {
        int count = 0;
        Node* temp = head;
        while (temp) {
            count++;
            temp = temp->next;
        }
        return count;
    }

    void sortByCategory() {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            Node* curr = head;
            while (curr->next) {
                if (curr->data.category > curr->next->data.category) {
                    swap(curr->data, curr->next->data);
                    swapped = true;
                }
                curr = curr->next;
            }
        } while (swapped);
    }

    void sortByPrice() {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            Node* curr = head;
            while (curr->next) {
                if (curr->data.price > curr->next->data.price) {
                    swap(curr->data, curr->next->data);
                    swapped = true;
                }
                curr = curr->next;
            }
        } while (swapped);
    }

    Node** toArray(int& length) const {
        length = getLength();
        Node** arr = new Node*[length];
        Node* temp = head;
        for (int i = 0; i < length && temp; ++i) {
            arr[i] = temp;
            temp = temp->next;
        }
        return arr;
    }
};

// ---------- Search Algorithms ----------
class LinkedListSearch {
public:
    // 1. Linear Search
    static int linearSearch(Node* head, const string& category, const string& payment) {
        int count = 0;
        while (head) {
            if (head->data.category == category && head->data.paymentMethod == payment)
                count++;
            head = head->next;
        }
        return count;
    }

    // 2. Binary Search (needs sorted list by category)
    static int binarySearch(const LinkedList& list, const string& category, const string& payment) {
        int len = 0;
        Node** arr = list.toArray(len);
        int count = 0;
        int low = 0, high = len - 1;

        while (low <= high) {
            int mid = (low + high) / 2;
            if (arr[mid]->data.category == category) {
                // Search left and right for all matches
                int i = mid;
                while (i >= 0 && arr[i]->data.category == category) {
                    if (arr[i]->data.paymentMethod == payment) count++;
                    i--;
                }
                i = mid + 1;
                while (i < len && arr[i]->data.category == category) {
                    if (arr[i]->data.paymentMethod == payment) count++;
                    i++;
                }
                break;
            } else if (arr[mid]->data.category < category)
                low = mid + 1;
            else
                high = mid - 1;
        }

        delete[] arr;
        return count;
    }

    // 3. Jump Search
    static int jumpSearch(Node* head, const string& category, const string& payment) {
        int length = 0;
        Node* temp = head;
        while (temp) { length++; temp = temp->next; }

        int step = sqrt(length);
        int count = 0;

        Node* current = head;
        while (current) {
            Node* jump = current;
            for (int i = 0; i < step && jump->next; ++i)
                jump = jump->next;

            Node* walker = current;
            while (walker != jump->next) {
                if (walker->data.category == category && walker->data.paymentMethod == payment)
                    count++;
                walker = walker->next;
            }

            current = jump->next;
        }

        return count;
    }

    // 4. Interpolation Search (adapted for price field)
    static Node* interpolationSearchByPrice(const LinkedList& list, double targetPrice) {
        int len = 0;
        Node** arr = list.toArray(len);
        int low = 0, high = len - 1;

        while (low <= high &&
               targetPrice >= arr[low]->data.price &&
               targetPrice <= arr[high]->data.price) {

            if (arr[high]->data.price == arr[low]->data.price)
                break;

            int pos = low + ((targetPrice - arr[low]->data.price) * (high - low)) /
                              (arr[high]->data.price - arr[low]->data.price);

            if (pos < 0 || pos >= len)
                break;

            double currentPrice = arr[pos]->data.price;

            if (currentPrice == targetPrice) {
                Node* result = arr[pos];
                delete[] arr;
                return result;
            }

            if (currentPrice < targetPrice)
                low = pos + 1;
            else
                high = pos - 1;
        }

        delete[] arr;
        return nullptr;
    }
};

