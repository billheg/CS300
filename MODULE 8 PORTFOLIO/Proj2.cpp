//============================================================================
// Name        : Proj2.cpp
// Author      : Billy Hegarty
// Date        : 25 Feb 2024
// Description : Project 2 - input course list, print course info and list
//============================================================================

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set> //constant-time access

using namespace std;

class Course {
public:
    //Alternatively, we could encapsulate these 3 members privately, and access them via public Getter() methods
    //But I don't plan on using inheritance or polymorphism
    string courseNumber;
    string courseName;
    vector<string> prereqCourses;

    //Default Constructor
    Course() {
        courseNumber = "";
        courseName = "";
        prereqCourses.clear(); //ensures that the default is populated with empty vector
    }

    //Constructor with passed arguments (not used in this code, but good practice and useful for future projects)
    Course(const string& number, const string& name, const vector<string>& prereqs) {
        courseNumber = number;
        courseName = name;
        prereqCourses = prereqs;
    }
};

// Define Node structure, which will be used to populate the Binary Search Tree
struct Node {
    Course course;      // Course stored in the node
    Node* left;       // Pointer to the left child
    Node* right;      // Pointer to the right child

    // Default constructor with null children
    Node() : left(nullptr), right(nullptr) {}

    //initialize with a passed course
    Node(Course aCourse) :
            Node() {
        course = aCourse;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

class BinarySearchTree {
    //note: Not all methods are used in this project.
    //I've left them in so I can refer back to this code for future projects.
private:
    Node* root;

    void addNode(Node* node, const Course& course);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    Node* removeNode(Node* node, const string& courseNumber);
    void removeSubtree(Node* node);

public:
    BinarySearchTree();
    ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(const Course& course);
    void Remove(const string& courseNumber);
    Course Search(const string& courseNumber);
};

//Default constructor
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

//Destructor
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

    //cout << "Deleting node with course number:: " << node->course.courseNumber;
    delete node;
}

//Public Traversals
void BinarySearchTree::InOrder() {
    // call inOrder function and pass root
    inOrder(root);
}
void BinarySearchTree::PostOrder() {
    // postOrder root
    postOrder(root);
}
void BinarySearchTree::PreOrder() {
    // preOrder root
    preOrder(root);
}
//Public Insert
void BinarySearchTree::Insert(const Course& course) {
    // if root equal to null ptr
    if (root == nullptr) {
        // root is equal to new node course
        root = new Node(course);
    }
    else {
        // add Node root and course
        addNode(root, course);
    }
}

//Public Removal
void BinarySearchTree::Remove(const string& courseNumber) {
    // remove node root courseNumber
    root = removeNode(root, courseNumber);
}
//private removal helper
Node* BinarySearchTree::removeNode(Node* node, const string& courseNumber) {
    // If the node is null, then the node with the courseNumber could not be found in the string.
    if (node == nullptr) {
        return nullptr;
    }

    //function will recursive traverse down left or down right,
    // depending on if the target courseNumber is less than or greater than the courseNumber in the current node

    // If target courseNumber is less than the current node's courseNumber, go left
    if (courseNumber < node->course.courseNumber) {
        node->left = removeNode(node->left, courseNumber);
    } else if (courseNumber > node->course.courseNumber) {
        // If target courseNumber is greater than the current node's courseNumber, go right
        node->right = removeNode(node->right, courseNumber);
    } else {
        //neither greater than nor less than, therefore equals... therefore this is the target node to delete!
        //and we're assuming courseNumber are unique!

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
        //this step is crucial, it ensures that the BST still maintains order
        node->course = temp->course;

        // Delete the inorder successor...
        //really, we are replacing the target node (for deletion) with its inorder successor
        //this way, there are no duplicate nodes in the BST
        node->right = removeNode(node->right, temp->course.courseNumber);
    }

    return node;
}

Course BinarySearchTree::Search(const string& courseNumber) {
    //made courseNumber a constant reference to avoid copying it every invocation

    // set current node equal to root
    Node* current = root;



    // keep looping downwards until bottom reached or matching courseNumber found
    while (current != nullptr) {
        // if match found, return current course
        if (current->course.courseNumber == courseNumber) {
            return current->course;
        }

        // if course is smaller (alphabetically) than current node then traverse left
        //because our BST is alphabetically ordered from left to right
        if (courseNumber < current->course.courseNumber) {
            current = current->left;
        }
        else {// else larger so traverse right
            current = current->right;
        }
    }

    //if we get this far, it means we never hit "return current->course;"
    Course emptyCourse;
    return emptyCourse;
}

//private helper for inserting nodes
void BinarySearchTree::addNode(Node* node, const Course& course) {
    //"course" is the new thing we want to add
    //I made it constant to avoid copying it with every recursive call

    //"node" is the address of a node we are contemplating adding to
    // if existing node is larger (alphabetically), then add the new node to left
    //this step is crucial for keeping our BST alphabetically ordered!
    if(node->course.courseNumber > course.courseNumber)

        if (node->left == nullptr) {// if no left node (left child is null)
            // this "course" becomes left node...create new node containing the course
            node->left = new Node(course);
        }
        else {// else recurse down the left node
            //left node is not null, so need to decide whether course should go left or right of that left child
            addNode(node->left, course);
        }
    else {//the new courseNumber is greater, or equal to, the existing node... go right

        if (node->right == nullptr) {// if no right node...
            //if right child is null, insert new course as right child
            node->right = new Node(course);
        } else {
            // recurse down the right subtree
            addNode(node->right, course);
        }
    }
}

//private helpers for traversal
void BinarySearchTree::inOrder(Node* node) {
    //if node is not equal to null ptr
    if (node != nullptr) {
        inOrder(node->left);

        //output course number, course title
        cout << node->course.courseNumber << ": " << node->course.courseName << endl;

        inOrder(node->right);
    }
}
void BinarySearchTree::postOrder(Node* node) {
    if (node != nullptr) {
        postOrder(node->left);
        postOrder(node->right);
        //output course number, course title
        cout << node->course.courseNumber << ": " << node->course.courseName << endl;
    }
}
void BinarySearchTree::preOrder(Node* node) {
    if (node != nullptr) {
        //output course number, course title
        cout << node->course.courseNumber << ": " << node->course.courseName << endl;
        preOrder(node->left);
        preOrder(node->right);
    }
}


//============================================================================
// Static methods used for menu operations
//============================================================================
void displayCourse(const Course& course) {
    cout << course.courseNumber << ": " << course.courseName << endl;

    // Check if there are prerequisites
    //Personal preference: I intentionally do not want to print anything if there aren't any prereqs
    if (!course.prereqCourses.empty()) {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prereqCourses.size(); ++i) {
            cout << course.prereqCourses[i];

            // Check if it's not the last element
            if (i != course.prereqCourses.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

void loadCourseList(const string& csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    //create new input file stream:
    ifstream inputFile(csvPath);

    //catch error opening file:
    if (!inputFile.is_open()) {
        cout << "Error opening file: " << csvPath << endl;
        return;
    }

    //I ran into 3 weird characters, ∩╗┐, which I learned are "byte order mark" in UTF-8
    //which mark the beginning of a text file... let's isolate and ignore those.
    char byteOrderMarkTrap[3];
    inputFile.read(byteOrderMarkTrap, sizeof(byteOrderMarkTrap));

    if (byteOrderMarkTrap[0] == static_cast<char>(0xEF) &&
        byteOrderMarkTrap[1] == static_cast<char>(0xBB) &&
        byteOrderMarkTrap[2] == static_cast<char>(0xBF)) {
        //ladies and gentlemen... we got 'em
    } else {
        // If not a BOM, rewind to the beginning of the file
        inputFile.seekg(0);
    }

    string line;
    unordered_set<string> courseNumbers;  // To check for duplicate course numbers
    //#todo kill prereqs set
    unordered_set<string> prerequisites;  // To store encountered prerequisites

    //read each line, one at a time:
    while (getline(inputFile, line)) {
        vector<string> courseAttributes;
        string attribute;
        //read in a single line using input string stream
        stringstream ss(line);

        //while there are comma-separated attributes in the line:
        //split the attributes in the line on each comma
        //and append each attribute to courseAttributes

        while (getline(ss, attribute, ',')) {
            courseAttributes.push_back(attribute);
        }

        // Check if there are at least two parameters for a course
        if (courseAttributes.size() < 2) {
            cout << "Error: Course needs at least two parameters." << endl;
            continue;  // Skip to the next line
        }

        // Create a Course object based on attributes that will get stored in BST
        Course tempCourse;
        tempCourse.courseNumber = courseAttributes[0];
        tempCourse.courseName = courseAttributes[1];
        tempCourse.prereqCourses = vector<string>(courseAttributes.begin() + 2, courseAttributes.end());


        // Check for duplicate course numbers
        if (!courseNumbers.insert(tempCourse.courseNumber).second) {
            //We're attempting to insert the courseNumber into the unordered set...
            //insert() should return the iterator of this element...
            //but if the insertion failed because the element already existed in the set,
            //then .second will return false.
            //In other words, this reads: if (attempted insertion failed):
            cout << "Error: Duplicate course number found: " << tempCourse.courseNumber << endl;
            continue;
        }

        //Validate existence of prerequisites:
        /*Need to wait until all courses have been loaded, because they may be listed out of order on the .txt
         * with respect to prerequisite properties.
         *
         * Probably best handled by a separate method altogether... such as:
         * validatePrereqs(unordered_set allCourseNumbers, BinarySearchTree* bst)
         *then perform a traversal (really doesn't matter the order) of the BST
         * at each node:
         *      For each prereq in the course:
         *          try to .find() the prereq inside the unordered_set(allCourseNumbers)
         *              if you reach the .end()
         *                  print error message: "Missing prerequisite"
         *
         * I need to somehow pass along an operator to actually interact with each Node during traversal
         * and that operation could be like "printCourseInfo" or "validatePrereqs" so the operation
         * could be integrated into the printing function... effectively makes InOrder() more modular/versatile
         *
         * But I can't figure out how to pass a function as an argument to another function, without
         * just passing it as a string and then using a switch statement or if-elif conditions.
         * That seems messy and outside the scope of this project.
         * Also, using a directed acyclic graph would make way more sense for preserving prerequisite relationships
         * But that was not an option for this assignment.
         */


        // Insert the Course into the BST
        bst->Insert(tempCourse);

    }

    inputFile.close();
}

int main(int argc, char* argv[]) {
// process command line arguments
    string csvPath, targetCourseNumber;

    switch (argc) {
        case 2:
            //case for .txt filepath being included from the command line
            csvPath = argv[1];
            break;

        default:
            csvPath = "../ABCU_Advising_Program_Input.txt";
    }


    // Define a binary search tree to hold the courses
    BinarySearchTree* bst;
    bst = new BinarySearchTree();
    Course course;

    int choice = 0;
    cout << "Welcome to the course planner." << endl;
    while (choice != 9) {
        cout << "\nMenu:" << endl;
        cout << "  1. Load Course List" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                // Complete the method call to load the courses
                //loadCourseList() calls Insert(), which calls addNode()
                //and addNode() has logic that inserts courses in the BST in alphabetical order
                //so there's no need for additional sorting logic.
                loadCourseList(csvPath, bst);
                break;

            case 2:
                //print all courses using the InOrder() method
                //note: we don't need to do any sorting at this step, because all courses were loaded in alphabetical order
                //and it can handle adding additional courses in the future, via addNode() still in alphabetical order
                cout << "Printing sample schedule: " << endl;
                bst->InOrder();
                break;

            case 3:

                cout << "Which course do you want to know about? ";
                cin >> targetCourseNumber;

                course = bst->Search(targetCourseNumber);

                if (!course.courseNumber.empty()) {
                    //Print the course number, the course title, and a list of prerequisites (if any)
                    displayCourse(course);
                } else {
                    cout << "Sorry, " << targetCourseNumber << " was not found." << endl;
                }

                break;


            default:
                if(choice != 9){
                    cout << "Error: invalid input. Please enter 1, 2, 3, or 9." << endl;
                }
                cin.clear();
                cin.ignore();
                break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
