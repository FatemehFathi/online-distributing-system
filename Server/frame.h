#ifndef __FRAME_H__
#define __FRAME_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>


using namespace std;

#define MESSAGE_SIZE 256
#define TRUE 1
#define FALSE 0


class Frame {
public:
    Frame();
    Frame(string n);
    
    void set_file_name(string n) { file_name = n; }
    void set_page_number(int p) { page_number = p; }
    void set_recently_access(bool v) { recently_access = v; }
    void set_is_eof(bool v) { is_eof = v; }
    void set_is_half(bool v) { is_half = v; }
    
    string get_file_name() { return file_name; }
    int get_page_number() { return page_number; }
    bool get_recently_access() { return recently_access; }
    bool get_is_eof() { return is_eof; }
    bool get_is_half() { return is_half; }
private:
    string file_name;
    int page_number;
    bool recently_access;
    bool is_eof;
    bool is_half;
};


#endif
