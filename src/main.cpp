#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>

#define PORT 7500


using namespace std;

class Request{
    public:
        char* method;
        char* path;
        char* root_path;

        Request(char* method, char* path, char* root_path)
        {
            this->method = method;
            this->path = path;
            this->root_path = root_path;
        }
        
        string get_static_file()
        {
            char* file = "";
            cout << "I get Static" << endl;
            if(strcmp(path,"/") == 0)
            {
                cout<< "Root path" << endl;
                file = "/index.html";
            }
            else
            {

                return "";
            }
            cout << "File: " << file << endl;
            int path_size = strlen(this->root_path) + strlen(file) + 1;
            char* file_path = new char[path_size];
            strcpy(file_path, this->root_path);
            strcat(file_path, file);
            cout << "File path: " << file_path << endl;
            if(is_file(file_path))
            {
                cout << "File exists" << endl;
                return read_file_to_string(file_path);
            }
            cout << "File does not exist" << endl;
            return "";
        }

        bool is_file(char* path)
        {
            ifstream file(path);
            return file.good();
        
        }
        
        string read_file_to_string(const char* path) {
            ifstream file(path);
            if (!file.is_open()) {
                throw runtime_error("Failed to open file: ");
            }
            cout << "Reading file: " << path << endl;
            char* file_buffer = new char[2048];
            string file_str;
            while(file.getline(file_buffer, 2048))
            {
                file_str += file_buffer;
                file_str += "\n";
            }

            file.close();
            delete file_buffer;

            return file_str;
        }
        
};


char* parse_request(char* buffer, vector<char*> &parsed_data)
{
    char* token = strtok(buffer, " ");
    while(token != NULL)
    {
        // std::cout << token << std::endl;
        parsed_data.push_back(token);
        token = strtok(NULL, " ");
    }
    return token;
}

int main()
{

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int address_len = sizeof(address);

    int option = 1;
    char buffer[2048] = {0};

    if(socket_fd == 0)
    {
        std::cout << "Failed to start socket." <<std::endl;
        return -1;
    }

    int set_options = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
        &option, sizeof(option));

    if(set_options == -1)
    {
        std::cout << "Failed to set socket options." << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);


    int port_binding = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if(port_binding < 0)
    {

        std::cout << " Cannot bind to PORT: " << PORT << std::endl;
        return -1;
    }
    std::cout << "Binded to PORT: " << PORT << std::endl;

    if(listen(socket_fd, 15) < 0)
        {
            std::cout << "Failed to listen on socket" << std::endl;
            return -1;

        }
    while(true)
    {
        
        // client communication
        int client_fd = accept(socket_fd, (struct sockaddr *)&address, (socklen_t *)&address_len);
        if(client_fd < 0)
        {
            std::cout << "client connection failed" << std::endl;
            return -1;
        }
        std::cout << "Connected to client.." << std::endl;
        read(client_fd, buffer, 2048);
        std::cout << "Message from client: " << buffer << std::endl;
        vector<char*> parsed_data;
        parse_request(buffer, parsed_data);
        std::cout << "Method: " << parsed_data[0] << std::endl;
        std::cout << "Path: " << parsed_data[1] << std::endl;
        Request request(parsed_data[0], parsed_data[1], "./sample");
        string file_str =request.get_static_file();
        string response;
        if(file_str != "")
        {
            response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + to_string(file_str.size()) + "\n\n" + file_str;
        }
        else
        {
            response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 0\n\n";
        }
        send(client_fd, response.c_str(), response.size(), 0);
        cout << "Response sent to client: "<< response << endl;
        cout << "Closing client connection" << endl;
        close(client_fd);

    }
    close(socket_fd);

    return 0;
}
