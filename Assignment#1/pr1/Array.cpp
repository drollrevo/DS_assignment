#include "Array.hpp"
#include <fstream>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

// Constructor
Array::Array(int initial_capacity) {
    trans_capacity = initial_capacity;
    transactions = new Transaction[trans_capacity];
    trans_size = 0;

    rev_capacity = initial_capacity;
    reviews = new Review[rev_capacity];
    rev_size = 0;
}

// Destructor
Array::~Array() {
    delete[] transactions;
    delete[] reviews;
}

// Resize transactions array
void Array::resizeTransactions() {
    trans_capacity *= 2;
    Transaction* new_array = new Transaction[trans_capacity];
    for (int i = 0; i < trans_size; i++) {
        new_array[i] = transactions[i];
    }
    delete[] transactions;
    transactions = new_array;
}

// Resize reviews array
void Array::resizeReviews() {
    rev_capacity *= 2;
    Review* new_array = new Review[rev_capacity];
    for (int i = 0; i < rev_size; i++) {
        new_array[i] = reviews[i];
    }
    delete[] reviews;
    reviews = new_array;
}

// Add a transaction
void Array::addTransaction(const Transaction& t) {
    if (trans_size == trans_capacity) {
        resizeTransactions();
    }
    transactions[trans_size++] = t;
}

// Add a review
void Array::addReview(const Review& r) {
    if (rev_size == rev_capacity) {
        resizeReviews();
    }
    reviews[rev_size++] = r;
}

// Get transaction at index
Transaction Array::getTransaction(int index) const {
    if (index < 0 || index >= trans_size) {
        throw std::out_of_range("Transaction index out of range");
    }
    return transactions[index];
}

// Get review at index
Review Array::getReview(int index) const {
    if (index < 0 || index >= rev_size) {
        throw std::out_of_range("Review index out of range");
    }
    return reviews[index];
}

// Search algorithms for transactions by category (used in Q2)
int Array::linearSearchByCategory(const std::string& category) {
    for (int i = 0; i < trans_size; i++) {
        if (transactions[i].category == category) {
            return i;
        }
    }
    return -1;
}

int Array::binarySearchByCategory(const std::string& category) {
    int low = 0;
    int high = trans_size - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (transactions[mid].category == category) {
            return mid;
        } else if (transactions[mid].category < category) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int Array::jumpSearchByCategory(const std::string& category) {
    int step = static_cast<int>(std::sqrt(trans_size));
    int prev = 0;
    while (prev < trans_size && transactions[prev].category < category) {
        prev += step;
    }
    int start = prev - step;
    if (start < 0) start = 0;
    for (int i = start; i < trans_size && i <= prev; i++) {
        if (transactions[i].category == category) {
            return i;
        }
    }
    return -1;
}

int Array::interpolationSearchByCategory(const std::string& category) {
    int low = 0;
    int high = trans_size - 1;
    while (low <= high && category >= transactions[low].category && category <= transactions[high].category) {
        if (low == high) {
            if (transactions[low].category == category) return low;
            return -1;
        }
        // Interpolation for strings is approximate
        int pos = low + ((high - low) / 2);
        if (transactions[pos].category == category) {
            return pos;
        } else if (transactions[pos].category < category) {
            low = pos + 1;
        } else {
            high = pos - 1;
        }
    }
    return -1;
}

// Search algorithms for reviews by rating (used in Q3)
int Array::linearSearchByRating(int rating) {
    for (int i = 0; i < rev_size; i++) {
        if (reviews[i].rating == rating) {
            return i;
        }
    }
    return -1;
}

int Array::binarySearchByRating(int rating) {
    int low = 0;
    int high = rev_size - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (reviews[mid].rating == rating) {
            return mid;
        } else if (reviews[mid].rating < rating) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int Array::jumpSearchByRating(int rating) {
    int step = static_cast<int>(std::sqrt(rev_size));
    int prev = 0;
    while (prev < rev_size && reviews[prev].rating < rating) {
        prev += step;
    }
    int start = prev - step;
    if (start < 0) start = 0;
    for (int i = start; i < rev_size && i <= prev; i++) {
        if (reviews[i].rating == rating) {
            return i;
        }
    }
    return -1;
}

int Array::interpolationSearchByRating(int rating) {
    int low = 0;
    int high = rev_size - 1;
    while (low <= high && rating >= reviews[low].rating && rating <= reviews[high].rating) {
        if (low == high) {
            if (reviews[low].rating == rating) return low;
            return -1;
        }
        int pos = low + ((rating - reviews[low].rating) * (high - low)) / (reviews[high].rating - reviews[low].rating);
        if (pos < low || pos > high) return -1;
        if (reviews[pos].rating == rating) {
            return pos;
        } else if (reviews[pos].rating < rating) {
            low = pos + 1;
        } else {
            high = pos - 1;
        }
    }
    return -1;
}

// Sort algorithms for transactions by date
void Array::bubbleSortByDate() {
    for (int i = 0; i < trans_size - 1; i++) {
        for (int j = 0; j < trans_size - i - 1; j++) {
            if (transactions[j].date > transactions[j + 1].date) {
                Transaction temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
            }
        }
    }
}

void Array::insertionSortByDate() {
    for (int i = 1; i < trans_size; i++) {
        Transaction key = transactions[i];
        int j = i - 1;
        while (j >= 0 && transactions[j].date > key.date) {
            transactions[j + 1] = transactions[j];
            j--;
        }
        transactions[j + 1] = key;
    }
}

void Array::selectionSortByDate() {
    for (int i = 0; i < trans_size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < trans_size; j++) {
            if (transactions[j].date < transactions[min_idx].date) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Transaction temp = transactions[i];
            transactions[i] = transactions[min_idx];
            transactions[min_idx] = temp;
        }
    }
}

void Array::merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Transaction* L = new Transaction[n1];
    Transaction* R = new Transaction[n2];
    for (int i = 0; i < n1; i++) L[i] = transactions[left + i];
    for (int j = 0; j < n2; j++) R[j] = transactions[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].date <= R[j].date) {
            transactions[k++] = L[i++];
        } else {
            transactions[k++] = R[j++];
        }
    }
    while (i < n1) transactions[k++] = L[i++];
    while (j < n2) transactions[k++] = R[j++];
    delete[] L;
    delete[] R;
}

void Array::mergeSortHelper(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(left, mid);
        mergeSortHelper(mid + 1, right);
        merge(left, mid, right);
    }
}

void Array::mergeSortByDate() {
    if (trans_size > 1) {
        mergeSortHelper(0, trans_size - 1);
    }
}

// Sort algorithms for transactions by category
void Array::bubbleSortByCategory() {
    for (int i = 0; i < trans_size - 1; i++) {
        for (int j = 0; j < trans_size - i - 1; j++) {
            if (transactions[j].category > transactions[j + 1].category) {
                Transaction temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
            }
        }
    }
}

void Array::insertionSortByCategory() {
    for (int i = 1; i < trans_size; i++) {
        Transaction key = transactions[i];
        int j = i - 1;
        while (j >= 0 && transactions[j].category > key.category) {
            transactions[j + 1] = transactions[j];
            j--;
        }
        transactions[j + 1] = key;
    }
}

void Array::selectionSortByCategory() {
    for (int i = 0; i < trans_size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < trans_size; j++) {
            if (transactions[j].category < transactions[min_idx].category) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Transaction temp = transactions[i];
            transactions[i] = transactions[min_idx];
            transactions[min_idx] = temp;
        }
    }
}

void Array::mergeSortByCategory() {
    if (trans_size > 1) {
        // Reuse merge function with category comparison
        auto merge_category = [&](int left, int mid, int right) {
            int n1 = mid - left + 1;
            int n2 = right - mid;
            Transaction* L = new Transaction[n1];
            Transaction* R = new Transaction[n2];
            for (int i = 0; i < n1; i++) L[i] = transactions[left + i];
            for (int j = 0; j < n2; j++) R[j] = transactions[mid + 1 + j];
            int i = 0, j = 0, k = left;
            while (i < n1 && j < n2) {
                if (L[i].category <= R[j].category) {
                    transactions[k++] = L[i++];
                } else {
                    transactions[k++] = R[j++];
                }
            }
            while (i < n1) transactions[k++] = L[i++];
            while (j < n2) transactions[k++] = R[j++];
            delete[] L;
            delete[] R;
        };
        // Fix: Use std::function for recursive lambda
        std::function<void(int, int, decltype(merge_category)&)> merge_sort_helper = 
            [&](int left, int right, auto& merge_fn) {
                if (left < right) {
                    int mid = left + (right - left) / 2;
                    merge_sort_helper(left, mid, merge_fn);
                    merge_sort_helper(mid + 1, right, merge_fn);
                    merge_fn(left, mid, right);
                }
            };
        merge_sort_helper(0, trans_size - 1, merge_category);
    }
}

// Sort algorithms for reviews by rating
void Array::bubbleSortByRating() {
    for (int i = 0; i < rev_size - 1; i++) {
        for (int j = 0; j < rev_size - i - 1; j++) {
            if (reviews[j].rating > reviews[j + 1].rating) {
                Review temp = reviews[j];
                reviews[j] = reviews[j + 1];
                reviews[j + 1] = temp;
            }
        }
    }
}

void Array::insertionSortByRating() {
    for (int i = 1; i < rev_size; i++) {
        Review key = reviews[i];
        int j = i - 1;
        while (j >= 0 && reviews[j].rating > key.rating) {
            reviews[j + 1] = reviews[j];
            j--;
        }
        reviews[j + 1] = key;
    }
}

void Array::selectionSortByRating() {
    for (int i = 0; i < rev_size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < rev_size; j++) {
            if (reviews[j].rating < reviews[min_idx].rating) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Review temp = reviews[i];
            reviews[i] = reviews[min_idx];
            reviews[min_idx] = temp;
        }
    }
}

void Array::mergeSortByRating() {
    if (rev_size > 1) {
        auto merge_rating = [&](int left, int mid, int right) {
            int n1 = mid - left + 1;
            int n2 = right - mid;
            Review* L = new Review[n1];
            Review* R = new Review[n2];
            for (int i = 0; i < n1; i++) L[i] = reviews[left + i];
            for (int j = 0; j < n2; j++) R[j] = reviews[mid + 1 + j];
            int i = 0, j = 0, k = left;
            while (i < n1 && j < n2) {
                if (L[i].rating <= R[j].rating) {
                    reviews[k++] = L[i++];
                } else {
                    reviews[k++] = R[j++];
                }
            }
            while (i < n1) reviews[k++] = L[i++];
            while (j < n2) reviews[k++] = R[j++];
            delete[] L;
            delete[] R;
        };
        // Fix: Use std::function for recursive lambda
        std::function<void(int, int, decltype(merge_rating)&)> merge_sort_helper = 
            [&](int left, int right, auto& merge_fn) {
                if (left < right) {
                    int mid = left + (right - left) / 2;
                    merge_sort_helper(left, mid, merge_fn);
                    merge_sort_helper(mid + 1, right, merge_fn);
                    merge_fn(left, mid, right);
                }
            };
        merge_sort_helper(0, rev_size - 1, merge_rating);
    }
}

// Helper function to standardize date to YYYYMMDD
std::string standardizeDate(const std::string& date_str) {
    std::string day, month, year;
    size_t pos1 = date_str.find('/');
    size_t pos2 = date_str.rfind('/');
    if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 != pos2) {
        day = date_str.substr(0, pos1);
        month = date_str.substr(pos1 + 1, pos2 - pos1 - 1);
        year = date_str.substr(pos2 + 1);
        if (year.length() == 2) year = "20" + year;
        if (day.length() == 1) day = "0" + day;
        if (month.length() == 1) month = "0" + month;
        return year + month + day;
    } else if ((pos1 = date_str.find('-')) != std::string::npos && (pos2 = date_str.rfind('-')) != std::string::npos && pos1 != pos2) {
        // Handle DD-MM-YYYY
        day = date_str.substr(0, pos1);
        month = date_str.substr(pos1 + 1, pos2 - pos1 - 1);
        year = date_str.substr(pos2 + 1);
        if (year.length() == 2) year = "20" + year;
        if (day.length() == 1) day = "0" + day;
        if (month.length() == 1) month = "0" + month;
        return year + month + day;
    }
    return date_str; // Return unchanged if format is invalid
}

// Helper function to display sample transactions
void displaySampleTransactions(const Array& arr, int count = 5) {
    std::cout << "\nSample Sorted Transactions:\n";
    std::cout << std::left << std::setw(12) << "Date"
              << " | " << std::setw(12) << "CustomerID"
              << " | " << std::setw(20) << "Product"
              << " | " << std::setw(10) << "Category"
              << " | " << "Price" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (int i = 0; i < count && i < arr.getTransSize(); ++i) {
        Transaction t = arr.getTransaction(i);
        std::cout << std::setw(12) << t.date << " | "
                  << std::setw(12) << t.customer_id << " | "
                  << std::setw(20) << t.product << " | "
                  << std::setw(10) << t.category << " | "
                  << std::fixed << std::setprecision(2) << t.price << std::endl;
    }
}

// Question 1: Sort transactions by date and display total counts
void Array::sortTransactionsByDate(int sort_choice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    switch (sort_choice) {
        case 1:
            bubbleSortByDate();
            std::cout << "[Bubble Sort] ";
            break;
        case 2:
            insertionSortByDate();
            std::cout << "[Insertion Sort] ";
            break;
        case 3:
            selectionSortByDate();
            std::cout << "[Selection Sort] ";
            break;
        case 4:
            mergeSortByDate();
            std::cout << "[Merge Sort] ";
            break;
        default:
            std::cout << "Invalid sort choice. Using Merge Sort.\n";
            mergeSortByDate();
            std::cout << "[Merge Sort] ";
            break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Execution time: " << duration_ms << " ms\n";
    std::cout << "Total transactions: " << trans_size << "\n";
    std::cout << "Total reviews: " << rev_size << "\n";
    displaySampleTransactions(*this);
}



// // Helper function to convert string to lowercase (include if not already present)
// std::string to_lowercase(const std::string& str) {
//     std::string result = str;
//     std::transform(result.begin(), result.end(), result.begin(), ::tolower);
//     return result;
// }

// // Updated calculateElectronicsCreditCardPercentage for Question 2
// double Array::calculateElectronicsCreditCardPercentage(int search_choice, long long& duration_ms) {
//     int electronics_count = 0;
//     int credit_card_count = 0;
//     auto start = std::chrono::high_resolution_clock::now();

//     // Use linear search logic for all search choices to ensure correct results
//     for (int i = 0; i < trans_size; i++) {
//         if (to_lowercase(transactions[i].category) == "electronics") {
//             electronics_count++;
//             if (to_lowercase(transactions[i].payment_method) == "credit card") {
//                 credit_card_count++;
//             }
//         }
//     }

//     auto end = std::chrono::high_resolution_clock::now();
//     duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

//     std::string algo_name;
//     switch (search_choice) {
//         case 1: algo_name = "Linear Search"; break;
//         case 2: algo_name = "Binary Search"; break;
//         case 3: algo_name = "Jump Search"; break;
//         case 4: algo_name = "Interpolation Search"; break;
//         default: algo_name = "Invalid"; break;
//     }

//     std::cout << "[" << algo_name << "] Execution time: " << duration_ms << " ms\n";
//     std::cout << "Total Electronics Purchases: " << electronics_count << "\n";
//     std::cout << "Electronics Purchases with Credit Card: " << credit_card_count << "\n";
//     double percentage = electronics_count > 0 ? (static_cast<double>(credit_card_count) / electronics_count * 100.0) : 0.0;
//     std::cout << "Percentage: " << std::fixed << std::setprecision(2) << percentage << "%\n";

//     return percentage;
// }

// Question 2: Calculate percentage of Electronics purchases with Credit Card
double Array::calculateElectronicsCreditCardPercentage(int search_choice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    int electronics_count = 0;
    int credit_card_count = 0;

    // Sort for non-linear search algorithms
    if (search_choice >= 2) {
        mergeSortByCategory();
    }

    switch (search_choice) {
        case 1: // Linear Search
            std::cout << "[Linear Search] ";
            for (int i = 0; i < trans_size; i++) {
                if (transactions[i].category == "Electronics") {
                    electronics_count++;
                    if (transactions[i].payment_method == "Credit Card") {
                        credit_card_count++;
                    }
                }
            }
            break;
        case 2: // Binary Search
            std::cout << "[Binary Search] ";
            {
                int idx = binarySearchByCategory("Electronics");
                if (idx != -1) {
                    // Scan left and right for all Electronics
                    for (int i = idx; i >= 0 && transactions[i].category == "Electronics"; i--) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                    for (int i = idx + 1; i < trans_size && transactions[i].category == "Electronics"; i++) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                }
            }
            break;
        case 3: // Jump Search
            std::cout << "[Jump Search] ";
            {
                int idx = jumpSearchByCategory("Electronics");
                if (idx != -1) {
                    for (int i = idx; i >= 0 && transactions[i].category == "Electronics"; i--) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                    for (int i = idx + 1; i < trans_size && transactions[i].category == "Electronics"; i++) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                }
            }
            break;
        case 4: // Interpolation Search
            std::cout << "[Interpolation Search] ";
            {
                int idx = interpolationSearchByCategory("Electronics");
                if (idx != -1) {
                    for (int i = idx; i >= 0 && transactions[i].category == "Electronics"; i--) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                    for (int i = idx + 1; i < trans_size && transactions[i].category == "Electronics"; i++) {
                        electronics_count++;
                        if (transactions[i].payment_method == "Credit Card") {
                            credit_card_count++;
                        }
                    }
                }
            }
            break;
        default:
            std::cout << "Invalid search choice. Using Linear Search.\n";
            std::cout << "[Linear Search] ";
            for (int i = 0; i < trans_size; i++) {
                if (transactions[i].category == "Electronics") {
                    electronics_count++;
                    if (transactions[i].payment_method == "Credit Card") {
                        credit_card_count++;
                    }
                }
            }
            break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double percentage = electronics_count > 0 ? (static_cast<double>(credit_card_count) / electronics_count * 100.0) : 0.0;
    std::cout << "Execution time: " << duration_ms << " ms\n";
    std::cout << "Total Electronics Purchases: " << electronics_count << "\n";
    std::cout << "Electronics Purchases with Credit Card: " << credit_card_count << "\n";
    std::cout << "Percentage: " << std::fixed << std::setprecision(2) << percentage << "%\n";
    return percentage;
}

// Helper function to split text into words
std::string* splitWords(const std::string& text, int& num_words) {
    std::string word;
    std::string* words = new std::string[100]; // Max 100 words per review
    num_words = 0;
    for (char c : text) {
        if (std::isalnum(c)) {
            word += std::tolower(c);
        } else if (!word.empty()) {
            words[num_words++] = word;
            word.clear();
        }
    }
    if (!word.empty()) words[num_words++] = word;
    return words;
}

// Question 3: Find frequent words in 1-star reviews
struct WordFrequency {
    std::string word;
    int count;
};

void Array::findFrequentWordsInOneStarReviews(int search_choice, long long& duration_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    const int MAX_WORDS = 1000;
    WordFrequency word_freq[MAX_WORDS];
    int word_count = 0;

    // Sort for non-linear search algorithms
    if (search_choice >= 2) {
        mergeSortByRating();
    }

    // Collect 1-star reviews
    switch (search_choice) {
        case 1: // Linear Search
            std::cout << "[Linear Search] ";
            for (int i = 0; i < rev_size; i++) {
                if (reviews[i].rating == 1) {
                    int num_words;
                    std::string* words = splitWords(reviews[i].review_text, num_words);
                    for (int j = 0; j < num_words; j++) {
                        std::string word = words[j];
                        bool found = false;
                        for (int k = 0; k < word_count; k++) {
                            if (word_freq[k].word == word) {
                                word_freq[k].count++;
                                found = true;
                                break;
                            }
                        }
                        if (!found && word_count < MAX_WORDS) {
                            word_freq[word_count].word = word;
                            word_freq[word_count].count = 1;
                            word_count++;
                        }
                    }
                    delete[] words;
                }
            }
            break;
        case 2: // Binary Search
            std::cout << "[Binary Search] ";
            {
                int idx = binarySearchByRating(1);
                if (idx != -1) {
                    for (int i = idx; i >= 0 && reviews[i].rating == 1; i--) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                    for (int i = idx + 1; i < rev_size && reviews[i].rating == 1; i++) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                }
            }
            break;
        case 3: // Jump Search
            std::cout << "[Jump Search] ";
            {
                int idx = jumpSearchByRating(1);
                if (idx != -1) {
                    for (int i = idx; i >= 0 && reviews[i].rating == 1; i--) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                    for (int i = idx + 1; i < rev_size && reviews[i].rating == 1; i++) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                }
            }
            break;
        case 4: // Interpolation Search
            std::cout << "[Interpolation Search] ";
            {
                int idx = interpolationSearchByRating(1);
                if (idx != -1) {
                    for (int i = idx; i >= 0 && reviews[i].rating == 1; i--) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                    for (int i = idx + 1; i < rev_size && reviews[i].rating == 1; i++) {
                        int num_words;
                        std::string* words = splitWords(reviews[i].review_text, num_words);
                        for (int j = 0; j < num_words; j++) {
                            std::string word = words[j];
                            bool found = false;
                            for (int k = 0; k < word_count; k++) {
                                if (word_freq[k].word == word) {
                                    word_freq[k].count++;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found && word_count < MAX_WORDS) {
                                word_freq[word_count].word = word;
                                word_freq[word_count].count = 1;
                                word_count++;
                            }
                        }
                        delete[] words;
                    }
                }
            }
            break;
        default:
            std::cout << "Invalid search choice. Using Linear Search.\n";
            std::cout << "[Linear Search] ";
            for (int i = 0; i < rev_size; i++) {
                if (reviews[i].rating == 1) {
                    int num_words;
                    std::string* words = splitWords(reviews[i].review_text, num_words);
                    for (int j = 0; j < num_words; j++) {
                        std::string word = words[j];
                        bool found = false;
                        for (int k = 0; k < word_count; k++) {
                            if (word_freq[k].word == word) {
                                word_freq[k].count++;
                                found = true;
                                break;
                            }
                        }
                        if (!found && word_count < MAX_WORDS) {
                            word_freq[word_count].word = word;
                            word_freq[word_count].count = 1;
                            word_count++;
                        }
                    }
                    delete[] words;
                }
            }
            break;
    }

    // Sort word frequencies by count (descending)
    for (int i = 1; i < word_count; i++) {
        WordFrequency key = word_freq[i];
        int j = i - 1;
        while (j >= 0 && word_freq[j].count < key.count) {
            word_freq[j + 1] = word_freq[j];
            j--;
        }
        word_freq[j + 1] = key;
    }

    auto end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Display top 5 words
    int top_n = 5;
    if (word_count < top_n) top_n = word_count;
    std::cout << "Execution time: " << duration_ms << " ms\n";
    std::cout << "Top " << top_n << " frequent words in 1-star reviews:\n";
    for (int i = 0; i < top_n; i++) {
        std::cout << word_freq[i].word << ": " << word_freq[i].count << "\n";
    }
}

// Load transactions from CSV
void loadTransactions(Array& arr, const std::string& filename) {
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
        arr.addTransaction(t);
    }
    file.close();
}

// Load reviews from CSV
void loadReviews(Array& arr, const std::string& filename) {
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
        arr.addReview(r);
    }
    file.close();
}

// Main function for user interaction
int main() {
    Array arr;
    loadTransactions(arr, "transactions_cleaned.csv");
    loadReviews(arr, "reviews_cleaned.csv");

    while (true) {
        std::cout << "\nE-commerce Transaction Analysis System (Array Implementation)\n";
        std::cout << "==========================================================\n";
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
            arr.sortTransactionsByDate(sort_choice, duration_ms);
        } else if (question_choice == 2) {
            std::cout << "\nChoose Search Algorithm:\n";
            std::cout << "1. Linear Search\n";
            std::cout << "2. Binary Search\n";
            std::cout << "3. Jump Search\n";
            std::cout << "4. Interpolation Search\n";
            std::cout << "Enter choice (1-4): ";
            int search_choice;
            std::cin >> search_choice;
            arr.calculateElectronicsCreditCardPercentage(search_choice, duration_ms);
        } else if (question_choice == 3) {
            std::cout << "\nChoose Search Algorithm:\n";
            std::cout << "1. Linear Search\n";
            std::cout << "2. Binary Search\n";
            std::cout << "3. Jump Search\n";
            std::cout << "4. Interpolation Search\n";
            std::cout << "Enter choice (1-4): ";
            int search_choice;
            std::cin >> search_choice;
            arr.findFrequentWordsInOneStarReviews(search_choice, duration_ms);
        }
    }

    return 0;
}