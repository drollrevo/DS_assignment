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
    void merge(int left, int mid, int right);
    void mergeSortHelper(int left, int right);

public:
    Array(int initial_capacity = 10);
    ~Array();

    void addTransaction(const Transaction& t);
    void addReview(const Review& r);
    Transaction getTransaction(int index) const;
    Review getReview(int index) const;
    int getTransSize() const { return trans_size; }
    int getRevSize() const { return rev_size; }

    // Search algorithms for transactions and reviews
    int linearSearchByCategory(const std::string& category); // Used for Q2
    int binarySearchByCategory(const std::string& category); // Requires sorted by category
    int jumpSearchByCategory(const std::string& category);   // Requires sorted by category
    int interpolationSearchByCategory(const std::string& category); // Requires sorted by category
    int linearSearchByRating(int rating); // Used for Q3
    int binarySearchByRating(int rating); // Requires sorted by rating
    int jumpSearchByRating(int rating);   // Requires sorted by rating
    int interpolationSearchByRating(int rating); // Requires sorted by rating

    // Sort algorithms for transactions by date
    void bubbleSortByDate();
    void insertionSortByDate();
    void selectionSortByDate();
    void mergeSortByDate();

    // Sort algorithms for transactions by category (for Q2 search)
    void bubbleSortByCategory();
    void insertionSortByCategory();
    void selectionSortByCategory();
    void mergeSortByCategory();

    // Sort algorithms for reviews by rating (for Q3 search)
    void bubbleSortByRating();
    void insertionSortByRating();
    void selectionSortByRating();
    void mergeSortByRating();

    // Methods for assignment questions
    void sortTransactionsByDate(int sort_choice, long long& duration_ms);
    double calculateElectronicsCreditCardPercentage(int search_choice, long long& duration_ms);
    void findFrequentWordsInOneStarReviews(int search_choice, long long& duration_ms);
};

// Function prototypes for loading data
void loadTransactions(Array& arr, const std::string& filename);
void loadReviews(Array& arr, const std::string& filename);

#endif // ARRAY_HPP