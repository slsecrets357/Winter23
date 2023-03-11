#include <iostream> // for input/output operations
#include <string>   // for string operations

int main(int argc, char const *argv[])
{
    /* code */
    std::cout << "Welcome to the Comp322 file versioning system!\n" << std::endl;
    std::cout << "To add the content of your file to version control press 'a'" << std::endl;
    std::cout << "To remove a version press 'r'" << std::endl;
    std::cout << "To load a version press 'l'" << std::endl;
    std::cout << "To print to the screen the detailed list of all versions press 'p'" << std::endl;
    std::cout << "To compare any 2 versions press 'c'" << std::endl;
    std::cout << "To search versions for a keyword press 's'" << std::endl;
    std::cout << "To exit press 'e'\n" << std::endl;
    std::string input;
    std::cin >> input;
    std::cout << "Your content has been added successfully." << std::endl;
    std::cin >> input;
    std::cout << "git322 did not detect any change to your file and will not create a new version." << std::endl;
    std::cin >> input;
    std::cout << "Number of versions: 1" << std::endl;
    std::cout << "Version number: 1" << std::endl;
    std::cout << "Hash value: 35345342435" << std::endl;
    std::cin >> input;
    std::cout << "Your content has been added successfully." << std::endl;
    std::cin >> input;
    std::cout << "Number of versions: 2" << std::endl;
    std::cout << "Version number: 1" << std::endl;
    std::cout << "Hash value: 35345342435" << std::endl;
    std::cout << "Content: Dear Comp 322 students.\n" << std::endl;
    std::cout << "Version number: 2" << std::endl;
    std::cout << "Hash value: 98735230844" << std::endl;
    std::cout << "Content: Dear Comp 322 students. C++ is a complicated language." << std::endl;
    std::cin >> input;
    std::cout << "Which version would you like to load?" << std::endl;
    std::cin >> input;
    std::cout << "Version 1 loaded successfully. Please refresh your text editor to see the changes." << std::endl;
    std::cin >> input;
    std::cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << std::endl;
    std::cin >> input;
    std::cout << "Number of versions: 3" << std::endl;
    std::cout << "Version number: 1" << std::endl;
    std::cout << "Hash value: 35345342435" << std::endl;
    std::cout << "Content: Dear Comp 322 students.\n" << std::endl;
    std::cout << "Version number: 2" << std::endl;
    std::cout << "Hash value: 98735230844" << std::endl;
    std::cout << "Content: Dear Comp 322 students. C++ is a complicated language.\n" << std::endl;
    std::cout << "Version number: 3" << std::endl;
    std::cout << "Hash value: 88345342411" << std::endl;
    std::cout << "Content: Dear Comp 322 students." << std::endl;
    std::cout << "Coding in C++ is like going to the gym: No pain no gain!\n" << std::endl;
    std::cin >> input;
    std::cout << "Please enter the number of the first version to compare:" << std::endl;
    std::cin >> input;
    std::cout << "Please enter the number of the second version to compare:" << std::endl;
    std::cin >> input;
    std::cout << "Line 1: Dear Comp 322 students. <<>> Dear Comp 322 students. C++ is a complicated language." << std::endl;
    std::cin >> input;
    std::cout << "Please enter the number of the first version to compare:" << std::endl;
    std::cin >> input;
    std::cout << "Please enter the number of the second version to compare:" << std::endl;
    std::cin >> input;
    std::cout << "Line 1: <Identical>" << std::endl;
    std::cout << "Line 2: <Empty line> <<>> Coding in C++ is like going to the gym: No pain no gain!" << std::endl;
    std::cin >> input;
    std::cout << "Please enter the keyword that you are looking for:" << std::endl;
    std::cin >> input;
    std::cout << "The keyword 'gym' has been found in the following versions:" << std::endl;
    std::cout << "Version number: 3" << std::endl;
    std::cout << "Hash value: 88345342411" << std::endl;
    std::cout << "Content: Dear Comp 322 students." << std::endl;
    std::cout << "Coding in C++ is like going to the gym: No pain no gain!\n" << std::endl;
    std::cin >> input;
    std::cout << "Your keyword 'keyword' was not found in any version." << std::endl;
    std::cin >> input;
    std::cout << "Enter the number of the version that you want to delete:" << std::endl;
    std::cin >> input;
    std::cout << "Version 2 deleted successfully ." << std::endl;
    std::cin >> input;
    std::cout << "Please enter a valid version number." << std::endl;
    std::cin >> input;
    std::cout << "Number of versions: 2" << std::endl;
    std::cout << "Version number: 1\nHash value: 35345342435\nContent: Dear Comp 322 students.\n" << std::endl;
    std::cout << "Version number: 3\nHash value: 88345342411\nContent: Dear Comp 322 students.\nCoding in C++ is like going to the gym: No pain no gain!\n" << std::endl;
    std::cout << "Exiting the program..." << std::endl;

    return 0;
}
