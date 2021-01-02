#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <unistd.h>


#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>


using namespace std;

void error(const string &message)
{
    perror(message.c_str());
}

bool is_request_complete(const string &http_request_message)
{
    // HTTP request messages end with the string \r\n\r\n. Because we read the request character by character, as soon as we see
    // this sequence, we know that the request is done.
    return http_request_message.find("\r\n\r\n") != string::npos;
}

optional<string> get_http_request(const int newsockfd)
{
    string http_request_message{};

    while (true)
    {
        char input_byte{};
        auto read_result{read(newsockfd, &input_byte, 1)};

        if (read_result < 0)
        {
            error("ERROR reading from socket");
            return nullopt;
        }

        if (read_result == 0)
        {
            break;
        }

        http_request_message += input_byte;

        if (is_request_complete(http_request_message))
        {
            break;
        }
    }

    return http_request_message;
}

optional<string> get_requested_file_name(const string &http_request_message)
{
    // The first line of an HTTP request is guaranteed to be in the form
    // method URL Version\r\n
    // Taken from Kurose and Ross.
    regex  request_format{R"((\S+) (\S+) (\S+))"};
    smatch matching_substrings{};

    if (regex_search(http_request_message, matching_substrings, request_format))
    {
        return matching_substrings[2];
    }
    else
    {
        cerr << "ERROR, bad request." << endl;
        return nullopt;
    }
}

string decode_URL(string url)
{
    // Comments taken from the URL specification at https://url.spec.whatwg.org/#percent-decode.

    string converted_url{};

    while (!url.empty())
    {
        auto character{url[0]};
        url.erase(0, 1);

        // If byte is not 0x25 (%), then append byte to output.
        if (character != '%')
        {
            converted_url += character;
        }

            // Otherwise, if byte is 0x25 (%) and the next two bytes after byte in input are not in the ranges 0x30 (0)
            // to 0x39 (9), 0x41 (A) to 0x46 (F), and 0x61 (a) to 0x66 (f), all inclusive, append byte to output.
        else if (url.substr(0, 2).length() != 2)
        {
            converted_url += character;
        }
        else if (!isxdigit(url[0]) || !isxdigit(url[1]))
        {
            converted_url += character;
        }

            // Otherwise:
        else
        {
            // Get the two bytes after byte in input, decoded, and then interpreted as hexadecimal number.
            auto code{url.substr(0, 2)};
            auto code_hex{stoi(code, nullptr, 16)};
            auto converted_character{static_cast<char>(code_hex)};

            // Append a byte whose value is bytePoint to output.
            converted_url += converted_character;

            // Skip the next two bytes in input.
            url.erase(0, 2);
        }
    }

    return converted_url;
}

optional<int> open_requested_file(string file_name)
{
    // Convert the filename to lowercase. This will help us to do a case-insensitive filename lookup later.
    transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);

    // Get all files in the current directory, and iteratively check to see if the filename matches.
    DIR    *directory{};
    dirent *entry{};

    if ((directory = opendir(".")) == nullptr)
    {
        error("Cannot open current directory.");
        return nullopt;
    }

    while ((entry = readdir(directory)) != nullptr)
    {
        string entry_name{entry->d_name};
        transform(entry_name.begin(), entry_name.end(), entry_name.begin(), ::tolower);

        if (entry_name == file_name)
        {
            auto file_descriptor{open(entry->d_name, O_RDONLY)};
            if (file_descriptor > 0)
            {
                return file_descriptor;
            }
        }
    }

    return nullopt;
}

optional<string> get_file_extension(const string &file_name)
{
    // Split the filename along the period to get the extension.
    // E.g. myfile.txt ===> matching_string[1] = myfile and matching_string[2] = txt
    regex  filename_format{R"((.*)\.(.*))"};
    smatch matching_string{};

    if (regex_match(file_name, matching_string, filename_format))
    {
        return matching_string[2];
    }
    else
    {
        return nullopt;
    }
}

string get_media_type(optional<string> possible_file_extension)
{
    // If we're not given a file extension, just return the default media type.
    if (!possible_file_extension)
    {
        return "application/octet-stream";
    }

    auto file_extension{possible_file_extension.value()};

    transform(file_extension.begin(), file_extension.end(), file_extension.begin(), ::tolower);

    if (file_extension == "html" || file_extension == "htm")
    {
        return "text/html";
    }

    if (file_extension == "txt")
    {
        return "text/plain";
    }

    if (file_extension == "jpg" || file_extension == "jpeg")
    {
        return "image/jpeg";
    }

    if (file_extension == "gif")
    {
        return "image/gif";
    }

    if (file_extension == "png")
    {
        return "image/png";
    }

    // If we don't know what the media type for the file is...
    return "application/octet-stream";
}

string get_current_datetime()
{
    chrono::system_clock::time_point current_time{chrono::system_clock::now()};
    auto                             current_time_c{chrono::system_clock::to_time_t(current_time)};
    auto                             current_time_gmt{gmtime(&current_time_c)};

    ostringstream output_date{};

    // The syntax of the date command is
    // Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
    // Taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
    output_date << put_time(current_time_gmt, "%a, %d %b %Y %H:%M:%S GMT");
    return output_date.str();
}

string form_http_response_header(const string &status_code, const string &phrase, const string &media_type)
{
    string version{"HTTP/1.1"};

    string response{version + " " + status_code + " " + phrase + "\r\n" +
                    "Content-Type:" + " " + media_type + "\r\n" +
                    "Date:" + " " + get_current_datetime() + "\r\n" +
                    "\r\n"};

    return response;
}

void write_http_response(const int newsockfd, const string &http_response_header, int requested_file_fd)
{
    if (write(newsockfd, http_response_header.c_str(), http_response_header.length()) < 0)
    {
        error("Couldn't write header.");
        return;
    }

    while (true)
    {
        //Read a byte from the file.
        char input_byte{};
        auto read_result{read(requested_file_fd, &input_byte, 1)};

        if (read_result < 0)
        {
            error("Cannot read from socket.");
            return;
        }
        else if (read_result == 0)
        {
            break;
        }
        else
        {
            // If successful, write that byte.
            if (write(newsockfd, &input_byte, 1) < 0)
            {
                error("Cannot write to socket");
                return;
            }
        }
    }
}

void write_404_page(const int newsockfd, const string &http_response_header)
{
    string page{R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>File Not Found</title>
</head>
<body>
<p>404 Not Found</p>
</body>
</html>)"};

    if (write(newsockfd, http_response_header.c_str(), sizeof(char) * http_response_header.length()) < 0)
    {
        error("Couldn't write header");
        return;
    }

    if (write(newsockfd, page.c_str(), sizeof(char) * page.length()) < 0)
    {
        error("Couldn't write 404 page");
        return;
    }
}

void close_connection(const int newsockfd)
{
    if (shutdown(newsockfd, SHUT_RDWR) < 0)
    {
        error("Could not gracefully shut down the socket.");
    }
}

void connect_to_client(int socket_fd)
{
    // Create the "connection socket." We will use this socket to send the file to the client.
    socklen_t   client_length{};
    sockaddr_in client_address{};

    auto newsockfd{accept(socket_fd, static_cast<sockaddr*> &client_address, &client_length)};

    if (newsockfd < 0)
    {
        error("ERROR on accept");
        return;
    }

    // Get the HTTP request message from the client. If something goes wrong with getting the request message, close the connection.
    auto http_request_message{get_http_request(newsockfd)};

    if (!http_request_message)
    {
        close_connection(newsockfd);
        return;
    }


    cout << http_request_message.value() << endl;

    // Using a regex, get the file that the client requested from the HTTP request message. If we can't extract the name of the file,
    // that means that we got a bad request, so close the connection to the client.
    auto requested_file{get_requested_file_name(http_request_message.value())};
    if (!requested_file)
    {
        close_connection(newsockfd);
        return;
    }

    // Remove the first '/' character from the filename. We don't need it.
    requested_file.value().erase(0, 1);

    // Converted %20s in the file name into spaces.
    auto decoded_file_name{decode_URL(requested_file.value())};

    // Open the file. If we can't open the file, return a 404 page.
    auto requested_file_fd{open_requested_file(decoded_file_name)};

    if (requested_file_fd)
    {
        // Get the extension on the file that we need to return. Use the extension to pick the media type that we will
        // stick in our HTTP response header.
        auto file_extension{get_file_extension(decoded_file_name)};
        auto media_type{get_media_type(file_extension)};

        auto http_response_header{form_http_response_header("200", "OK", media_type)};
        write_http_response(newsockfd, http_response_header, requested_file_fd.value());
    }
    else
    {
        auto http_response_header{form_http_response_header("404", "Not Found", "text/html")};
        write_404_page(newsockfd, http_response_header);
    }

    close_connection(newsockfd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "ERROR, no port provided." << endl;
        return 1;
    }

    // Create a "welcoming socket" as it's called in Kurose and Ross. Clients will first connect to this socket.
    // Later, the server will create a "connection socket." The file is served over the connection socket.
    auto socket_fd{socket(AF_INET, SOCK_STREAM, 0)};
    if (socket_fd < 0)
    {
        error("ERROR opening socket");
        return 1;
    }

    // The following two lines fix the "Address already in use" error.
    // See http://beej.us/guide/bgnet/html/single/bgnet.html#setsockoptman
    auto option_value{1};
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));

    // Get the server's IP address.
    sockaddr_in server_address{};
    string      port_number{argv[1]};

    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port        = htons(stoi(port_number)); // This holds the server port number.

    // Attach the server's IP address to the socket.
    if (bind(socket_fd, static_cast<sockaddr*> &server_address, sizeof(server_address)) < 0)
    {
        error("ERROR on binding");
        return 1;
    }

    // Wait until a client contacts the server. Up to 5 clients can wait for the server to open a connection with them.
    listen(socket_fd, 5);

    // In this loop, we will accept a connection to a client and serve its requested file. This is in a loop because
    // once that client is done, we need to connect to another client.
    while (true)
    {
        connect_to_client(socket_fd);
    }
}
