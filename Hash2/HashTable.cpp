//============================================================================
// Name        : HashTable.cpp
// Author      : Billy Hegarty
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Lab 4-2 HashTable Tables
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <ctime>

#include "CSVparser.cpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;



// define a structure to hold bid information
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
     * like Dec2016 has 8 fields, big CSV has 20...
     */

    string bidId; // unique integer identifier, aka "ArticleID", like 98101
    string title; //aka "ArticleTitle"
    string fund; //aka "Department" I guess?
    double amount; //aka "WinningBid" I guess?
    Bid() {
        //I'm adding other default values here in order to return an empty bid later
        bidId = "";
        title = "";
        fund = "";
        amount = 0.0;
    }
};



// forward declarations
double strToDouble(string str, char ch);

void displayBid(Bid bid);

//============================================================================
// HashTable Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Bid bid;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Bid aBid) : Node() { //this invokes the default constructor of Node
            bid = aBid;
        }

        // initialize with a bid and a key
        Node(Bid aBid, unsigned int aKey) : Node(aBid) {
            key = aKey;
        }
    };

    vector<Node> nodes; //each node in "nodes" will either be a main node (like a particular bid),
    // or a linked list if there are collisions... so for example nodes[1] could potentially contain a linked list
    //and the end of the linked list would be identified by bid.next = nullptr

    unsigned int tableSize = DEFAULT_SIZE; // note, default size is 179

    unsigned int hash(int key); //some integer, which is the result of hashing a key from a node

public:
    HashTable(); //the default constructor resizes the vector "nodes" to contain 179 nodes
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
    //tableSize is set to DEFAULT_SIZE which is 179, so we can index from 0 to 178
    //at this point all we have is a vector containing 179 default Nodes...
    //each node has a default key and next=nullptr,
    // and implicitly initializes the 'bid' member with the default constructor of 'Bid'
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    // resize nodes size
    this->tableSize = size;
    nodes.resize(this->tableSize);
    //can't I just say..... nodes.resize(size)?
    //does it have to be this->nodes...???
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (2): Implement logic to free storage when class is destroyed
    //We have to iterate through each node in the vector "nodes"
    unsigned int i;
    for (i = 0; i < tableSize; ++i) {
        // Start by pointing to the "next" member of the first node, accessed by nodes[i].next
        Node* current = nodes[i].next;
        // Each node could potentially hold a linked list as the result of collisions
        while (current != nullptr) {
            Node* temp = current; //create a temp reference to the i node
            current = current->next; //move "current" to next node in the list
            delete temp; // Delete the temp reference... rinse and repeat until all linked nodes deleted
        } //by the end of this, we will still have a vector filled with nodes, but there will not be any linked lists
    }
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // FIXME (3): Implement logic to calculate a hash value
    //several options... direct hash, double hashing, mid-square hash...
    //let's keep it simple with a simple modulo hash
    return key % tableSize;
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {
    // create the key for the given bid
    //each bid has a "bidId", for example "98121"
    //it is stored as a string in the Bid structure, so we need to convert string to int


    //in order to convert string to int using the "atoi" function, need to convert string to constant char
    //The starter code seems to want me to use atoi... but atoi won't report conversion errors.
    //I'm going to use strtol instead, for better error handling
    string bidIDstring = bid.bidId;
    char* endPtr; //can be used in error handling to find the first invalid character
    long keyLong = strtol(bidIDstring.c_str(), &endPtr, 10);
    int bidIDint = static_cast<int>(keyLong);

    unsigned int key = hash(bidIDint);

    // retrieve node using key... the address of "current" is assigned to the address of the node holding the key
    Node* current = &nodes[key];


    // if the node for that key is default, then a bid hasn't yet been inserted there
    if (current->key == UINT_MAX) { //UINT_MAX is assigned in the default constructor...
        // fill this node with the bid  data
        current->bid = bid;
        current->key = key;
        current->next = nullptr; //the node was previously default, so collision occurs
    }
    else { // COLLISION ALERT! Node has already been accessed with at least 1 bid
        // Chaining technique: Iterate to the end of the linked list, looking for the next open node
        while (current->next != nullptr) {
            current = current->next;
        }
        //now that we've found the end of the linked list, let's make a new node to "append" (link to next)
        //also note that because we're using "new", dynamically allocating memory, we need to "delete" later
        Node* newNode = new Node(bid, key);
        current->next = newNode;
    }
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {
    // FIXME (6): Implement logic to print all bids
    for (unsigned int i = 0; i < tableSize; ++i) { //for each node in the vector...
        Node* current = &nodes[i];  //starting with node 0...
        if (current->key != UINT_MAX) { //if the bid key is non-default (aka if it's populated with a bid)
            //then print bid data!
            // output key, bidID, title, amount and fund
            cout << "Key: " << current->key << ";  ";
            cout << "Bid ID: ";
            displayBid(current->bid);  //note: displayBid will should ID, title, amount, and fund
        }
        current = current->next;
        while (current != nullptr) { //check if there's a link list... if there is, print those bids too
            cout << "Key: " << current->key << ";  ";
            cout << "Bid ID: ";
            displayBid(current->bid);
            current = current->next;
        }
    }
}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
    // FIXME (7): Implement logic to remove a bid
    char* endPtr; //can be used in error handling to find the first invalid character
    long keyLong = strtol(bidId.c_str(), &endPtr, 10);
    int bidIDint = static_cast<int>(keyLong);
    unsigned int key = hash(bidIDint);

    //The key will tell us where to look, within the vector called "nodes"
    //but if any collisions occurred, it's possible that this bid is part of a linked list stemming from it's key node
    //so we need to be careful about preserving the list...
    // We want to delete just the one bid, not all bids whose IDs hashed to the same key

    Node* current = &nodes[key]; //first, navigate to the head node associated with the hash key
    //note, it doesn't matter if this node was populated with a real bid, or just default
    //we initialized all nodes when we created the hash table, so there is a valid address to each Node in nodes

    Node* prev = nullptr; //initialize this for later use


    bool needToDeleteCurrent = false;

    /*********** THREE SCENARIOS
     *  1. The bid exists within the node at nodes[key], and there are no prior collisions (linked list is empty).
        2. The bid exists within the node at nodes[key], and there is a linked list (collision occurred).
        3. The bid is within a linked list at nodes[key], and it's not the first node in the list.
     ****************/

    while (current != nullptr) { //while we haven't hit the tail of the list:
        if (current->bid.bidId == bidId) {//if we find the node associated with the passed bidID...

            if (prev != nullptr) { //this "if" will never be True the first time we enter the while loop

                // Bid is not the first node in the linked list
                // let's cut out the target bid, while preserving the linked list
                prev->next = current->next;
                needToDeleteCurrent = true;
            }
            else { // Bid is the first node in the linked list, because prev == nullptr
                if (current->next != nullptr) { //if a linked list exists in this node...
                    // If there's a next node, let's have the head point to the second node in the linked list
                    // this step bypasses the target bid node

                    Node* temp = current->next;  // Save the address of the second node
                    nodes[key] = *temp; //nodes[key] gets filled with the bid that is at address "temp"
                    //so now, if target bid was the first node, and nodes[key] contains the contents of the second now,
                    //then nodes[key].next should still point to the third node.
                    current = temp;

                    needToDeleteCurrent = true;
                }
                else {
                    // If there's no next node, replace the target bid node with a default-constructed node
                    nodes[key] = Node();
                    //at this point, current still points to the nodes[key] Node... just now it's a default node
                    //don't want to delete current, we still want that node to exist
                    needToDeleteCurrent = false;
                }
            }

            if(needToDeleteCurrent){
                //if the target bid was in a linked list with 2 or more nodes,
                //we want to delete the node that held the target bid to free up memory
                //we do not want to delete the node if no linked list existed, because that would mess up the hashtable
                delete current;
            }

            cout << "Successfully removed bid ID# " << bidId << endl;
            return; //not returning anything, it's void. But this stops the function from continuing.

        }//if

        //we didn't find the bid node on this pass. Traverse to the next node in the list and try again.
        prev = current;
        current = current->next;
    }

    //of course, it's possible that the passed bidID argument was never entered into the hash table...
    cout << "Bid with ID# " << bidId << " not found in hash table." << endl;

}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    Bid blankBid;

    // FIXME (8): Implement logic to search for and return a bid
    //I think this is going to look very similar to the "remove" function I just made...

    // create the key for the given bid
    char* endPtr; //can be used in error handling to find the first invalid character
    long keyLong = strtol(bidId.c_str(), &endPtr, 10);
    int bidIDint = static_cast<int>(keyLong);
    unsigned int key = hash(bidIDint);

    //first, navigate to the head node associated with the hash key
    Node* current = &nodes[key];

    //let's while-loop down the linked list:
    while (current != nullptr) { //while we haven't hit the tail of the list:
        if (current->bid.bidId == bidId) {
            cout << "Ladies and gentlemen, we got 'em... found bid ID# " << bidId << endl;
            return current->bid;
        }
        current = current->next;
    }

    cout << "Error: bid ID# " << bidId << " not found." << endl;
    //return a blank/generic bid
    return blankBid;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
         << bid.fund << endl;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
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

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
        case 2:
            csvPath = argv[1];
            bidKey = "98109";
            break;
        case 3:
            csvPath = argv[1];
            bidKey = argv[2];
            break;
        default:
            csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
            //bidKey = "98109"; //btw, this bid is not contained in the Dec 2016 sample data
            //but we can try bid ID 98268 or 98094 instead
            bidKey = "98094";
            //Try 98273 to test linked list post collision
            //bidKey = "98273";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;
    bidTable = new HashTable();

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

            case 1:

                // Initialize a timer variable before loading bids
                ticks = clock();

                // Complete the method call to load the bids
                loadBids(csvPath, bidTable);

                // Calculate elapsed time and display result
                ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
                break;

            case 2:
                bidTable->PrintAll();
                break;

            case 3:
                ticks = clock();

                bid = bidTable->Search(bidKey);

                ticks = clock() - ticks; // current clock ticks minus starting clock ticks

                if (!bid.bidId.empty()) {
                    displayBid(bid);
                } else {
                    cout << "Bid Id " << bidKey << " not found." << endl;
                }

                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
                break;

            case 4:
                bidTable->Remove(bidKey);
                break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
