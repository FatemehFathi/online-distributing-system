

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>


using namespace std;

#define IP "127.0.0.1"
#define MESSAGE_SIZE 256


int main(int argc, char *argv[]) {
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int PORT = atoi(argv[1]);
	char buffer[MESSAGE_SIZE];
    struct timeval timeout;
    
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) cerr << "Error Opening Socket!" << endl;
	server = gethostbyname(IP);
	if (server == NULL) {
		cerr << "no such host" << endl;
		exit(0);
	}
    
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(IP);
	serv_addr.sin_port = htons(PORT);
	int temp;
    bool dc = false;
    
	if ((temp=connect(sock_fd, (sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        cerr << "Connecting Error!" << endl;
        dc = true;
        return 0;
    }

    
    ///////////////// select:
    
    fd_set master_fd, read_fd;
	FD_ZERO(&master_fd);
	FD_SET(sock_fd, &master_fd);
	FD_SET(0, &master_fd);
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    

    while (dc == false) {
        string input;

        
        FD_ZERO(&read_fd);
		FD_SET(sock_fd, &read_fd);
		FD_SET(0, &read_fd);
        
		if (select(sock_fd + 1, &read_fd, NULL, NULL, NULL) == -1) {
			cerr << "Select() Error!" << endl;
			exit(1);
		}
   
        if (FD_ISSET(0, &read_fd)) {
            cin >> input;
            bzero(buffer, MESSAGE_SIZE);
            strcpy(buffer, input.c_str());
            
            if (input == "disconnect") {
                dc = true;
            }
            
            //write
            int n = write(sock_fd, buffer, strlen(buffer));
            if (n < 0) cerr << "Error: Writing to the Socket" << endl;
        }
        
        if (FD_ISSET(sock_fd, &read_fd)) {
            //read
            bzero(buffer, MESSAGE_SIZE);
            int n1 = read(sock_fd, buffer, MESSAGE_SIZE);
            if (n1 < 0) cerr << "Error: Reading from the Socket" << endl;
            
            cout << (string)buffer << endl << endl;
        }
        
	} // while(1)
    
	close(sock_fd);
	return 0;
}

                    
                    
                    
