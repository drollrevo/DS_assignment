# Validation Script for Ecommerce Assignment Questions in R
# Checks feasibility of Questions 1, 2, and 3 using transactions.csv and reviews.csv
# Uses base R, no external packages

# Helper function to standardize dates to YYYYMMDD
standardize_date <- function(date_str) {
  parts <- unlist(strsplit(date_str, "/"))
  if (length(parts) == 3) {
    day <- parts[1]
    month <- parts[2]
    year <- parts[3]
    if (nchar(year) == 2) year <- paste0("20", year)
    if (nchar(day) == 1) day <- paste0("0", day)
    if (nchar(month) == 1) month <- paste0("0", month)
    return(paste0(year, month, day))
  }
  return(date_str)
}

# Function to load and validate transactions.csv
load_transactions <- function(filename) {
  tryCatch({
    data <- read.csv(filename, stringsAsFactors = FALSE)
    # Check required columns
    required_cols <- c("Customer.ID", "Product", "Category", "Price", "Date", "Payment.Method")
    if (!all(required_cols %in% colnames(data))) {
      stop("Missing required columns in transactions.csv")
    }
    # Standardize dates
    data$Date <- sapply(data$Date, standardize_date)
    return(data)
  }, error = function(e) {
    cat("Error loading transactions.csv:", conditionMessage(e), "\n")
    return(NULL)
  })
}

# Function to load and validate reviews.csv
load_reviews <- function(filename) {
  tryCatch({
    data <- read.csv(filename, stringsAsFactors = FALSE)
    # Check required columns
    required_cols <- c("Product.ID", "Customer.ID", "Rating", "Review.Text")
    if (!all(required_cols %in% colnames(data))) {
      stop("Missing required columns in reviews.csv")
    }
    return(data)
  }, error = function(e) {
    cat("Error loading reviews.csv:", conditionMessage(e), "\n")
    return(NULL)
  })
}

# Validation for Question 1: Sort transactions by date and display total counts
validate_question_1 <- function(trans_df, rev_df) {
  start_time <- Sys.time()
  if (is.null(trans_df) || is.null(rev_df)) {
    cat("Question 1: Invalid - Data loading failed.\n")
    return()
  }
  # Check if Date column is usable
  if (all(trans_df$Date != "")) {
    # Test sorting feasibility (using base R sort for validation)
    sorted_df <- trans_df[order(trans_df$Date), ]
    trans_count <- nrow(trans_df)
    rev_count <- nrow(rev_df)
    end_time <- Sys.time()
    elapsed_ms <- as.numeric(difftime(end_time, start_time, units = "secs")) * 1000
    cat("Question 1: Valid - Can sort transactions by date and count totals.\n")
    cat("  Sample output: Total transactions =", trans_count, ", Total reviews =", rev_count, "\n")
    cat("  Validation time:", elapsed_ms, "ms\n")
  } else {
    cat("Question 1: Invalid - Date column contains invalid entries.\n")
  }
}

# Validation for Question 2: Percentage of Electronics purchases with Credit Card
validate_question_2 <- function(trans_df) {
  start_time <- Sys.time()
  if (is.null(trans_df)) {
    cat("Question 2: Invalid - Data loading failed.\n")
    return()
  }
  # Check if Category and Payment.Method columns are usable
  if ("Category" %in% colnames(trans_df) && "Payment.Method" %in% colnames(trans_df)) {
    # Test filtering feasibility
    electronics <- trans_df[trans_df$Category == "Electronics", ]
    credit_card <- electronics[electronics$Payment.Method == "Credit Card", ]
    electronics_count <- nrow(electronics)
    credit_card_count <- nrow(credit_card)
    percentage <- if (electronics_count > 0) (credit_card_count / electronics_count) * 100 else 0
    end_time <- Sys.time()
    elapsed_ms <- as.numeric(difftime(end_time, start_time, units = "secs")) * 1000
    cat("Question 2: Valid - Can calculate Electronics Credit Card percentage.\n")
    cat("  Sample output: Percentage =", percentage, "%\n")
    cat("  Validation time:", elapsed_ms, "ms\n")
  } else {
    cat("Question 2: Invalid - Required columns missing.\n")
  }
}

# Validation for Question 3: Most frequent words in 1-star reviews
validate_question_3 <- function(rev_df) {
  start_time <- Sys.time()
  if (is.null(rev_df)) {
    cat("Question 3: Invalid - Data loading failed.\n")
    return()
  }
  # Check if Rating and Review.Text columns are usable
  if ("Rating" %in% colnames(rev_df) && "Review.Text" %in% colnames(rev_df)) {
    # Test text processing feasibility
    one_star <- rev_df[rev_df$Rating == 1, ]
    if (nrow(one_star) > 0) {
      # Process one review as a sample
      sample_text <- one_star$Review.Text[1]
      words <- unlist(strsplit(tolower(gsub("[^[:alnum:]]", " ", sample_text)), "\\s+"))
      word_freq <- table(words)
      sorted_freq <- sort(word_freq, decreasing = TRUE)[1:5]
      end_time <- Sys.time()
      elapsed_ms <- as.numeric(difftime(end_time, start_time, units = "secs")) * 1000
      cat("Question 3: Valid - Can identify frequent words in 1-star reviews.\n")
      cat("  Sample output: Top words =", paste(names(sorted_freq), collapse = ", "), "\n")
      cat("  Validation time:", elapsed_ms, "ms\n")
    } else {
      cat("Question 3: Invalid - No 1-star reviews found.\n")
    }
  } else {
    cat("Question 3: Invalid - Required columns missing.\n")
  }
}

# Main validation function
main <- function() {
  cat("Starting validation for assignment questions...\n")
  
  # Load datasets
  trans_df <- load_transactions("transactions_cleaned.csv")
  rev_df <- load_reviews("reviews_cleaned.csv")
  
  # Validate each question
  cat("\nValidating Question 1...\n")
  validate_question_1(trans_df, rev_df)
  
  cat("\nValidating Question 2...\n")
  validate_question_2(trans_df)
  
  cat("\nValidating Question 3...\n")
  validate_question_3(rev_df)
  
  cat("\nValidation complete.\n")
}

# Run validation
main()





















# R Script for Question 3: Frequent Words in 1-Star Reviews
# Lists top 5 words with their counts from reviews.csv
# Uses base R, no external packages

# Function to load reviews.csv
load_reviews <- function(filename) {
  tryCatch({
    data <- read.csv(filename, stringsAsFactors = FALSE)
    # Check required columns
    required_cols <- c("Product.ID", "Customer.ID", "Rating", "Review.Text")
    if (!all(required_cols %in% colnames(data))) {
      stop("Missing required columns in reviews.csv")
    }
    return(data)
  }, error = function(e) {
    cat("Error loading reviews.csv:", conditionMessage(e), "\n")
    return(NULL)
  })
}

# Function to clean and split text into words
split_words <- function(text) {
  # Convert to lowercase and replace non-alphanumeric with spaces
  clean_text <- tolower(gsub("[^[:alnum:]]", " ", text))
  # Split on whitespace and remove empty strings
  words <- unlist(strsplit(clean_text, "\\s+"))
  words <- words[words != ""]
  return(words)
}

# Function to count word frequencies in 1-star reviews
find_frequent_words <- function(reviews_df) {
  start_time <- Sys.time()
  
  if (is.null(reviews_df)) {
    cat("Cannot process reviews: Data loading failed.\n")
    return()
  }
  
  # Filter for 1-star reviews
  one_star_reviews <- reviews_df[reviews_df$Rating == 1, ]
  if (nrow(one_star_reviews) == 0) {
    cat("No 1-star reviews found.\n")
    return()
  }
  
  # Initialize word frequency table
  word_freq <- list()
  
  # Process each review
  for (text in one_star_reviews$Review.Text) {
    words <- split_words(text)
    # Count each word
    for (word in words) {
      if (nchar(word) > 0) { # Skip empty words
        word_freq[[word]] <- (word_freq[[word]] %||% 0) + 1
      }
    }
  }
  
  # Convert to data frame for sorting
  word_df <- data.frame(
    word = names(word_freq),
    count = unlist(word_freq),
    stringsAsFactors = FALSE
  )
  
  # Sort by count (descending), then alphabetically for ties
  word_df <- word_df[order(-word_df$count, word_df$word), ]
  
  # Select top 5 words
  top_n <- min(5, nrow(word_df))
  top_words <- word_df[1:top_n, ]
  
  # Calculate execution time
  end_time <- Sys.time()
  elapsed_ms <- as.numeric(difftime(end_time, start_time, units = "secs")) * 1000
  
  # Output results
  cat("Top", top_n, "frequent words in 1-star reviews:\n")
  for (i in 1:top_n) {
    cat(top_words$word[i], ":", top_words$count[i], "\n")
  }
  cat("Execution time:", elapsed_ms, "ms\n")
}

# Main function
main <- function() {
  # Load reviews
  reviews_df <- load_reviews("reviews_cleaned.csv")
  
  # Find frequent words
  find_frequent_words(reviews_df)
}

# Run the script
main()
