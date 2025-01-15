#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = {"stop"};
    char* helloo = "hello from client1";
    char buffer[1024] = { 0 };
    while(1){
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "192.168.10.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(client_fd, helloo, strlen(helloo), 0);
    printf("hello message send\n");  
    valread = read(client_fd, buffer,
                   1024 - 1); // subtract 1 for the null
                              // terminator at the end
    printf("server: %s\n", buffer);
    while(1){

    char help[50];

    printf("do you need help?\n");
    scanf("%49s", help); // Strings do not require '&'
    
    buffer[strcspn(help, "\n")] = '\0';

    // Send the string to the server
    send(client_fd, help, strlen(help), 0);
    printf("Message sent: %s\n", help);
}
    return 0;
}

    return 0;
}


