#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <sstream>

using namespace std;


// Node for linked list
struct node
{
	string content;
	int version;
	size_t hash;
	struct node *prev;
	struct node *next;
};

// Linked list
struct list
{
	int size;
	struct node *head;
	struct node *tail;
};

// Global variable
struct list versions;

// Get file content, parameter is filename, return file content
string get_file_content(string filename);

// Add the content of the file to the tracking system
void add(string content);

// Print versions
void print(void);

// Load the content of a revision into the file
void load(int version);

// Compare the content of 2 versions
void compare(int version1, int version2);

// Search the tracking system
void search(string keyword);

// Remove a version from the tracking system
void remove(int version);

// Calculate hash value
size_t hash_it (string someString);

int main()
{
    struct node *iter;
    // Initalize
	versions.size = 0;
	versions.head = NULL;
	versions.tail = NULL;
	cout << "Welcome to the Comp322 file versioning system!" << endl << endl;
	cout << "To add the content of your file to version control press 'a'" << endl;
	cout << "To remove a version press 'r'" << endl;
	cout << "To load a version press 'l'" << endl;
	cout << "To print to the screen the detailed list of all versions press 'p'" << endl;
	cout << "To compare any 2 versions press 'c'" << endl;
	cout << "To search versions for a keyword press 's'" << endl;
	cout << "To exit press 'e'" << endl << endl;
	while(1)
	{
		string command;
		
		getline(cin, command);
		// Compare command
		if(command.compare("a") == 0)       // Add
		{
			string content = get_file_content("file.txt");
			add(content);
		}
		else if(command.compare("p") == 0)  // Print
		{
			print();
		}
		else if(command.compare("l") == 0)  // Load
		{
			int version;
			string line;
			cout << "Which version would you like to load?" << endl;
			getline(cin, line);
			version = stoi(line);
			load(version);
		}
		else if(command.compare("c") == 0)  // Compare
		{
			int version1, version2;
			string line;
			cout << "Please enter the number of the first version to compare: ";
			getline(cin, line);
			version1 = stoi(line);
			cout << "Please enter the number of the second version to compare: ";
			getline(cin, line);
			version2 = stoi(line);
			compare(version1, version2);
		}
		else if(command.compare("s") == 0)  // Search
		{
			string keyword;
			cout << "Please enter the keyword that you are looking for: ";
			getline(cin, keyword);
			search(keyword);
		}
		else if(command.compare("r") == 0)  // Remove
		{
			int version;
			string line;
			cout << "Enter the number of the version that you want to delete: ";
			getline(cin, line);
			version = stoi(line);
			remove(version);
		}
		else if(command.compare("e") == 0)  // Exit
		{
			break;
		}
		else
		{
			cout << "Invalid choice." << endl;
		}
	}
	// Free all memory
	for(iter = versions.head; iter != NULL;)
	{
	    struct node *temp = iter;
	    iter = iter->next;
	    delete temp;
	}
	return 0;
}

// Calculate hash value
size_t hash_it (string someString)
{
	return std::hash<std::string>()(someString);
}

// Get file content, parameter is filename, return file content
string get_file_content(string filename)
{
	string content;
	ifstream ifs;
	char *buff; 
	int len;
	ifs.open(filename.c_str(), ios::in);
    if(!ifs.is_open())      // If opening is failed
    {
	    return ""; 
    }
    // read content
    ifs.seekg(0, ios::end);
    len = ifs.tellg();  // Calculate the size of file
    ifs.seekg(0, ios::beg);
    buff = new char[len + 1];
    ifs.read(buff, len);    // Read all
    buff[len] = '\0';
    content = buff;
    delete[] buff; 
	return content;
}

// Add the content of the file to the tracking system
void add(string content)
{
	int version = 1;
	int hash = hash_it(content);
	struct node *new_node;
	if(versions.tail != NULL)
	{
		version = versions.tail->version + 1;   // Set the new version as last version + 1
		if(versions.tail->hash == hash)         // If content is the same, will not create
		{
			cout << "git322 did not detect any change to your file and will not create a new version." << endl;
			return;
		}
	}
	// Create a node
	new_node = new struct node;
	if(new_node != NULL)
	{
		new_node->content = content;
		new_node->version = version;
		new_node->hash = hash;
		new_node->prev = versions.tail;
		new_node->next = NULL;
		// Add to linked list
		if(versions.tail == NULL)
		{
			versions.head = new_node;
		}
		else
		{
			versions.tail->next = new_node;
		}
		versions.tail = new_node;
		cout << "Your content has been added successfully." << endl;
		versions.size += 1;
	}
}

// Print versions
void print(void)
{
	struct node *iter;
	cout << "Number of versions: " << versions.size << endl;
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		cout << "Version number: " << iter->version << endl;
		cout << "Hash value: " << iter->hash << endl;
		cout << "Content: " << iter->content << endl << endl;
	}
}

// Load the content of a revision into the file
void load(int version)
{
	struct node *iter;
	ofstream ofs;
	// Find the version
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		if(iter->version == version)
		{
			break;
		}
	}
	if(iter == NULL)    // If not found
	{
		cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << endl;
		return;
	}
	// Load it
	ofs.open("file.txt", ios::out);
	ofs << iter->content;
	ofs.close();
	cout << "Version " << version << " loaded successfully. Please refresh your text editor to see the changes." << endl;
}

// Compare the content of 2 versions
void compare(int version1, int version2)
{
	struct node *iter;
	string str1, str2;
	// Find the versions
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		if(iter->version == version1)
		{
			break;
		}
	}
	if(iter == NULL)    // If version1 is not exist
	{
		cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << endl;
		return;
	}
	str1 = iter->content;
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		if(iter->version == version2)
		{
			break;
		}
	}
	if(iter == NULL)    // If version2 is not exist
	{
		cout << "Please enter a valid version number. If you are not sure please press 'p' to list all valid version numbers." << endl;
		return;
	}
	else
	{
    	str2 = iter->content;
    	// Start compare with str1 and str2
		istringstream iss1(str1);
		istringstream iss2(str2);
		int line = 1, i;
		// split to lines and compare
		while(!iss1.eof() || !iss2.eof())
		{
		    string line1, line2;
		    getline(iss1, line1);   // Read one line
		    getline(iss2, line2);   // Read one line
		    // Remove the special character
	        for(i = 0; i < line1.length(); i++)
	        {
	            if(line1.at(i) == 10 || line1.at(i) == 13)
	            {
	                line1 = line1.substr(0, i);
	                break;
	            }
	        }
	        for(i = 0; i < line2.length(); i++)
	        {
	            if(line2.at(i) == 10 || line2.at(i) == 13)
	            {
	                line2 = line2.substr(0, i);
	                break;
	            }
	        }
	        // If it's the end, exit comparing
		    if(line1.length() == 0 && line2.length() == 0)
		    {
		        if(iss1.eof() && iss2.eof())
		        {
		            break;
		        }
		    }
		    // Compare
	    	if(hash_it(line1) != hash_it(line2))
		    {
    			cout << "Line " << line << ": ";
    			if(line1.length() == 0)
    			{
			    	cout << "<Empty line>";
			    }
			    else
			    {
    				cout << line1;
    			}
			    cout << " <<>> ";
    			if(line2.length() == 0)
    			{
			    	cout << "<Empty line>";
			    }
			    else
			    {
    				cout << line2;
    			}
    			cout << endl;
		    }
		    else
		    {
    			cout << "Line " << line << ": <Identical>" << endl;
    		}
    		line += 1;
		}
	}
}

// Search the tracking system
void search(string keyword)
{
	int found = 0;
	struct node *iter;
	// Find the version
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		if(iter->content.find(keyword) != string::npos) // If found
		{
		    // If this is the first found version
			if(found == 0)
			{
				cout << "The keyword '" << keyword << "' has been found in the following versions:" << endl;
				found = 1;
			}
			cout << "Version number: " << iter->version << endl;
			cout << "Hash value: " << iter->hash << endl;
			cout << "Content: " << iter->content << endl << endl;
		}
	}
	// If not found
	if(found == 0)
	{
		cout << "Your keyword 'keyword' was not found in any version." << endl;
	}
}

// Remove a version from the tracking system
void remove(int version)
{
	struct node *iter;
	// Find the version
	for(iter = versions.head; iter != NULL; iter = iter->next)
	{
		if(iter->version == version)
		{
			break;
		}
	}
	if(iter == NULL)
	{
		cout << "Please enter a valid version number." << endl;
	}
	else
	{
	    // Remove it from the list
		if(iter->prev == NULL)
		{
			versions.head = iter->next;
		}
		else
		{
			iter->prev->next = iter->next;
		}
		if(iter->next == NULL)
		{
			versions.tail = iter->prev;
		}
		else
		{
			iter->next->prev = iter->prev;
		}
		delete iter;
		versions.size -= 1;
		cout << "Version " << version << " deleted successfully." << endl;
	}
}

