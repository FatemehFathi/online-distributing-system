#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/time.h>
#include "user.h"
#include "frame.h"

using namespace std;

#define MAX_CLIENTS 100
#define MESSAGE_SIZE 256


class Server {
public:
    //Server() { }
    
    void run(char* port);
    void get_files_name();
    vector<string> round_robin();
    int page_replacement();
    int second_chance() ;
    int optimal();
    void disc_to_memory(int empty_frame, string file_name, int page_numb);
    
private:
    char main_memory[256][257];
    Frame page_table[256];
    vector<string> files_name;
    vector<User*> rr_queue;
    vector<User*> users;
};


#endif
