#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>


// process www.google.com to 3www6google3com
void process_lexem(unsigned char* source, unsigned char* answer) {
    unsigned int i = 0;
    unsigned char* curr_pos = source;
    char* next_pos;
    do{
        if (NULL != (next_pos = strchr(curr_pos, '.'))) { // position of first point
            next_pos[0] = '\0';  // to not count point
        }
        answer[i] = (unsigned char)(strlen(curr_pos));  // write length
        strcpy(&answer[++i], curr_pos);         // package answer
        i += (unsigned int)(strlen(curr_pos));
        curr_pos = (unsigned char*)&next_pos[1];        // update current position
    }
    while (NULL != next_pos);
}

int main(int argc, char** argv)
{
    unsigned char endings[5] = {0x00, 0x00, 0x01, 0x00, 0x01 };
    unsigned char input_str[512];

    //initialize DNS server
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("8.8.8.8");
    address.sin_port = htons(53);  // DNS port

    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_fd == -1){
        exit(1);
    }

    // reading
    while (scanf("%s", input_str) != EOF) {
        unsigned int len = strlen(input_str);
        // send query to server
        unsigned char dns_query[512] = {(unsigned char)(getpid()), (unsigned char)(getpid()), 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        process_lexem((unsigned char *) &input_str, (unsigned char *) &dns_query[12]);     // package our query
        memcpy((unsigned char*)&dns_query[len + 13], (unsigned char*)&endings, 5);      // still package query
        if ( -1 == sendto(socket_fd, &dns_query, len + 18, 0, (struct sockaddr*) &address, sizeof(address))){
            close(socket_fd);
            exit(1);
        }
        // query was sent
        // receive answer from server
        recvfrom(socket_fd, &dns_query, sizeof(dns_query), 0, (struct sockaddr*)&address, (unsigned int*)(sizeof(address)));
        // if we received two or more answers -> we must pass them
        unsigned char* answer = &dns_query[len + 18];
        unsigned int i = 0;
        unsigned int position = 2;
        // find position of ip address
        while (answer[position] != 0 || answer[position + 1] != 1){
            ++i;
            position = 14*i + 2;
        }

        struct sockaddr_in get_ip;
        get_ip.sin_addr.s_addr = (*(long*)(answer + 14 * i + 12));
        printf("%s\n", inet_ntoa(get_ip.sin_addr));
    }
    close(socket_fd);
    return 0;
}