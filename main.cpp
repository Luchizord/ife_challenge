#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include "configuration.hpp"
 
using namespace boost::system;
using namespace boost::asio;
namespace filesys = boost::filesystem;


/*
 * Get the list of all files in given directory and its sub directories.
 *
 * Arguments
 * 	dirPath : Path of directory to be traversed
 * 	dirSkipList : List of folder names to be skipped
 *
 * Returns:
 * 	vector containing paths of all the files in given directory and its sub directories
 *
 */
std::vector<std::string> getAllFilesInDir(const std::string &dirPath, 	const std::vector<std::string> dirSkipList = { })
{
 
	// Create a vector of string
	std::vector<std::string> listOfFiles;
	try {
		// Check if given path exists and points to a directory
		if (filesys::exists(dirPath) && filesys::is_directory(dirPath))
		{
			// Create a Recursive Directory Iterator object and points to the starting of directory
			filesys::recursive_directory_iterator iter(dirPath);
 
			// Create a Recursive Directory Iterator object pointing to end.
			filesys::recursive_directory_iterator end;
 
			// Iterate till end
			while (iter != end)
			{
				// Check if current entry is a directory and if exists in skip list
				if (filesys::is_directory(iter->path()) &&
						(std::find(dirSkipList.begin(), dirSkipList.end(), iter->path().filename()) != dirSkipList.end()))
				{
					// Boost Fileystsem  API to skip current directory iteration
					iter.no_push();
				}
				else
				{
					// Add the name in vector
					listOfFiles.push_back(iter->path().string());
				}
 
				error_code ec;
				// Increment the iterator to point to next entry in recursive iteration
				iter.increment(ec);
				if (ec) {
					std::cerr << "Error While Accessing : " << iter->path().string() << " :: " << ec.message() << '\n';
				}
			}
		}
	}
	catch (std::system_error & e)
	{
		std::cerr << "Exception :: " << e.what();
	}

	return listOfFiles;
}


/*
 * Get the result of a bash command.
 *
 * Arguments
 * 	cmd : Receive the bash command.
 *
 * Returns:
 * 	string with the result of bash.
 *
 */
std::string GetStdoutFromCommand(std::string cmd) 
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) 
	{
    	while (!feof(stream))
		{
    		if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
		}

    	pclose(stream);
    }

    return data;
}


/*
 * Set the command to use at bash.
 *
 * Arguments
 * 	*str : String that will be modified.
 * 	dir : Directory to be read.
 *
 */
void setCommand (std::string *str, std::string dir)
{
	str->clear();
	str->append("echo ");
	str->append(dir);
	str->append(" |");
	str->append("grep -a ");
}


/*
 * Move a file from the directory A to B.
 *
 * Arguments
 * 	cmd : Bash command
 * 	str : Local of the file
 *  strName : File name
 *  *parameters : Parameters of the configuration file
 *
 */
void moveFile (std::string cmd, std::string str, std::string strName, Configuration *parameters)
{
	std::string auxStr = GetStdoutFromCommand(cmd);

	if (auxStr != "")
	{
		try
		{
			filesys::copy_file(str, parameters->getDirCopy().append(strName));
			filesys::remove(str);

			std::cout << str << " transfered to " << parameters->getDirCopy().append(strName) << std::endl;
			std::cout << "**********************" << std::endl;

		}
		catch (const filesys::filesystem_error& a)
		{
			try
			{
				filesys::remove(parameters->getDirCopy().append(strName));
			}
			catch(const filesys::filesystem_error& b){}

			try
			{
				filesys::copy_file(str, parameters->getDirCopy().append(strName));
				filesys::remove(str);

				std::cout << str << " transfered to " << parameters->getDirCopy().append(strName) << std::endl;
				std::cout << "**********************" << std::endl;
			}
			catch (const filesys::filesystem_error& e)
			{
				// std::cerr << "Error: " << e.what() << std::endl;
			}
		}
	}
}


/*
 * Verify changes in the specified directory at the configuration file
 * 
 * Arguments
 * 	parameters : Parameters of the configuration file
 *
 */
void checkFiles(Configuration* parameters)
{
	// Get recursive list of files in given directory and its sub directories
	std::vector<std::string> listOfFiles = getAllFilesInDir(parameters->getDirSearch());

	// Variables
	std::string strCommand;
	std::string strName;

	// Iterate over the vector and print all files
	for (auto str : listOfFiles)
	{
		strName = &str[parameters->getDirSearch().length()];

		// std::cout << strName << std::endl;

		// Verify if file prefix exist
		if (parameters->getPrefix() != "")
		{
			setCommand(&strCommand, str);

			strCommand.append(parameters->getPrefix());

			moveFile(strCommand, str, strName, parameters);
		}

		// Verify if file type exist
		if (parameters->getFileType() != "")
		{
			setCommand(&strCommand, str);

			strCommand.append(parameters->getFileType());

			moveFile(strCommand, str, strName, parameters);
		}
	}
}


/*
 * Wait function.
 *
 * Arguments
 * 	seconds : seconds in milliseconds
 *
 */
void wait(unsigned long int seconds)
{ 
	boost::this_thread::sleep(boost::posix_time::milliseconds(seconds));
}


// Main function
int main (int argc, char *argv[])
{
	// Get the path of the current application to find the configuration file
	std::string dirPath = argv[1];

	// Get the parameters
	Configuration configParameters(dirPath);

	// loop
	for (;;)
	{
		// Check if any change at the directory ocurred
		checkFiles(&configParameters);
		
		// Wait for the next verification
		wait(configParameters.getTimeMs());
	}

	return 0;
}