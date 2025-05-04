#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <string>
#include <map>
#include <vector>

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

// Helper function to standardize date format
std::string standardizeDate(const std::string& date);

// LinkedList class to manage transactions and reviews
class LinkedList {
private:
    TransactionNode* transactionHead;
    ReviewNode* reviewHead;
    int transactionCount;
    int reviewCount;

    // Helper methods for merge sort
    TransactionNode* getMiddle(TransactionNode* head);
    TransactionNode* sortedMerge(TransactionNode* a, TransactionNode* b);

public:
    LinkedList();
    ~LinkedList();
    
    void addTransaction(const Transaction& t);
    void addReview(const Review& r);
    
    // Sorting methods
    void sortTransactionsByDate(int sortChoice, long long& duration_ms);
    void bubbleSortTransactions();
    void insertionSortTransactions();
    void selectionSortTransactions();
    TransactionNode* mergeSortTransactions(TransactionNode* head);
    
    // Analysis methods
    void countTransactionsByDate();
    void calculateElectronicsCreditCardPercentage(int searchChoice, long long& duration_ms);
    void findFrequentWordsInOneStarReviews(int searchChoice, long long& duration_ms);
};

// File loading functions
void loadTransactions(LinkedList& list, const std::string& filename);
void loadReviews(LinkedList& list, const std::string& filename);

#endif // LINKED_LIST_HPP
