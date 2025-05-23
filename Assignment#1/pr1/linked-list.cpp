#include "linked-list.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>

// Helper function implementation
std::string standardizeDate(const std::string& date) {
    // Implementation would go here
    // For now, just return the date as is
    return date;
}

// LinkedList class implementation
LinkedList::LinkedList() : transactionHead(nullptr), reviewHead(nullptr), transactionCount(0), reviewCount(0) {}

LinkedList::~LinkedList() {
    // Clean up transaction nodes
    TransactionNode* currentT = transactionHead;
    while (currentT != nullptr) {
        TransactionNode* temp = currentT;
        currentT = currentT->next;
        delete temp;
    }
    
    // Clean up review nodes
    ReviewNode* currentR = reviewHead;
    while (currentR != nullptr) {
        ReviewNode* temp = currentR;
        currentR = currentR->next;
        delete temp;
    }
}

void LinkedList::addTransaction(const Transaction& t) {
    TransactionNode* newNode = new TransactionNode(t);
    if (transactionHead == nullptr) {
        transactionHead = newNode;
    } else {
        TransactionNode* current = transactionHead;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    transactionCount++;
}

void LinkedList::addReview(const Review& r) {
    ReviewNode* newNode = new ReviewNode(r);
    if (reviewHead == nullptr) {
        reviewHead = newNode;
    } else {
        ReviewNode* current = reviewHead;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    reviewCount++;
}

void LinkedList::sortTransactionsByDate(int sortChoice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (sortChoice == 1) {
        // Bubble Sort
        bubbleSortTransactions();
    } else if (sortChoice == 2) {
        // Insertion Sort
        insertionSortTransactions();
    } else if (sortChoice == 3) {
        // Selection Sort
        selectionSortTransactions();
    } else if (sortChoice == 4) {
        // Merge Sort
        transactionHead = mergeSortTransactions(transactionHead);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // Display results
    std::cout << "\nSorting completed in " << duration_ms << " ms\n";
    countTransactionsByDate();
}

void LinkedList::bubbleSortTransactions() {
    if (transactionHead == nullptr || transactionHead->next == nullptr) return;
    
    bool swapped;
    TransactionNode* current;
    TransactionNode* last = nullptr;
    
    do {
        swapped = false;
        current = transactionHead;
        
        while (current->next != last) {
            if (current->data.date > current->next->data.date) {
                // Swap data (not nodes)
                Transaction temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}

void LinkedList::insertionSortTransactions() {
    if (transactionHead == nullptr || transactionHead->next == nullptr) return;
    
    TransactionNode* sorted = nullptr;
    TransactionNode* current = transactionHead;
    
    while (current != nullptr) {
        TransactionNode* next = current->next;
        
        if (sorted == nullptr || sorted->data.date >= current->data.date) {
            current->next = sorted;
            sorted = current;
        } else {
            TransactionNode* search = sorted;
            while (search->next != nullptr && search->next->data.date < current->data.date) {
                search = search->next;
            }
            current->next = search->next;
            search->next = current;
        }
        
        current = next;
    }
    
    transactionHead = sorted;
}

void LinkedList::selectionSortTransactions() {
    if (transactionHead == nullptr || transactionHead->next == nullptr) return;
    
    TransactionNode* current = transactionHead;
    
    while (current != nullptr) {
        TransactionNode* min = current;
        TransactionNode* r = current->next;
        
        while (r != nullptr) {
            if (r->data.date < min->data.date) {
                min = r;
            }
            r = r->next;
        }
        
        if (min != current) {
            // Swap data (not nodes)
            Transaction temp = current->data;
            current->data = min->data;
            min->data = temp;
        }
        
        current = current->next;
    }
}

TransactionNode* LinkedList::mergeSortTransactions(TransactionNode* head) {
    // Base case
    if (head == nullptr || head->next == nullptr) {
        return head;
    }
    
    // Split the list into two halves
    TransactionNode* middle = getMiddle(head);
    TransactionNode* nextOfMiddle = middle->next;
    middle->next = nullptr;
    
    // Recursively sort both lists
    TransactionNode* left = mergeSortTransactions(head);
    TransactionNode* right = mergeSortTransactions(nextOfMiddle);
    
    // Merge the sorted lists
    TransactionNode* sortedList = sortedMerge(left, right);
    return sortedList;
}

TransactionNode* LinkedList::getMiddle(TransactionNode* head) {
    if (head == nullptr) return head;
    
    TransactionNode* slow = head;
    TransactionNode* fast = head->next;
    
    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    return slow;
}

TransactionNode* LinkedList::sortedMerge(TransactionNode* a, TransactionNode* b) {
    TransactionNode* result = nullptr;
    
    // Base cases
    if (a == nullptr) return b;
    if (b == nullptr) return a;
    
    // Pick smaller value
    if (a->data.date <= b->data.date) {
        result = a;
        result->next = sortedMerge(a->next, b);
    } else {
        result = b;
        result->next = sortedMerge(a, b->next);
    }
    return result;
}

void LinkedList::countTransactionsByDate() {
    std::map<std::string, int> dateCounts;
    TransactionNode* current = transactionHead;
    
    while (current != nullptr) {
        dateCounts[current->data.date]++;
        current = current->next;
    }
    
    std::cout << "\nTransaction Counts by Date:\n";
    for (const auto& pair : dateCounts) {
        std::cout << pair.first << ": " << pair.second << " transactions\n";
    }
}

void LinkedList::calculateElectronicsCreditCardPercentage(int searchChoice, int sortChoice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int totalElectronics = 0;
    int electronicsCreditCard = 0;
    std::vector<Transaction> electronicsTransactions;
    
    // First use search algorithm to find Electronics transactions
    if (searchChoice == 1) {
        // Linear Search
        TransactionNode* current = transactionHead;
        while (current != nullptr) {
            if (current->data.category == "Electronics") {
                electronicsTransactions.push_back(current->data);
                totalElectronics++;
                if (current->data.payment_method == "Credit Card") {
                    electronicsCreditCard++;
                }
            }
            current = current->next;
        }
    } else {
        // For other search methods in a linked list, we would need to
        // convert to a vector first, which defeats the purpose of using linked list
        // So we'll use linear search for all cases
        std::cout << "\nNote: Linked lists are best suited for linear search. "
                << "Other search methods require conversion to arrays.\n";
        
        TransactionNode* current = transactionHead;
        while (current != nullptr) {
            if (current->data.category == "Electronics") {
                electronicsTransactions.push_back(current->data);
                totalElectronics++;
                if (current->data.payment_method == "Credit Card") {
                    electronicsCreditCard++;
                }
            }
            current = current->next;
        }
    }
    
    // Now sort the electronics transactions by price
    if (sortChoice == 1) {
        // Bubble Sort
        for (size_t i = 0; i < electronicsTransactions.size(); i++) {
            for (size_t j = 0; j < electronicsTransactions.size() - i - 1; j++) {
                if (electronicsTransactions[j].price > electronicsTransactions[j + 1].price) {
                    std::swap(electronicsTransactions[j], electronicsTransactions[j + 1]);
                }
            }
        }
    } else if (sortChoice == 2) {
        // Insertion Sort
        for (size_t i = 1; i < electronicsTransactions.size(); i++) {
            auto key = electronicsTransactions[i];
            int j = i - 1;
            while (j >= 0 && electronicsTransactions[j].price > key.price) {
                electronicsTransactions[j + 1] = electronicsTransactions[j];
                j--;
            }
            electronicsTransactions[j + 1] = key;
        }
    } else if (sortChoice == 3) {
        // Selection Sort
        for (size_t i = 0; i < electronicsTransactions.size(); i++) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < electronicsTransactions.size(); j++) {
                if (electronicsTransactions[j].price < electronicsTransactions[min_idx].price) {
                    min_idx = j;
                }
            }
            std::swap(electronicsTransactions[i], electronicsTransactions[min_idx]);
        }
    } else if (sortChoice == 4) {
        // Quick Sort (using std::sort)
        std::sort(electronicsTransactions.begin(), electronicsTransactions.end(), 
            [](const auto& a, const auto& b) { return a.price < b.price; });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    double percentage = (totalElectronics > 0) ? 
        (static_cast<double>(electronicsCreditCard) / totalElectronics) * 100.0 : 0.0;
    
    std::cout << "\nSearch and sort completed in " << duration_ms << " ms\n";
    std::cout << "Total Electronics purchases: " << totalElectronics << "\n";
    std::cout << "Electronics purchases with Credit Card: " << electronicsCreditCard << "\n";
    std::cout << "Percentage: " << percentage << "%\n";
    
    // Display some sorted electronics transactions (top 5)
    std::cout << "\nSorted Electronics Transactions (by price, ascending):\n";
    int count = 0;
    for (const auto& t : electronicsTransactions) {
        if (count++ >= 5) break;
        std::cout << t.product << " - $" << t.price << " (" << t.payment_method << ")\n";
    }
}

void LinkedList::findFrequentWordsInOneStarReviews(int sortChoice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::string> oneStarReviewTexts;
    
    // Linear Search to find 1-star reviews (this part stays the same regardless of sort choice)
    ReviewNode* current = reviewHead;
    while (current != nullptr) {
        if (current->data.rating == 1) {
            oneStarReviewTexts.push_back(current->data.review_text);
        }
        current = current->next;
    }
    
    // Process the review texts to find frequent words
    std::map<std::string, int> wordFrequency;
    for (const auto& text : oneStarReviewTexts) {
        std::istringstream iss(text);
        std::string word;
        while (iss >> word) {
            // Convert to lowercase and remove punctuation
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            word.erase(std::remove_if(word.begin(), word.end(), 
                [](char c) { return std::ispunct(c); }), word.end());
            
            if (word.length() > 2) { // Skip very short words
                wordFrequency[word]++;
            }
        }
    }
    
    // Convert to vector for sorting
    std::vector<std::pair<std::string, int>> wordFreqVec(wordFrequency.begin(), wordFrequency.end());
    
    // Apply different sorting algorithms based on user choice
    if (sortChoice == 1) {
        // Bubble Sort
        for (size_t i = 0; i < wordFreqVec.size(); i++) {
            for (size_t j = 0; j < wordFreqVec.size() - i - 1; j++) {
                if (wordFreqVec[j].second < wordFreqVec[j + 1].second) {
                    std::swap(wordFreqVec[j], wordFreqVec[j + 1]);
                }
            }
        }
    } else if (sortChoice == 2) {
        // Insertion Sort
        for (size_t i = 1; i < wordFreqVec.size(); i++) {
            auto key = wordFreqVec[i];
            int j = i - 1;
            while (j >= 0 && wordFreqVec[j].second < key.second) {
                wordFreqVec[j + 1] = wordFreqVec[j];
                j--;
            }
            wordFreqVec[j + 1] = key;
        }
    } else if (sortChoice == 3) {
        // Selection Sort
        for (size_t i = 0; i < wordFreqVec.size(); i++) {
            size_t max_idx = i;
            for (size_t j = i + 1; j < wordFreqVec.size(); j++) {
                if (wordFreqVec[j].second > wordFreqVec[max_idx].second) {
                    max_idx = j;
                }
            }
            std::swap(wordFreqVec[i], wordFreqVec[max_idx]);
        }
    } else if (sortChoice == 4) {
        // Quick Sort (using std::sort)
        std::sort(wordFreqVec.begin(), wordFreqVec.end(), 
            [](const auto& a, const auto& b) { return a.second > b.second; });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "\nAnalysis completed in " << duration_ms << " ms\n";
    std::cout << "Number of 1-star reviews: " << oneStarReviewTexts.size() << "\n";
    std::cout << "Most frequent words in 1-star reviews:\n";
    
    int count = 0;
    for (const auto& pair : wordFreqVec) {
        if (count++ >= 10) break; // Show top 10
        std::cout << pair.first << ": " << pair.second << " occurrences\n";
    }
}

// Load transactions from CSV
void loadTransactions(LinkedList& list, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening transactions file: " << filename << "\n";
        return;
    }
    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        Transaction t;
        int field = 0;
        while (std::getline(ss, token, ',')) {
            if (field == 0) t.customer_id = token;
            else if (field == 1) t.product = token;
            else if (field == 2) t.category = token;
            else if (field == 3) {
                try {
                    t.price = std::stod(token);
                } catch (...) {
                    t.price = 0.0;
                }
            }
            else if (field == 4) t.date = standardizeDate(token);
            else if (field == 5) t.payment_method = token;
            field++;
        }
        list.addTransaction(t);
    }
    file.close();
}

// Load reviews from CSV
void loadReviews(LinkedList& list, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening reviews file: " << filename << "\n";
        return;
    }
    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        Review r;
        int field = 0;
        while (std::getline(ss, token, ',')) {
            if (field == 0) r.product_id = token;
            else if (field == 1) r.customer_id = token;
            else if (field == 2) {
                try {
                    r.rating = std::stoi(token);
                } catch (...) {
                    r.rating = 0;
                }
            }
            else if (field == 3) {
                r.review_text = token;
                // Append remaining line (handles commas in review text)
                std::string remaining;
                std::getline(ss, remaining);
                if (!remaining.empty()) r.review_text += "," + remaining;
            }
            field++;
        }
        list.addReview(r);
    }
    file.close();
}

// Main function for user interaction
int main() {
    LinkedList list;
    loadTransactions(list, "transactions_cleaned.csv");
    loadReviews(list, "reviews_cleaned.csv");

    while (true) {
        std::cout << "\nE-commerce Transaction Analysis System (Linked List Implementation)\n";
        std::cout << "=================================================================\n";
        std::cout << "Choose a question to analyze:\n";
        std::cout << "1. Sort transactions by date and display counts\n";
        std::cout << "2. Calculate Electronics purchases with Credit Card percentage\n";
        std::cout << "3. Find frequent words in 1-star reviews\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice (1-4): ";

        int question_choice;
        std::cin >> question_choice;

        if (question_choice == 4) {
            std::cout << "Exiting program.\n";
            break;
        }

        if (question_choice < 1 || question_choice > 3) {
            std::cout << "Invalid choice. Please select 1, 2, 3, or 4.\n";
            continue;
        }

        long long duration_ms = 0;

        if (question_choice == 1) {
            std::cout << "\nChoose Sorting Algorithm:\n";
            std::cout << "1. Bubble Sort\n";
            std::cout << "2. Insertion Sort\n";
            std::cout << "3. Selection Sort\n";
            std::cout << "4. Merge Sort\n";
            std::cout << "Enter choice (1-4): ";
            int sort_choice;
            std::cin >> sort_choice;
            list.sortTransactionsByDate(sort_choice, duration_ms);
        } else if (question_choice == 2) {
            std::cout << "\nChoose Search Algorithm:\n";
            std::cout << "1. Linear Search\n";
            std::cout << "2. Binary Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "3. Jump Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "4. Interpolation Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "Enter choice (1-4): ";
            int search_choice;
            std::cin >> search_choice;
            
            std::cout << "\nChoose Sorting Algorithm:\n";
            std::cout << "1. Bubble Sort\n";
            std::cout << "2. Insertion Sort\n";
            std::cout << "3. Selection Sort\n";
            std::cout << "4. Quick Sort\n";
            std::cout << "Enter choice (1-4): ";
            int sort_choice;
            std::cin >> sort_choice;
            
            list.calculateElectronicsCreditCardPercentage(search_choice, sort_choice, duration_ms);
        } else if (question_choice == 3) {
            std::cout << "\nChoose Sorting Algorithm:\n";
            std::cout << "1. Bubble Sort\n";
            std::cout << "2. Insertion Sort\n";
            std::cout << "3. Selection Sort\n";
            std::cout << "4. Quick Sort\n";
            std::cout << "Enter choice (1-4): ";
            int sort_choice;
            std::cin >> sort_choice;
            list.findFrequentWordsInOneStarReviews(sort_choice, duration_ms);
        }
    }

    return 0;
}
