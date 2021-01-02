#include <iostream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <regex>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

//open file found with input string, and returns file descriptor (-1 if failure)
//(tested, but may need a few modifications here and there (for error messages/returns?))
int search_and_open(string file_name)
{
	//replace hex with spaces and lowercase all letters
	//(doesn't work with actual space in url, but space shouldn't be in url to begin with?)
	transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);
	int x;
	while ((x = file_name.find("%20")) != string::npos)
	{
		file_name.replace(x, 3, " ");
	}

	//get all files in directory, and iteratively check to see if name matches
	DIR *directory;
	struct dirent *entry;
	string entry_name;
	if ((directory = opendir(".")) != NULL)
	{
		while ((entry = readdir(directory)) != NULL)
		{
			entry_name = entry->d_name;
			transform(entry_name.begin(), entry_name.end(), entry_name.begin(), ::tolower);
			if (entry_name == file_name)
			{
				int file_desc = open(entry->d_name, O_RDONLY);
				if (file_desc > 0)
					return file_desc;
				//may have multiple files that fit critera, and must consider different file permissions
			}
		}
	}
	else
	{
		//error("Cannot open current directory");
		exit(1);
	}
	cout << "Not found" << endl;
	return -1;
}

//optional?
//modified function in main.cpp (not tested, may have errors)
//should remove extra line from response_header
//ADDED:
//new parameter - chunk_size
//reading in  chunks
//added content-length header (we should probably set a chunk size in case, bc some applications have a content limit of 10MB)
//[just making sure the specs are followed for "large files" ; seriously think we should be using buffered writes]
//https://en.wikipedia.org/wiki/Chunked_transfer_encoding
void write_http_response(const int newsockfd, const string &http_response_header, int requested_file_fd, int chunk_size)
{
	if (write(newsockfd, http_response_header.c_str(), sizeof(char) * http_response_header.length()) < 0)
	{
		error("Couldn't write header.");
	}

	//new header
	string buffer;
	string complete_response;
	char input_bytes[60001];	//maximum size of char array - 65535
	while (true)
	{
		//Read a chunk from the file.
		auto read_result{ read(requested_file_fd, &input_bytes, 60000) };
		if (read_result < 0)
		{
			error("Cannot read from socket.");
			//exit(1);?
		}
		else
		{
			string byte_string(input_bytes, read_results);
			// If successful, write that byte.
			complete_response = http_response_header + "Content-Length: " + to_string(read_result) + "/r/n/r/n" + byte_string;
			if (write(newsockfd, byte_string.c_str(), sizeof(char) * cpmlete_response.length()) < 0)
			{
				error("Cannot write to socket");
			}
			//termination after sending body of size 0
			if (read_result == 0)
				break;
		}
	}
}

//ignore - for testing purposes
int main()
{
	string input;
	cin >> input;
	search_and_open(input);
	return 0;
}

//took longer than it should have
//sorry, not very good with c++ and its libraries -_-