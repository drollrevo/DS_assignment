#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <string>
#include <iostream>

// Struct to represent a transaction from transactions.csv
struct Transaction {
    std::string customer_id;
    std::string product;
    std::string category;
    double price;
    std::string date; // Standardized to "YYYYMMDD"
    std::string payment_method;
};

// Struct to represent a review from reviews.csv
struct Review {
    std::string product_id;
    std::string customer_id;
    int rating;
    std::string review_text;
};

// Struct to hold word frequency data for Question 3
struct WordFrequency {
    std::string word;
    int count;
};

// Array class to manage dynamic arrays of transactions and reviews
class Array {
private:
    Transaction* transactions;
    int trans_size;
    int trans_capacity;
    Review* reviews;
    int rev_size;
    int rev_capacity;

    void resizeTransactions();
    void resizeReviews();
    void merge(int left, int mid, int right, bool by_category);
    void mergeSortHelper(int left, int right, bool by_category);
    void mergeReviews(int left, int mid, int right);
    void mergeSortReviewsHelper(int left, int right);
    void displaySampleTransactions(const Array& arr, int count = 5);

public:
    Array(int initial_capacity = 10);
    ~Array();

    void addTransaction(const Transaction& t);
    void addReview(const Review& r);
    Transaction getTransaction(int index) const;
    Review getReview(int index) const;
    int getTransSize() const;
    int getRevSize() const;

    // Search Algorithms for Transactions
    int linearSearchByCategory(const std::string& category);
    int binarySearchByCategory(const std::string& category);
    int jumpSearchByCategory(const std::string& category);
    int interpolationSearchByCategory(const std::string& category);

    // Search Algorithms for Reviews
    int linearSearchByRating(int rating);
    int binarySearchByRating(int rating);
    int jumpSearchByRating(int rating);
    int interpolationSearchByRating(int rating);

    // Sort Algorithms for Transactions by Category
    void bubbleSortByCategory();
    void insertionSortByCategory();
    void selectionSortByCategory();
    void mergeSortByCategory();

    // Sort Algorithms for Transactions by Date
    void bubbleSortByDate();
    void insertionSortByDate();
    void selectionSortByDate();
    void mergeSortByDate();

    // Sort Algorithms for Reviews by Rating
    void bubbleSortByRating();
    void mergeSortByRating();

    // Methods for Assignment Questions
    void sortTransactionsByDate(int sort_choice, long long& duration_ms);
    double calculateElectronicsCreditCardPercentage(int search_choice, int sort_choice, long long& duration_ms);
    void findFrequentWordsInOneStarReviews(int search_choice, long long& duration_ms);
};

// Free function declarations
std::string trim(const std::string& str);
std::string to_lowercase(const std::string& str);
void loadTransactions(Array& arr, const std::string& filename);
void loadReviews(Array& arr, const std::string& filename);

#endif // ARRAY_HPP