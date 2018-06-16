#include "frame.h"



Frame::Frame() {
    file_name = "/";
    page_number = 1;
    recently_access = 0;
    is_eof = 0;
    is_half = 0;
}


Frame::Frame(string n) {
    file_name = n;
    page_number = 1;
    recently_access = 1;
    is_eof = 0;
    is_half = 0;
}