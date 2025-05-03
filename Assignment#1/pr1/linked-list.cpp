#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>

// Define structures for transaction and review data
struct Transaction {
    std::string customer_id;
    std::string product;
    std::string category;
    double price;
    std::string date;
    std::string payment_method;
};

struct Review {
    std::string product_id;
    std::string customer_id;
    int rating;
    std::string review_text;
};

// Define nodes for linked lists
struct TransactionNode {
    Transaction data;
    TransactionNode* next;
    
    TransactionNode(Transaction t) : data(t), next(nullptr) {}
};

struct ReviewNode {
    Review data;
    ReviewNode* next;
    
    ReviewNode(Review r) : data(r), next(nullptr) {}
};

// Helper function to standardize date format (assumed to be defined elsewhere)
std::string standardizeDate(const std::string& date) {
    // Implementation would go here
    // For now, just return the date as is
    return date;
}

// LinkedList class to manage transactions and reviews
class LinkedList {
private:
    TransactionNode* transactionHead;
    ReviewNode* reviewHead;
    int transactionCount;
    int reviewCount;

public:
    LinkedList() : transactionHead(nullptr), reviewHead(nullptr), transactionCount(0), reviewCount(0) {}
    
    ~LinkedList() {
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
    
    void addTransaction(const Transaction& t) {
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
    
    void addReview(const Review& r) {
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
    
    // Sort transactions by date using different sorting algorithms
    void sortTransactionsByDate(int sortChoice, long long& duration_ms) {
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
    
    // Bubble Sort implementation for linked list
    void bubbleSortTransactions() {
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
    
    // Insertion Sort implementation for linked list
    void insertionSortTransactions() {
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
    
    // Selection Sort implementation for linked list
    void selectionSortTransactions() {
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
    
    // Merge Sort implementation for linked list
    TransactionNode* mergeSortTransactions(TransactionNode* head) {
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
    
    TransactionNode* getMiddle(TransactionNode* head) {
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
    
    TransactionNode* sortedMerge(TransactionNode* a, TransactionNode* b) {
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
    
    // Count transactions by date and display
    void countTransactionsByDate() {
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
    
    // Calculate Electronics purchases with Credit Card percentage
    void calculateElectronicsCreditCardPercentage(int searchChoice, long long& duration_ms) {
        auto start = std::chrono::high_resolution_clock::now();
        
        int totalElectronics = 0;
        int electronicsCreditCard = 0;
        
        if (searchChoice == 1) {
            // Linear Search
            TransactionNode* current = transactionHead;
            while (current != nullptr) {
                if (current->data.category == "Electronics") {
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
                    totalElectronics++;
                    if (current->data.payment_method == "Credit Card") {
                        electronicsCreditCard++;
                    }
                }
                current = current->next;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        double percentage = (totalElectronics > 0) ? 
            (static_cast<double>(electronicsCreditCard) / totalElectronics) * 100.0 : 0.0;
        
        std::cout << "\nSearch completed in " << duration_ms << " ms\n";
        std::cout << "Total Electronics purchases: " << totalElectronics << "\n";
        std::cout << "Electronics purchases with Credit Card: " << electronicsCreditCard << "\n";
        std::cout << "Percentage: " << percentage << "%\n";
    }
    
    // Find frequent words in 1-star reviews
    void findFrequentWordsInOneStarReviews(int searchChoice, long long& duration_ms) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<std::string> oneStarReviewTexts;
        
        if (searchChoice == 1) {
            // Linear Search
            ReviewNode* current = reviewHead;
            while (current != nullptr) {
                if (current->data.rating == 1) {
                    oneStarReviewTexts.push_back(current->data.review_text);
                }
                current = current->next;
            }
        } else {
            // For other search methods in a linked list, we would need to
            // convert to a vector first, which defeats the purpose of using linked list
            // So we'll use linear search for all cases
            std::cout << "\nNote: Linked lists are best suited for linear search. "
                      << "Other search methods require conversion to arrays.\n";
            
            ReviewNode* current = reviewHead;
            while (current != nullptr) {
                if (current->data.rating == 1) {
                    oneStarReviewTexts.push_back(current->data.review_text);
                }
                current = current->next;
            }
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
        
        auto end = std::chrono::high_resolution_clock::now();
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        // Convert to vector for sorting
        std::vector<std::pair<std::string, int>> wordFreqVec(wordFrequency.begin(), wordFrequency.end());
        std::sort(wordFreqVec.begin(), wordFreqVec.end(), 
            [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::cout << "\nSearch completed in " << duration_ms << " ms\n";
        std::cout << "Number of 1-star reviews: " << oneStarReviewTexts.size() << "\n";
        std::cout << "Most frequent words in 1-star reviews:\n";
        
        int count = 0;
        for (const auto& pair : wordFreqVec) {
            if (count++ >= 10) break; // Show top 10
            std::cout << pair.first << ": " << pair.second << " occurrences\n";
        }
    }
};

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
            list.calculateElectronicsCreditCardPercentage(search_choice, duration_ms);
        } else if (question_choice == 3) {
            std::cout << "\nChoose Search Algorithm:\n";
            std::cout << "1. Linear Search\n";
            std::cout << "2. Binary Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "3. Jump Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "4. Interpolation Search (Note: Using Linear Search for Linked List)\n";
            std::cout << "Enter choice (1-4): ";
            int search_choice;
            std::cin >> search_choice;
            list.findFrequentWordsInOneStarReviews(search_choice, duration_ms);
        }
    }

    return 0;
}
