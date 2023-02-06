/* README
COMP322 assignment 1 
author: Simon Li - 260984998
Note: to run the code, simply press run. 
Thank you for your cooperation and I wish you an euphoric grading experience:)) 
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
bool word_diff(std::string word1, std::string word2);
bool classical_file_diff(std::string file1, std::string file2);
std::size_t hash_it (std::string someString);
bool enhanced_file_diff(std::string file1, std::string file2);
void list_mismatched_lines(std::string file1, std::string file2);
void list_mismatched_words(std::string file1, std::string file2);

int main()
{
    // Q1
    std::string str1 = "Hello World";
    std::string str2 = "hEllO World";
    std::string str3 = "World";
    std::string str4 = "Hello World";
    bool result = word_diff(str1, str2); // False
    bool result = word_diff(str1, str3); // False
    bool result = word_diff(str1, str4); // True
    // Q2
    std::string file1 = "./txt_folder/file1.txt";
    std::string file2 = "./txt_folder/file2.txt";
    bool result = classical_file_diff(file1, file2); // False
    // Q3
    std::string mystr = "I love this assignment";
    std::size_t h1 = hash_it (mystr);
    std::cout << h1 << std::endl;
    // Q4
    bool result = enhanced_file_diff(file1, file2); // False
    // Q5
    list_mismatched_lines(file1, file2); // This should print to the screen the mismatched lines
    // Q6
    list_mismatched_words(file1, file2); // This should print to the screen the mismatched words
}
bool word_diff(std::string word1, std::string word2){
    return word1.compare(word2)==0;
}
bool classical_file_diff(std::string file1, std::string file2) {
    // Open the first file using an input file stream
    std::ifstream f1(file1);
    // Open the second file using an input file stream
    std::ifstream f2(file2);
    // Create strings to store each line as we read through the files
    std::string line1, line2;

    // While we can still get a line from both files
    while (std::getline(f1, line1) && std::getline(f2, line2)) {
        // If the current lines from both files are different
        if (!word_diff(line1, line2)) {
            // The files are different, so return false
            return false;
        }
    }

    // If we've reached the end of both files without finding any differences
    // The files are identical, so return true
    return f1.eof() && f2.eof();
}
// Function that takes a string as an input parameter and returns the hashed value
std::size_t hash_it(std::string someString) {
    // Create a hasher object using the default hash function
    std::hash<std::string> hasher;
    // Pass the input string to the hasher and return the resulting hash value
    return hasher(someString);
}
bool enhanced_file_diff(std::string file1, std::string file2){
    return hash_it(file1)==hash_it(file2);
}
void list_mismatched_lines(std::string file1, std::string file2){
    // Open the first file using an input file stream
    std::ifstream f1(file1);
    // Open the second file using an input file stream
    std::ifstream f2(file2);
    // Create strings to store each line as we read through the files
    std::string line1, line2;

    // While we can still get a line from both files
    while (std::getline(f1, line1) && std::getline(f2, line2)) {
        // If the current lines from both files are different
        if (!enhanced_file_diff(line1, line2)) {
            // The lines are different, so print
            std::cout << "file1.txt: " << line1 << std::endl;
            std::cout << "file2.txt: " << line2 << std::endl;
        }
    }
}
void list_mismatched_words(std::string file1, std::string file2){
    // Open the first file using an input file stream
    std::ifstream f1(file1);
    // Open the second file using an input file stream
    std::ifstream f2(file2);
    // Create strings to store each line as we read through the files
    std::string line1, line2;
    int line_number = 0;
    // While we can still get a line from both files
    while (std::getline(f1, line1) && std::getline(f2, line2)) {
        // If the current lines from both files are different
        if (!enhanced_file_diff(line1, line2)) {
            std::istringstream iss1(line1);
            std::istringstream iss2(line2);
            std::string word1, word2;
            while(iss1 >> word1 && iss2 >> word2){
                if(!enhanced_file_diff(word1, word2)){
                    std::cout<<file1 << ": " << word1 << " (line " << line_number + 1 << ")" << std::endl;
                    std::cout<<file2 << ": " << word2 << " (line " << line_number + 1 << ")" << std::endl;
                }
            }
        }
        line_number++;
    }
}

/* Thank you for reading through the entire assignment! Please enjoy the rest of your day :D */