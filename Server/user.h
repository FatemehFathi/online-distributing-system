#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class User {
public:
    User(int fd);
    void next_page();
    void next_half_page();
    void set_file_name(string f);
    
    string get_file_name() { return file_name; }
    int get_sockfd() { return sockfd; }
    int get_page() { return page; }
    int get_half_page() { return half_page; }
private:
    int sockfd;
    string file_name;
    int page;
    int half_page;
};


#endif
