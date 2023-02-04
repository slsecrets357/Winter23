#include <iostream>
#include <fstream>
using namespace std;
bool word_diff(std::string word1, std::string word2){
    return word1 == word2;
}

bool classical_file_diff(std::string file1, std::string file2){
    std::ifstream myfile1, myfile2;
    myfile1.open(file1);
    myfile2.open(file2);
    while (myfile1.good()&&myfile2.good()){
        std::string l1;
        std::getline(myfile1,l1);
        std::string l2;
        std::getline(myfile2,l2);
        if (!word_diff(l1,l2)){
            return false;
        }
    }
    return true;
}

std::size_t hash_it(std::string someString){
    return std::hash<std::string>{}(someString);
}

bool enhanced_file_diff(std::string file1, std::string file2){
    std::ifstream myfile1, myfile2;
    myfile1.open(file1);
    myfile2.open(file2);
    while (myfile1.good()&&myfile2.good()){
        std::string l1;
        std::getline(myfile1,l1);
        std::string l2;
        std::getline(myfile2,l2);
        if (!(hash_it(l1)==hash_it(l2))){
            return false;
        }
    }
    return true;
}

void list_mismatched_lines(std::string file1, std::string file2){
    std::ifstream f1, f2;
    f1.open(file1);
    f2.open(file2);
    while (f1.good()&&f2.good()){
        std::string l1;
        std::getline(f1,l1);
        std::string l2;
        std::getline(f2,l2);
        if (!(hash_it(l1)==hash_it(l2))){
            cout << file1+": "+l1 << std::endl;
            cout << file2+": "+l2 << std::endl;
        }
        // if (f1.eof()||f2.eof()){
        //     return;
        // }
        // list_mismatched_lines(file1,file2);
    }
    return;
}

void list_mismatched_words(std::string file1, std::string file2){
    std::ifstream f1, f2;
    f1.open(file1);
    f2.open(file2);
    while (f1.good()&&f2.good()){
        std::string s1;
        f1 >> s1;
        std::string s2;
        f2 >> s2;
        if (!(hash_it(s1)==hash_it(s2))){
            cout << file1+": "+s1 << std::endl;
            cout << file2+": "+s2 << std::endl;
        }
    }
    return;
}

int main()
{
// Q1
std::string str1 = "Hello World";
std::string str2 = "hEllO World";
std::string str3 = "World";
std::string str4 = "Hello World";
bool result = word_diff(str1, str2); // False
std::cout << result << std::endl;
result = word_diff(str1, str3); // False
std::cout << result << std::endl;
result = word_diff(str1, str4); // True
std::cout << result << std::endl;
// Q2
std::string file1 = "./txt_folder/file1.txt";
std::string file2 = "./txt_folder/file2.txt";
result = classical_file_diff(file1, file2); // False
std::cout << result << std::endl;
// Q3
std::string mystr = "I love this assignment";
std::size_t h1 = hash_it (mystr);
std::cout << h1 << std::endl;
// Q4
result = enhanced_file_diff(file1, file2); // False
std::cout << result << std::endl;
// Q5
list_mismatched_lines(file1, file2); // This should print to the screen the mismatched lines
// Q6
list_mismatched_words(file1, file2); // This should print to the screen the mismatched words
}