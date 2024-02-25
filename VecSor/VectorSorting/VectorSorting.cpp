//============================================================================
// Name        : VectorSorting.cpp
// Author      : Billy Hegarty
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Vector Sorting Algorithms
//============================================================================

#include <algorithm>
#include <iostream>
#include <ctime>
#include <vector>

#include "CSVparser.cpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information... not sure why we are using struct instead of a public class but ok
struct Bid {
    /* The CSV contains the following fields:
     * ArticleTitle (string)
     * ArticleID (int)
     * Department (string)
     * CloseDate (formatted string, mm/dd/yyyy)
     * WinningBid (formatted double, $#,###.##)
     * InventoryID (list of strings, some of which might need to be read as int)
     *
     * Actually... this is dumb but the smaller CSV file, just December monthly sales, has completely different header fields
     * compared to the big CSV file...
     * like Dec2016 has 8 fields, big CSV has 20... doesn't make it very useful for testing
     */

    string bidId; // unique identifier, aka "ArticleID"
    string title; //aka "ArticleTitle"
    string fund; //aka "Department" I guess?
    double amount; //aka "WinningBid" I guess?
    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out) //but we're using namespace std already
 *
 * @param bid struct containing the bid info
 */
void displayBid(const Bid& bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
         << bid.fund << endl;
}

/**
 * Prompt user for bid information using console (std::in)
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore(); //why is this line here?
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
vector<Bid> loadBids(const string& csvPath) {
    cout << "Loading CSV file " << csvPath << endl;

    // Define a vector data structure to hold a collection of bids.
    vector<Bid> bids;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (int i = 0; i < file.rowCount(); i++) {
            // Create a data structure and add to the collection of bids
            // Names ___ or ___ refer to the Dec 2016 sample data or the main eBid Monthly master data set
            Bid bid;
            bid.bidId = file[i][1]; //ArticleID or Auction ID
            bid.title = file[i][0]; //ArticleTitle or Auction Title
            bid.fund = file[i][8]; //Fund or Auction Fee Subtotal ... shouldn't this be [19] for the big data set?
            bid.amount = strToDouble(file[i][4], '$'); //WinningBid or Winning Bid

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bids.push_back(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
    return bids;
}

/**
 * Partition the vector of bids into two parts, low and high
 *
 * @param bids Address of the vector<Bid> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Bid>& bids, int begin, int end) {
    //set low and high equal to begin and end
    int lowIndex = begin;
    int highIndex = end;

    // pick the middle element as pivot point
    int midpoint = lowIndex + (highIndex-lowIndex) / 2;
    Bid pivotBid = bids[midpoint];

    // while not done
    bool done = false;
    while (!done) {
        // keep incrementing low index while bids[low] < bids[pivot]
        while (bids[lowIndex].title < pivotBid.title) {
            ++lowIndex;
        }
        // Keep decrementing high index while pivotBid < bids[high]
        while (pivotBid.title < bids[highIndex].title) {
            --highIndex;
        }

        /* If there are zero or one elements remaining,
        all bids are partitioned. Return high */
        if (lowIndex >= highIndex) {
            done = true; //exit the loop... could also just say "while(true)" and return highIndex here
        }
        else {
            // else swap the low and high bids (built in vector method)
            swap(bids[lowIndex], bids[highIndex]);

            // move low and high closer ++low, --high
            ++lowIndex;
            --highIndex;
        }
    }
    return highIndex;
}

/**
 * Perform a quick sort on bid title
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * @param bids address of the vector<Bid> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Bid>& bids, int begin, int end) {
    //set mid equal to 0, I'm gonna call it lowEndIndex... and actually I don't want to initialize it to 0.
    //I want the partitioning process to set the midpoint (lowEndIdex).
    int lowEndIndex;

    /* Base case: If there are 1 or zero bids to sort, (partition size 1 or 0)
     partition is already sorted... otherwise if begin is greater than or equal to end then return*/
    if (begin >= end) {
        return;
    }

    /* Partition the data (the bids) into low and high such that midpoint is location of last element in low */
    lowEndIndex = partition(bids, begin, end);

    // recursively sort low partition (begin to lowEndIndex)
    quickSort(bids, begin, lowEndIndex);

    // recursively sort high partition
    quickSort(bids, lowEndIndex + 1, end);
}

/**
 * Perform a selection sort on bid title
 * Average performance: O(n^2))
 * Worst case performance O(n^2))
 *
 * @param bid address of the vector<Bid>
 *            instance to be sorted
 */
void selectionSort(vector<Bid>& bids) {
    //define min as int (index of the current minimum bid)
        //I'm gonna call it indexMinBid cause that is more readable
    int indexMinBid; //index of current minimum bid, aka "indexSmallest" in the example code
    int j;
    // check size of bids vector
    // set size_t platform-neutral result equal to bid.size()
        //size_t is an unsigned integer type that is guaranteed to be large enough to represent the size of any object in memory.
    size_t size = bids.size();

    // pos is the position within bids that divides sorted/unsorted
        //ZyBooks uses "i" for pos

    // for size_t pos = 0 and less than size -1
    for (size_t pos = 0; pos < (size-1); ++pos) {
        // set min = pos AKA set indexMinBid = pos
        indexMinBid = pos;

        // loop over remaining elements to the right of position
            //"to the right" means we have to use pos + 1
        // and find the smallest remaining element
        for (j = pos + 1; j < size; ++j) {
            if(bids[j].title < bids[indexMinBid].title) {// if this element's title is less than minimum (so far) title
                indexMinBid = j; // this element becomes the minimum
            }
        }

        // swap the current minimum with smaller one found
            // (swap is a built-in vector method)
        swap(bids[pos], bids[indexMinBid]);
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

//I might change the strToDouble function to the code below... otherwise there won't be conversion error reporting
/*
 *  #include <cstdlib>  // for strtod
    #include <string>

    double strToDouble(const std::string& str, char ch) {
        std::string cleanedStr = str;
        cleanedStr.erase(std::remove(cleanedStr.begin(), cleanedStr.end(), ch), cleanedStr.end());

        char* endPtr;
        double result = strtod(cleanedStr.c_str(), &endPtr);

        // Check for conversion errors
        if (*endPtr != '\0') {
            // Handle error: The conversion was incomplete or failed
            // You may want to throw an exception or return a default value
            throw std::invalid_argument("Invalid floating-point number: " + str);
        }

        return result;
}

 */

int main(int argc, char* argv[]) {
    // process command line arguments
    string csvPath;
    switch (argc) {
        case 2:
            csvPath = argv[1];
            break;
        default:
            //csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
            csvPath = R"(C:\Users\Magnum\Documents\SNHU\5 CS300 24EW3 Data Structures and Algo\VectorSorting\cmake-build-debug\eBid_Monthly_Sales_Dec_2016.csv)";
    }

    cout << "Retrieving bids from: " << csvPath << endl;

    // Define a vector to hold all the bids
    vector<Bid> bids;

    // Define a timer variable
    clock_t ticks;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Selection Sort All Bids" << endl;
        cout << "  4. Quick Sort All Bids" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                // Initialize a timer variable before loading bids
                ticks = clock();

                // Complete the method call to load the bids
                bids = loadBids(csvPath);

                cout << bids.size() << " bids read" << endl;

                // Calculate elapsed time and display result
                ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

                break;

            case 2:
                // Loop and display the bids read
                for (int i = 0; i < bids.size(); ++i) {
                    displayBid(bids[i]);
                }
                cout << endl;

                break;

            case 3:
                // Initialize a timer variable before sorting bids
                ticks = clock();

                // Method call to load the bids
                bids = loadBids(csvPath);

                //Method call to Selection Sort the bids
                selectionSort(bids);

                //Reported the number of bids sorted
                cout << bids.size() << " bids Selection sorted" << endl;

                // Calculate elapsed time and display result
                ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

                break;


            case 4:
                // Initialize a timer variable before sorting bids
                ticks = clock();

                // Method call to load the bids
                bids = loadBids(csvPath);

                //Method call to Quick Sort the bids
                quickSort(bids, 0, bids.size()-1);

                //Reported the number of bids sorted
                cout << bids.size() << " bids Quick sorted" << endl;

                // Calculate elapsed time and display result
                ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

                break;

        }
    }

    cout << "Good bye." << endl;

    return 0;
}
