#include <iostream> // for input/output operations
#include <string>   // for string operations
#include <cstring>    // for string operations
#include <ctime>    // for time operations
#include <cstdlib>    // for random number generation
#include <cstdio>     // for file input/output operations
#include <fstream>    // for file input/output operations
/*
    a simple implementation of a git322, a git-like program using dynamic memory management.
    git322 will track one file and will keep all versions of this file in memory. The user
    decides when to take a snapshot of his/her modification. The user should be able to
    load a specific version to make it the current version or can also decide to delete any
    version of the file. Here are the allowed operations while running git322 program:
    1. Adding a version of the file when the user finds suitable
    2. Removing a version (any version that was being tracked)
    3. Loading a specific version and making it the current active one
    4. Printing to the screen the list of all versions of the file
    5. Comparing any 2 versions of the file
    6. Searching and displaying all the versions having a specific keyword
    7. Exiting the program
    git322 has to keep all versions of the file in memory in a linked list using dynamic
    memory allocation. Every element in the list holds information about one version
    of the file.
*/
// function prototypes
struct Version {
    int versionNumber;      // unique identifier for the version
    std::time_t timestamp;  // timestamp of the modification
    std::string content;    // file content    
    std::string hashValue;  // hash value of the file content
    Version* next;      // pointer to the next version
};
//global variables
int versionNumber = 1; // version number
int loadVersionNumber = 1;
Version* head = nullptr; // head of the linked list

// Function to add a new version of the file
int add(std::string filename);

// Function to remove a version of the file
void removeVersion(int versionNumber);

// Function to load a specific version and make it the current active one
void loadVersion(int version);

// Function to print the list of all versions of the file
void print();

// Function to compare any two versions of the file
void compareVersions(int version1, int version2);

// Function to search and display all versions having a specific keyword
void searchVersions(std::string keyword);

// Function to generate a hash value for a given string
std::string generateHash(std::string content);

// main
int main(){
    // Version* head = nullptr; // head of the linked list
    std::string filename; // name of the file
    char choice; // user choice
    std::string keyword; // keyword to search for
    std::cout << "Welcome to the Comp322 file versioning system!" << std::endl;
    std::cout << "To add the content of your file to version control press 'a'" << std::endl;
    std::cout << "To remove a version press 'r'" << std::endl;
    std::cout << "To load a version press 'l'" << std::endl;
    std::cout << "To print to the screen the detailed list of all versions press 'p'" << std::endl;
    std::cout << "To compare any 2 versions press 'c'" << std::endl;
    std::cout << "To search versions for a keyword press 's'" << std::endl;
    std::cout << "To exit press 'e'" << std::endl;
    do {
        std::cin >> choice;
        switch (choice){
            case 'a':
                std::cout << "Please enter the content of the file:" << std::endl;
                std::cin.ignore(); // ignore the newline character which corresponds to the enter key
                std::getline(std::cin, filename);
                add(filename);
                break;
            case 'r':
                std::cout << "Please enter the version number to remove:" << std::endl;
                std::cin >> versionNumber;
                removeVersion(versionNumber);
                break;
            case 'l':
                std::cout << "Which version would you like to load?" << std::endl;
                std::cin >> loadVersionNumber;
                loadVersion(loadVersionNumber);
                break;
            case 'p':
                print();
                break;
            case 'c':
                std::cout << "Please enter the number of the first version to compare: " << std::endl;
                std::cin >> versionNumber;
                int versionNumber2;
                std::cout << "Please enter the number of the second version to compare: " << std::endl;
                std::cin >> versionNumber2;
                compareVersions(versionNumber, versionNumber2);
                break;
            case 's':
                std::cout << "Please enter the keyword to search for:" << std::endl;
                std::cin.ignore();
                std::getline(std::cin, keyword);
                searchVersions(keyword);
                break;
            case 'e':
                std::cout << "Exiting the program..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
    } while (choice != 'e');
    return 0;
}

// function definitions
int add(std::string filename){
    // Version*& head is a reference to a pointer to a Version object
    /*
    If you try to add the same content twice or if you don’t make any modification to your file and
    you try to add the content after it was being added previously the following message should
    be displayed to the screen:
    git322 did not detect any change to your file and will not create a new
    version.
    */
    std::ifstream file(filename); // open the file
    // check if file exists
    if (!file.is_open()){ // check if the file is open
        std::cout << "Error: file could not be opened." << std::endl;
        return -1;
    }
    std::string content; // content of the file
    std::string line; // line of the file
    while (std::getline(file, line)) { // read the file line by line
        content += line + " "; // add the line to the content
    }
    if (head != nullptr && head->content == content){
        std::cout << "git322 did not detect any change to your file and will not create a new version." << std::endl;
        return -1;
    }
    Version* newVersion = new Version; // new Version is a pointer to a Version object
    newVersion->versionNumber = versionNumber++; // initialize the version number to 1
    newVersion->timestamp = std::time(0); // initialize the timestamp to the current time
    newVersion->content = content; // initialize the content to the given content
    newVersion->hashValue = generateHash(content); // initialize the hash value to the hash of the given content
    newVersion->next = head; // initialize the next pointer to the current head
    head = newVersion; // set the head to the new version
    std::cout << "Your content has been added successfully." << std::endl;
    return 0;
}

std::string generateHash(std::string content) {
    // use the std::hash function to generate a hash value for the given string
    return std::to_string(std::hash<std::string>{}(content));
}

void removeVersion(int versionNumber){
    Version* current = head;
    Version* previous = nullptr;
    while (current != nullptr){
        if (current->versionNumber == versionNumber){ // if the version number is found
            if (previous == nullptr){
                head = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;
            return;
        }
        previous = current;
        current = current->next;
    }
}

void loadVersion(int version){
    Version* current = head;
    while (current != nullptr){
        if (current->versionNumber == version){
            std::cout << "Version " << version << " loaded successfully. Please refresh your text editor to see the changes." << std::endl;
            // write the content of this version to a "file.txt" file
            std::ofstream file("file.txt");
            file << current->content;
            file.close();
            // std::cout << current->content << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << std::endl;
}

void print(){
    /* 
    print all the available versions (shown example only has one so far) to the screen. This
    function is handy for debugging. From the menu press ‘p’. Example output:
    Number of versions: 1
    Version number: 1
    Hash value: 35345342435
    Content: Dear Comp 322 students.
    */
    Version* current = head;
    int count = 0;
    // count the number of versions
    while (current != nullptr){ 
        count++;
        current = current->next;
    }
    std::cout << "Number of versions: " << count << std::endl;
    current = head;
    while (current != nullptr){
        std::cout << "Version number: " << current->versionNumber << std::endl;
        std::cout << "Hash value: " << current->hashValue << std::endl;
        std::cout << "Content: " << current->content << "\n" << std::endl;
        current = current->next;
    }
}

void compareVersions(int versionNumber1, int versionNumber2){
    /*
    The following result will be printed to the screen:
    Line 1: Dear Comp 322 students. <<>> Dear Comp 322 students. C++ is a
    complicated language.
    Let’s enter ‘c’ again to compare the first and the 3rd versions
    Please enter the number of the first version to compare:
    The user inputs 1. The following will be printed to the screen:
    Please enter the number of the second version to compare:
    The user inputs 3.
    The following result will be printed to the screen:
    Line 1: <Identical>
    Line 2: <Empty line> <<>> Coding in C++ is like going to the gym: No
    pain no gain!
    */
    Version* current = head;
    Version* version1 = nullptr;
    Version* version2 = nullptr;
    while (current != nullptr){
        if (current->versionNumber == versionNumber1){
            version1 = current;
        }
        if (current->versionNumber == versionNumber2){
            version2 = current;
        }
        current = current->next;
    }
    if (version1 == nullptr || version2 == nullptr){
        std::cout << "One or both of the versions you entered do not exist." << std::endl;
        return;
    }
    if (version1->hashValue == version2->hashValue){
        std::cout << "The two versions are identical." << std::endl;
    } else {
        std::cout << "The two versions are different." << std::endl;
    }
}

void searchVersions(std::string keyword){
    Version* current = head;
    while (current != nullptr){
        if (current->content.find(keyword) != std::string::npos){ //npos is the maximum value for size_t
            std::cout << "Version " << current->versionNumber << " contains the keyword " << keyword << std::endl;
        }
        current = current->next;
    }
}




