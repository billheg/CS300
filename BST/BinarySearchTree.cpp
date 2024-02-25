//============================================================================
// Name        : BinarySearchTree.cpp
// Author      : Billy Hegarty
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : BST assignment 5
//============================================================================

#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>

#include "CSVparser.cpp"

using namespace std;



//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch); //took me a while to solve, but this requires #include <algorithm>

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

// Internal structure for tree node
struct Node {
    Bid bid;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a bid
    Node(Bid aBid) :
            Node() {
        bid = aBid;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a binary search tree
 */
class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, const Bid& bid);
    void inOrder(Node* node);
    void postOrder(Node* node);   //I added forward declarations here for lowercase post and preOrder
    void preOrder(Node* node);
    Node* removeNode(Node* node, const string& bidId);
    void removeSubtree(Node* node); //I'm going to use this recursively for the destructor

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();       //added forward decs here for upper case Post and PreOrder...
    void PreOrder();
    void Insert(const Bid& bid);
    void Remove(const string& bidId);
    Bid Search(const string& bidId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    //root is equal to nullptr
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    // recurse from root deleting every node
    //Got some help from this video: https://www.youtube.com/watch?v=ozBhVQNSJ9E
    if (root != nullptr) {
        removeSubtree(root);
    }
}

//recursive helper for destructor:
void BinarySearchTree::removeSubtree(Node* node) {
    if(node->left != nullptr) {
        removeSubtree(node->left); //this will recursively drill down the left-most subtree
    }

    if(node->right != nullptr) {
        removeSubtree(node->right);
    }

    //if we've made it this far, then we've gone as far left and as far right as possible
    //delete the current node, then allow the recursion to unwind and delete trace the parent's right subtree
    //if no right subtree exists, then delete that node and unwind a further level up the tree towards the root

    //cout << "Deleting node with key: " << node->bid.bidId << endl;
    delete node;

    /****
    if (node != nullptr) {
        //if the node that gets passed exists...
        // Recursively delete left and right subtrees... these will drill down the subtrees until reaching nullptr
        removeSubtree(node->left);
        removeSubtree(node->right);

        // Delete the current node
        delete node;
    }
    ****/
}

/**
 * Traverse the tree in order
 */
void BinarySearchTree::InOrder() {
    // call inOrder function and pass root
    inOrder(root);
}

/**
 * Traverse the tree in post-order
 */
void BinarySearchTree::PostOrder() {
    // postOrder root
    postOrder(root);
}

/**
 * Traverse the tree in pre-order
 */
void BinarySearchTree::PreOrder() {
    // preOrder root
    preOrder(root);
}

/**
 * Insert a bid
 */
void BinarySearchTree::Insert(const Bid& bid) {
    // if root equal to null ptr
    if (root == nullptr) {
        // root is equal to new node bid
        root = new Node(bid);
    }
    else {
        // add Node root and bid
        addNode(root, bid);
    }
}

/**
 * Remove a bid
 */
void BinarySearchTree::Remove(const string& bidId) {
    // remove node root bidID
    root = removeNode(root, bidId);
}

//helper method for Remove()
Node* BinarySearchTree::removeNode(Node* node, const string& bidId) {
    // If the node is null, then the node with the bidID could not be found in the string.
    if (node == nullptr) {
        return nullptr;
    }

    //function will recursive traverse down left or down right,
    // depending on if the target bidID is less than or greater than the bidID in the current node

    // If target bidId is less than the current node's bidId, go left
    if (bidId < node->bid.bidId) {
        node->left = removeNode(node->left, bidId);
    } else if (bidId > node->bid.bidId) {
        // If target bidId is greater than the current node's bidId, go right
        node->right = removeNode(node->right, bidId);
    } else {
        //neither greater than nor less than, therefore equals... therefore this is the target node to delete!
        //and we're assuming bidIDs are unique!

        // Case 1: Node with only one child or no child
        if (node->left == nullptr) {
            //left child doesn't exist, so we will pop up the right child and then delete the current node.
            Node* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            //same same but right doesn't exist... pop left
            Node* temp = node->left;
            delete node;
            return temp;
        }

        // Case 2: Node with two children
        // Get the inorder successor (smallest in the right subtree, aka left-most in the right subtree)
        Node* temp = node->right;
        //I wanted to use the existing inOrder() method but that's a void...
        //would need to create a new helper method just for this method... nah let's just insert logic here.
        while (temp->left != nullptr) {
            temp = temp->left;
        }

        // Copy the inorder successor's content to this node
        //this step is crucial, it ensuures that the BST still maintains order
        node->bid = temp->bid;

        // Delete the inorder successor...
        //really, we are replacing the target node (for deletion) with its inorder successor
        //this way, there are no duplicate nodes in the BST
        node->right = removeNode(node->right, temp->bid.bidId);
    }

    return node;
}



/**
 * Search for a bid
 */
Bid BinarySearchTree::Search(const string& bidId) {
    //made bidId a constant reference to avoid copying it every invocation

    // set current node equal to root
    Node* current = root;



    // keep looping downwards until bottom reached or matching bidId found
    while (current != nullptr) {
        // if match found, return current bid
        if (current->bid.bidId == bidId) {
            return current->bid;
        }

        // if bid is smaller than current node then traverse left
        if (bidId < current->bid.bidId) {
            current = current->left;
        }
        else {// else larger so traverse right
            current = current->right;
        }
    }

    //if we get this far, it means we never hit "return current->bid;"
    Bid emptyBid;
    return emptyBid;
}

/**
 * Add a bid to some node (recursive)
 *
 * @param node Current node in tree
 * @param bid Bid to be added
 */
void BinarySearchTree::addNode(Node* node, const Bid& bid) {
    //"bid" is the new thing we want to add
    //I made it constant to avoid copying it with every recursive call

    //"node" is the address of a node we are contemplating adding to
    // if node is larger then add to left
    if(node->bid.bidId > bid.bidId)

        if (node->left == nullptr) {// if no left node (left child is null)
            // this "bid" becomes left node...create new node containing the bid
            node->left = new Node(bid);
        }
        else {// else recurse down the left node
            //left node is not null, so need to decide whether bid should go left or right of that left child
            addNode(node->left, bid);
        }
    else {//the new bidID is greater, or equal to, the existing node... go right

        if (node->right == nullptr) {// if no right node...
            //if right child is null, insert new bid as right child
            node->right = new Node(bid);
        } else {
            // recurse down the right subtree
            addNode(node->right, bid);
        }
    }
}
void BinarySearchTree::inOrder(Node* node) {
    //if node is not equal to null ptr
    if (node != nullptr) {
        inOrder(node->left);

        //output bidID, title, amount, fund
        cout << node->bid.bidId << ": " << node->bid.title << " | " << node->bid.amount << " | " << node->bid.fund << endl;

        inOrder(node->right);
    }
}
void BinarySearchTree::postOrder(Node* node) {
    if (node != nullptr) {
        postOrder(node->left);
        postOrder(node->right);
        //output bidID, title, amount, fund
        cout << node->bid.bidId << ": " << node->bid.title << " | " << node->bid.amount << " | " << node->bid.fund << endl;
    }
}

void BinarySearchTree::preOrder(Node* node) {
    if (node != nullptr) {
        //output bidID, title, amount, fund
        cout << node->bid.bidId << ": " << node->bid.title << " | " << node->bid.amount << " | " << node->bid.fund << endl;
        preOrder(node->left);
        preOrder(node->right);
    }
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(const Bid& bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
         << bid.fund << endl;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(const string& csvPath, BinarySearchTree* bst) {
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
            bst->Insert(bid);
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
    return atof(str.c_str()); //will not report conversion errors, maybe fix later if time
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
            //bidKey = "98109"; //you already know... bid 98109 doesn't exist in the sample data!
            bidKey = "98024"; //this one is "Ice Cream Box", sounds fun!
            break;
        case 3:
            csvPath = argv[1];
            bidKey = argv[2];
            break;
        default:
            csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
            bidKey = "98024"; //bid 98109 doesn't exist in the sample data!
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst;
    bst = new BinarySearchTree();
    Bid bid;

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
                loadBids(csvPath, bst);

                //cout << bst->Size() << " bids read" << endl;

                // Calculate elapsed time and display result
                ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
                break;

            case 2:
                bst->InOrder();
                break;

            case 3:
                ticks = clock();

                bid = bst->Search(bidKey);

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
                bst->Remove(bidKey);
                break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
