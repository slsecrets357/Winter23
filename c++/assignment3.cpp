#include <iostream> 
#include <string>   
#include <cstring>   
#include <ctime>   
#include <cstdlib>  
#include <cstdio>    
#include <fstream>   
#include <sstream> 

class LinkedList {
private:
    struct Version {
        int versionNumber;     
        std::string hashValue;  
        Version* next;      
        std::string* content;
        int numLines;
    };
    int totalVersions;
    int versionIndex;
    Version* head;
public:
    LinkedList() : totalVersions(0), versionIndex(0), head(nullptr) {}
    ~LinkedList();
    int add(std::string filename);
    int getSize();
    std::string getNodeValue(int index);
    void removeVersion(int versionNumber);
    void loadVersion(int version);
    void print();
    void compareVersions(int version1, int version2);
    void searchVersions(std::string keyword);
    std::string generateHash(std::string content);
    std::size_t hash_it(std::string someString);
};
//new functions
LinkedList::~LinkedList() {
    Version* current = head;
    while (current != nullptr) {
        Version* temp = current;
        current = current->next;
        delete[] temp->content;
        delete temp;
    }
}
int LinkedList::getSize() {
    return totalVersions;
}
std::string LinkedList::getNodeValue(int index) {
    if (index < 0 || index >= totalVersions) {
        throw std::out_of_range("Index out of range");
    }
    Version* current = head;
    int count = 0;
    while (current != nullptr) {
        if (count == index) {
            std::stringstream contentStream;
            for (int i = 0; i < current->numLines; i++) {
                contentStream << current->content[i] << std::endl;
            }
            return contentStream.str();
        }
        current = current->next;
        count++;
    }
    throw std::out_of_range("Index out of range");
}

//old functions
int LinkedList::add(std::string filename) {
    std::ifstream file(filename); // open the file
    // check if file exists
    if (!file.is_open()){ // check if the file is open
        std::cout << "Error: file could not be opened." << std::endl;
        return -1;
    }
    //content is a string* containing every line of the file
    std::string* content = new std::string[100]; // create a string array of size 100
    std::string line; // line of the file
    int i = 0; // index of the string array
    int different = 0;
    while (std::getline(file, line)) { // read the file line by line
        //add the line to the content
        content[i] = line;
        if (head && hash_it(head->content[i]) != hash_it(content[i])){ // if the line is different from the previous version
            different++;
        }
        i++;
    }
    if(head && i != head->numLines) different++; // if the number of lines is different from the previous version
    if (head && !different){
        std::cout << "git322 did not detect any change to your file and will not create a new version." << std::endl;
        return -1;
    }
    Version* newVersion = new Version; // new Version is a pointer to a Version object
    newVersion->versionNumber = ++versionIndex; // initialize the version number to 1
    totalVersions++; // increment the version index
    newVersion->content = content; // initialize the content to the given content
    newVersion->hashValue = generateHash(content[0]); // initialize the hash value to the hash of the given content
    newVersion->numLines = i;
    // add the new version to the end of linked list
    if (head == nullptr){
        head = newVersion;
        head->next = nullptr;
        std::cout << "Your content has been added successfully." << std::endl;
        return 0;
    }
    Version* current = head;
    while(current->next){
        current = current->next;
    }
    current->next = newVersion;
    newVersion->next = nullptr;
    std::cout << "Your content has been added successfully." << std::endl;
    return 0;
}

std::string LinkedList::generateHash(std::string content) {
    return std::to_string(std::hash<std::string>{}(content));
}

std::size_t LinkedList::hash_it(std::string someString) {
    std::hash<std::string> hasher;
    return hasher(someString);
}

void LinkedList::removeVersion(int versionNumber) {
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
            totalVersions--;
            std::cout << "Version " << versionNumber << " has been removed successfully." << std::endl;
            return;
        }
        previous = current;
        current = current->next;
    }
    std::cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << std::endl;
}

void LinkedList::loadVersion(int version) {
    Version* current = head;
    while (current != nullptr){
        if (current->versionNumber == version){
            std::cout << "Version " << version << " loaded successfully. Please refresh your text editor to see the changes." << std::endl;
            // write the content of this version to a "file.txt" file
            std::ofstream file("file.txt");
            // write the content of the version to the file
            std::string *line = current->content;
            while (*line != ""){
                file << *line << std::endl;
                line++;
            }
            file.close();
            return;
        }
        current = current->next;
    }
    std::cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << std::endl;
}

void LinkedList::print() {
    Version* current = head;
    std::cout << "Number of versions: " << totalVersions << std::endl;
    current = head;

    while (current!= nullptr){
        std::cout << "Version number: " << current->versionNumber << std::endl;
        std::cout << "Hash value: " << current->hashValue << std::endl;
        // print the content of the version stored in the content array
        std::string *line = current->content;
        while(*line != ""){
            std::cout << *line << std::endl;
            line++;
        }
        current = current->next;
        std::cout << "" << std::endl;
    }
}

void LinkedList::compareVersions(int versionNumber1, int versionNumber2) {
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
    std::string *content1 = version1->content;
    std::string *content2 = version2->content;
    std::string line1;
    std::string line2;
    int lineCount = 1;
    while (lineCount <= version1->numLines || lineCount <= version2->numLines){
        line1 = *content1;
        line2 = *content2;
        if (lineCount > version1->numLines){
            std::cout << "Line " << lineCount << ": " << "<Empty line>" << " <<>> " << line2 << std::endl;
        } else if (lineCount > version2->numLines){
            std::cout << "Line " << lineCount << ": " << line1 << " <<>> " << "<Empty line>" << std::endl;
        } else if (hash_it(line1) == hash_it(line2)){
            std::cout << "Line " << lineCount << ": <Identical>" << std::endl;
        } else {
            std::cout << "Line " << lineCount << ": " << line1 << " <<>> " << line2 << std::endl;
        }
        content1++;
        content2++;
        lineCount++;
    }
}

void LinkedList::searchVersions(std::string keyword) {
    Version* current = head;
    int found = 0;
    while (current != nullptr){ // loop through all the versions
        std::string *content = current->content; // get the content of the current version
        int lineCount = 1;
        while (lineCount <= current->numLines){ // loop through the lines of the current version
            std::string line = *content;
            if (line.find(keyword) != std::string::npos){ // if the keyword is found in the line
                found++;
                //printing version info
                std::cout << "The keyword '"<<keyword<<"' has been found in the following versions: " << std::endl;
                std::cout << "Version number: " << current->versionNumber << std::endl;
                std::cout << "Hash value: " << current->hashValue << std::endl;
                // print the content of the version stored in the content array
                std::string *line = current->content;
                while(*line != ""){
                    std::cout << *line << std::endl;
                    line++;
                }
                std::cout << "" << std::endl;

                if (current->next==nullptr){ // if the current version is the last version
                    return;
                }
                break; // break out of the loop to avoid printing the same version multiple times
            }
            content++;
            lineCount++;
        }
        current = current->next; // move to the next version
    }
    if(!found) std::cout << "Your keyword " << keyword<< " was not found in any version." << std::endl;
}

class Git322 {
protected:
    LinkedList myList;

public:
    Git322() {} // Default constructor, add any required setup here

    void add(std::string filename) {
        myList.add(filename);
    }

    void removeVersion(int versionNumber) {
        myList.removeVersion(versionNumber);
    }

    void loadVersion(int version) {
        myList.loadVersion(version);
    }

    void print() {
        myList.print();
    }

    void compareVersions(int version1, int version2) {
        myList.compareVersions(version1, version2);
    }

    void searchVersions(std::string keyword) {
        myList.searchVersions(keyword);
    }
};

class EnhancedGit322 : public Git322 {
public:
    EnhancedGit322() {
        loadVersions();
    }

    ~EnhancedGit322() {
        saveVersions();
    }

private:
    void saveVersions() {
        for (size_t i = 0; i < myList.getSize(); i++) {
            std::string versionFilename = "version_" + std::to_string(i) + ".txt";
            std::ofstream outFile(versionFilename);
            outFile << myList.getNodeValue(i);
            outFile.close();
        }
    }

    void loadVersions() {
        size_t versionIndex = 0;
        while (true) {
            std::string versionFilename = "version_" + std::to_string(versionIndex) + ".txt";
            std::ifstream inFile(versionFilename);

            if (!inFile) {
                break;
            }

            std::stringstream buffer;
            buffer << inFile.rdbuf();
            std::string fileContent = buffer.str();
            inFile.close();

            // myList.addNode(fileContent);
            versionIndex++;
        }
    }
};

int main() {
    Git322 git; // Create a Git322 object
    std::string file = "file.txt";
    char choice;
    std::string keyword;
    int loadVersionNumber;
    std::cout << "Welcome to the Comp322 file versioning system!\n" << std::endl;
    std::cout << "To add the content of your file to version control press 'a'" << std::endl;
    std::cout << "To remove a version press 'r'" << std::endl;
    std::cout << "To load a version press 'l'" << std::endl;
    std::cout << "To print to the screen the detailed list of all versions press 'p'" << std::endl;
    std::cout << "To compare any 2 versions press 'c'" << std::endl;
    std::cout << "To search versions for a keyword press 's'" << std::endl;
    std::cout << "To exit press 'e'\n" << std::endl;

    do {
        std::cin >> choice;
        switch (choice){
            case 'a':
                git.add(file);
                break;
            case 'r':
                std::cout << "Please enter the version number to delete:" << std::endl;
                std::cin >> loadVersionNumber;
                git.removeVersion(loadVersionNumber);
                break;
            case 'l':
                std::cout << "Which version would you like to load?" << std::endl;
                std::cin >> loadVersionNumber;
                git.loadVersion(loadVersionNumber);
                break;
            case 'p':
                git.print();
                break;
            case 'c':
                std::cout << "Please enter the number of the first version to compare: " << std::endl;
                std::cin >> loadVersionNumber;
                int versionNumber2;
                std::cout << "Please enter the number of the second version to compare: " << std::endl;
                std::cin >> versionNumber2;
                git.compareVersions(loadVersionNumber, versionNumber2);
                break;
            case 's':
                std::cout << "Please enter the keyword that you are looking for:" << std::endl;
                std::cin.ignore();
                std::getline(std::cin, keyword);
                git.searchVersions(keyword);
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