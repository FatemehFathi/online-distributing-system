#include "user.h"


User::User(int fd) {
    sockfd = fd;
    file_name = "";
    page = 1;
    half_page = 1;
}

void User::next_page() {
    page++;
    half_page = 1;
}


void User::next_half_page() {
    half_page = 2;
}


void User::set_file_name(string f) {
    file_name = f;
    page = 1;
    half_page = 1;
}
