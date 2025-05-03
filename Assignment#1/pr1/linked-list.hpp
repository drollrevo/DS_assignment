#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <string>

// Transaction structure definition
struct Transaction {
    std::string customer_id;
    std::string product;
    std::string category;
    double price;
    std::string date; // Standardized as YYYYMMDD for comparison
    std::string payment_method;
};

// Review structure definition
struct Review {
    std::string product_id;
    std::string customer_id;
    int rating;
    std::string review_text;
};

// Node structures for the linked lists
struct TransactionNode {
    Transaction data;
    TransactionNode* next;

    TransactionNode(const Transaction& t) : data(t), next(nullptr) {}
};

struct ReviewNode {
    Review data;
    ReviewNode* next;

    ReviewNode(const Review& r) : data(r), next(nullptr) {}
};

// LinkedList class
class LinkedList {
private:
    TransactionNode* trans_head;
    ReviewNode* rev_head;
    int trans_size;
    int rev_size;

    // Helper function for merge sort
    TransactionNode* mergeTwoLists(TransactionNode* l1, TransactionNode* l2, bool byDate);
    TransactionNode* findMiddle(TransactionNode* head);
    TransactionNode* mergeSortList(TransactionNode* head, bool byDate);

    // Helper function for reviews merge sort
    ReviewNode* mergeTwoReviewLists(ReviewNode* l1, ReviewNode* l2);
    ReviewNode* findMiddleReview(ReviewNode* head);
    ReviewNode* mergeSortReviewList(ReviewNode* head);

public:
    // Constructor and destructor
    LinkedList();
    ~LinkedList();

    // Basic operations
    void addTransaction(const Transaction& t);
    void addReview(const Review& r);
    int getTransSize() const { return trans_size; }
    int getRevSize() const { return rev_size; }

    // Search operations for transactions
    TransactionNode* linearSearchByCategory(const std::string& category);
    TransactionNode* binarySearchByCategory(const std::string& category);
    TransactionNode* jumpSearchByCategory(const std::string& category);
    TransactionNode* interpolationSearchByCategory(const std::string& category);

    // Search operations for reviews
    ReviewNode* linearSearchByRating(int rating);
    ReviewNode* binarySearchByRating(int rating);
    ReviewNode* jumpSearchByRating(int rating);
    ReviewNode* interpolationSearchByRating(int rating);

    // Sort operations for transactions
    void bubbleSortByDate();
    void insertionSortByDate();
    void selectionSortByDate();
    void mergeSortByDate();

    // Sort operations for transactions by category
    void bubbleSortByCategory();
    void insertionSortByCategory();
    void selectionSortByCategory();
    void mergeSortByCategory();

    // Sort operations for reviews by rating
    void bubbleSortByRating();
    void insertionSortByRating();
    void selectionSortByRating();
    void mergeSortByRating();

    // Analysis functions
    void sortTransactionsByDate(int sort_choice, long long& duration_ms);
    double calculateElectronicsCreditCardPercentage(int search_choice, long long& duration_ms);
    void findFrequentWordsInOneStarReviews(int search_choice, long long& duration_ms);

    // Helper functions to get data for display
    void displayTransactions(int count) const;
    Transaction* getTransactionArray(int* size) const;
    Review* getReviewArray(int* size) const;

    // Extra helper methods
    TransactionNode* getTransHead() const { return trans_head; }
    void setTransHead(TransactionNode* head) { trans_head = head; }
    ReviewNode* getRevHead() const { return rev_head; }
};

#endif // LINKEDLIST_HPP
