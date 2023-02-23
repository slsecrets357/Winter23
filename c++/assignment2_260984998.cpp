#include <iostream> // for input/output operations
#include <string>   // for string operations
#include <cstring>    // for string operations
#include <ctime>    // for time operations
#include <cstdlib>    // for random number generation
#include <cstdio>     // for file input/output operations
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
// Function to add a new version of the file
void addVersion(Version*& head, std::string content);

// Function to remove a version of the file
void removeVersion(Version*& head, int versionNumber);

// Function to load a specific version and make it the current active one
void loadVersion(Version*& head, int versionNumber);

// Function to print the list of all versions of the file
void printVersions(Version* head);

// Function to compare any two versions of the file
void compareVersions(Version* head, int version1, int version2);

// Function to search and display all versions having a specific keyword
void searchVersions(Version* head, std::string keyword);

// Function to generate a hash value for a given string
std::string generateHash(std::string content);

// main
int main(){

}

// function definitions
void addVersion(Version*& head, std::string content){
    // Version*& head is a reference to a pointer to a Version object
    Version* newVersion = new Version; // new Version is a pointer to a Version object
    newVersion->versionNumber = 1; // initialize the version number to 1
    newVersion->timestamp = std::time(0); // initialize the timestamp to the current time
    newVersion->content = content; // initialize the content to the given content
    newVersion->hashValue = generateHash(content); // initialize the hash value to the hash of the given content
    newVersion->next = head; // initialize the next pointer to the current head
    head = newVersion; // set the head to the new version
}

std::string generateHash(std::string content) {
    // use the std::hash function to generate a hash value for the given string
    return std::to_string(std::hash<std::string>{}(content));
}

void removeVersion(Version*& head, int versionNumber){
    Version* current = head;
    Version* previous = nullptr;
    while (current != nullptr){
        if (current->versionNumber == versionNumber){
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

void loadVersion(Version*& head, int versionNumber){
    Version* current = head;
    while (current != nullptr){
        if (current->versionNumber == versionNumber){
            std::cout << "Loading version " << versionNumber << std::endl;
            std::cout << current->content << std::endl;
            return;
        }
        current = current->next;
    }
}

void printVersions(void){
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
        std::cout << "Content: " << current->content << std::endl;
        current = current->next;
    }
}


