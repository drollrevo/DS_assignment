#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <string>

// Struct for transaction data
struct Transaction {
    std::string customer_id;
    std::string product;
    std::string category;
    double price;
    std::string date;
    std::string payment_method;
};

// Struct for review data
struct Review {
    std::string product_id;
    std::string customer_id;
    int rating;
    std::string review_text;
};

// Node for transaction linked list
struct TransactionNode {
    Transaction data;
    TransactionNode* next;
    
    TransactionNode(const Transaction& t) : data(t), next(nullptr) {}
};

// Node for review linked list
struct ReviewNode {
    Review data;
    ReviewNode* next;
    
    ReviewNode(const Review& r) : data(r), next(nullptr) {}
};

// Helper function declaration
std::string standardizeDate(const std::string& date);

// Main LinkedList class
class LinkedList {
private:
    TransactionNode* transactionHead;
    ReviewNode* reviewHead;
    int transactionCount;
    int reviewCount;
    
    // Helper methods for sorting
    void bubbleSortTransactions();
    void insertionSortTransactions();
    void selectionSortTransactions();
    TransactionNode* mergeSortTransactions(TransactionNode* head);
    TransactionNode* getMiddle(TransactionNode* head);
    TransactionNode* sortedMerge(TransactionNode* a, TransactionNode* b);
    
public:
    LinkedList();
    ~LinkedList();
    
    void addTransaction(const Transaction& t);
    void addReview(const Review& r);
    void sortTransactionsByDate(int sortChoice, long long& duration_ms);
    void countTransactionsByDate();
    void calculateElectronicsCreditCardPercentage(int searchChoice, int sortChoice, long long& duration_ms);
    void findFrequentWordsInOneStarReviews(int sortChoice, long long& duration_ms);
};

// Helper functions for loading data
void loadTransactions(LinkedList& list, const std::string& filename);
void loadReviews(LinkedList& list, const std::string& filename);

#endif // LINKED_LIST_HPP
