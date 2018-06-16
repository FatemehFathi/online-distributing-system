#include "server.h"



void Server::get_files_name() {
    DIR *dir;
    struct dirent *dp;
    
    dir = opendir("Disc");
    
    while(dp = readdir(dir)) {
        files_name.push_back(dp->d_name);
    }
    
    (void)closedir(dir);
    
    
    /*
     cout << "Files Name:" << endl;
     for (int i = 0; i < files_name.size(); i++) {
     if(i == 0)
     files_name.erase(files_name.begin() + i);
     else
     cout << files_name[i] <<endl;
     }
     */
}





void Server::run(char* p) {
    get_files_name();
    
    int port = atoi(p);
    char message[MESSAGE_SIZE];
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    struct dirent **namelist;
    struct timeval timeout;
    int master_socket;
    fd_set fds;
    fd_set temp_fds;
    int opt = 1;
    int max_fd, addrlen, newfd, i;
    
    FD_ZERO(&fds);
    FD_ZERO(&temp_fds);
    
    
    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        write(1, "Socket Failed!", 14);
        exit(1);
    }
    
    
    //set master socket to allow multiple connections. this is just a good habit. it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0 )
    {
        write(1, "Setsockopt Error!", 17);
        exit(EXIT_FAILURE);
    }
    
    
    //create type of socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    memset(&(serv_addr.sin_zero), '\0', 8);
    
    
    //binding:
    if (bind(master_socket, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) == -1)
    {
        write(1, "Bind Failed!", 12);
        exit(1);
    }
    
    
    //listening:
    if (listen(master_socket, 20) == -1)
    {
        write(1, "Listen Failed!", 14);
        exit(1);
    }
    
    
    
    FD_SET(master_socket, &fds);
    max_fd = master_socket;
    
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    
    //////////////////////////////////////////////////////////
    
    while(TRUE) {
        temp_fds = fds;
        
        if ((select(max_fd + 1, &temp_fds, NULL, NULL, &timeout)) == -1) {
            write(1, "Select Error!", 13);
            exit(1);
        }
        
        
        
        if (rr_queue.size() > 0) {
            vector<string> rr_res = round_robin();
            int sfd = atoi(rr_res[0].c_str());
            
            write(sfd, rr_res[1].c_str(), rr_res[1].length());
            
            if(rr_res.size() == 3) {
                write(sfd, "File Sent Successfully", 22);
            }
        }
        
        
        //connectionaro nega mikone bebine kudumeshun mikhad data befreste
        for (i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &temp_fds)) { //yekio peyda kard
                
                if (i == master_socket) { //age haminie ke alan vared shode, acc kon va be liste clienta ezafash kon
                    addrlen = sizeof(cli_addr);
                    if ((newfd = accept(master_socket, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen)) == -1) {
                        write(1, "Accept Failed!", 14);
                        
                    } else {
                        FD_SET(newfd, &fds);
                        if (newfd > max_fd) {
                            max_fd = newfd;
                        }
                        
                        cout << "New Connection!" << endl;
                        
                        User* new_user = new User(newfd);
                        users.push_back(new_user);
                    }
                    
                } else { //pas ye cliente ghadimi dare payam mide!
                    memset(message, '\0', MESSAGE_SIZE);
                    
                    if (recv(i, message, sizeof(message), 0) <= 0) { //age dc shode bud yedafeii
                        write(1, "Client Disconnected!\n", 22);
                        for(int k = 0; k < users.size(); k++) {
                            if(users[k]->get_sockfd() == i) {
                                users.erase(users.begin() + k);
                                ////close
                            }
                        }
                        
                    } else {
                        
                        for(int k = 0; k < users.size(); k++) {
                            if(users[k]->get_sockfd() == i) {
                                if(strcmp(message, "disconnect") == 0) {
                                    //////close!
                                    write(1, "Client Disconnected!\n", 22);
                                    users.erase(users.begin() + k);
                                    
                                } else {
                                    bool valid_name = false;
                                    for(int m = 0; m < files_name.size(); m++) {
                                        if(files_name[m] == message) {
                                            valid_name = true;}
                                    }
                                    
                                    if(valid_name) {
                                        users[k]->set_file_name(message); //esme filesho behesh ezafe kon
                                        rr_queue.push_back(users[k]); //ezafash kon be safe rr
                                    } else {
                                        write(i, "Invalid File Name!\n", 20);
                                    }
                                }
                            }
                        } //for
                    }
                }
                FD_CLR(i, &temp_fds);
            }
        } //for
    } //while
}







vector<string> Server::round_robin() {
    vector<string> result;
    string send_it = "Loading.. Please Wait!\n";
    
    //page tuye main_memory has ya na
    int frame_index;
    bool is_exist = false;
    for (int i = 0; i < 256; i++) {
        if(page_table[i].get_file_name() == rr_queue[0]->get_file_name()) {
            if(page_table[i].get_page_number() == rr_queue[0]->get_page()) {
                is_exist = true;
                frame_index = i;
            }
        }
    }
    
    int sockfd = rr_queue[0]->get_sockfd();
    stringstream ss;
    ss << sockfd;
    string str = ss.str();
    result.push_back(str);
    
    ///////////////////////////////////////////////
    
    if (is_exist) { //age tuye main memory bud
        cout << "Search in the Main Memory" << endl;
        send_it = "***File Name: ";
        send_it += rr_queue[0]->get_file_name();
        send_it += ", Page: ";
        stringstream sss;
        sss << rr_queue[0]->get_page();
        string curr_page = sss.str();
        send_it += curr_page;
        send_it += "***\n";
        
        if(rr_queue[0]->get_half_page() == 1) {
            for(int i = 0; i < 128; i++)
                send_it += main_memory[frame_index][i];
            rr_queue[0]->next_half_page();
            
        } else if(rr_queue[0]->get_half_page() == 2) {
            for(int i = 128; i < 256; i++)
                send_it += main_memory[frame_index][i];
            rr_queue[0]->next_page();
        }
        
        result.push_back(send_it);
        page_table[frame_index].set_recently_access(1);
        
        
        //age filesh kamel shode bud az rr hazf she + bege kamel shode:
        if(page_table[frame_index].get_is_eof()) {
            if((page_table[frame_index].get_is_half() == 1 && rr_queue[0]->get_half_page() == 2) ||
               (page_table[frame_index].get_is_half() == 0 && rr_queue[0]->get_half_page() == 1)) {
                rr_queue.erase(rr_queue.begin());
                result.push_back("&END&");
            }
        } else {
            //usero bezar tahe safe rr
            rr_queue.push_back(rr_queue[0]);
            rr_queue.erase(rr_queue.begin());
        }
        
        ///////////////////////////////////////////////
        
    } else { //age tuye disc bud
        cout << "Search in the Disc" << endl;
        int empty_frame;
        bool is_empty = false;
        for (int i = 0; i < 256; i++) {
            if(page_table[i].get_file_name() == "/") {
                is_empty = true;
                empty_frame = i;
            }
        }
        
        if(!is_empty) {
            cout << "There is no Empty Frame! Page Replacement" << endl;
            empty_frame = page_replacement();
        }
        
        disc_to_memory(empty_frame, rr_queue[0]->get_file_name(), rr_queue[0]->get_page());
        page_table[empty_frame].set_recently_access(1);
        
        //usero bezar tahe safe rr
        rr_queue.push_back(rr_queue[0]);
        rr_queue.erase(rr_queue.begin());
        
        result.push_back(send_it);
    }
    
    
    return result;
}



int Server::page_replacement() {
    int empty_frame = 0;
    
    empty_frame = second_chance();
    //empty_frame = optimal();
    
    
    return empty_frame;
}




int Server::second_chance() {
    for (int i = 0; i < 256; i++) {
        if (page_table[i].get_recently_access() == 1)
            page_table[i].set_recently_access(0);
        else
            return i;
    }
    return 0;
}




int Server::optimal() {
    int farest_i = 0;
    int farest_j = 0;
    
    for (int i = 0; i < 256; i++) {
        for(int j = 0; j < rr_queue.size(); j++) {
            if(rr_queue[i]->get_file_name() == page_table[i].get_file_name() && rr_queue[i]->get_page() == page_table[i].get_page_number())
                if(farest_j < j) {
                    farest_j = j;
                    farest_i = i;
                }
        }
    }
    
    return farest_i;
}




void Server::disc_to_memory(int empty_frame, string file_name, int page_numb) {
    chdir("Disc");
    ifstream f(file_name.c_str());
    
    int position = 0;
    f.seekg((page_numb-1) * 256, ios::beg);
    
    bool is_eof = false;
    
     memset(main_memory[empty_frame], 0, sizeof main_memory[empty_frame]);
    
    while(!(is_eof = f.eof()) && position < 258) {
            f.get(main_memory[empty_frame][position]);
        position++;
    }
    
    
    if (position < 256)
        main_memory[empty_frame][position - 1] = '\0';
    else
        main_memory[empty_frame][256] = '\0';
    
    //update page table:
    Frame* fr = new Frame(file_name);
    if(is_eof == true)
        fr->set_is_eof(1);
    if(position <= 129)
        fr->set_is_half(1);
    fr->set_page_number(page_numb);
    page_table[empty_frame] = *fr;
    
    /*
     for(int i = 0; main_memory[empty_frame][i] != '\0'; i++) {
     cout << main_memory[empty_frame][i];
     }
     */
}





int main(int argc, char* argv[]) {
    Server* s = new Server();
    s->run(argv[1]);
    return 0;
}



