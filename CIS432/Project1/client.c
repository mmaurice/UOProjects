#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "duckchat.h"
 
//
#define BUFLEN 512  //Max length of buffer
//The port on which to send data


void die(const char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char *argv[])
{
    argc = argc;    
    struct sockaddr_in si_other;
    struct hostent *server;
    char **pointer;
    char ipaddress[INET_ADDRSTRLEN];

    int s, portno;
    socklen_t slen;
    slen = sizeof(si_other);
    char buf[BUFLEN];
    char str[BUFLEN];
    portno = atoi(argv[2]);
    //const char *username = (argv[3]);
    //printf("username/port: %s:%d\n", username, portno);

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    server = gethostbyname(argv[1]);
    if (server == NULL) {
	   die("server");
       return 0;
    }

    printf("host's official name: %s\n",server->h_name);

    for(pointer=server->h_addr_list;*pointer;pointer++)
    {
        inet_ntop(AF_INET,(void *)*pointer,ipaddress,sizeof(ipaddress));
        printf("IP address: %s\n",ipaddress);
    }


    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(portno);
     
    if (inet_aton(ipaddress , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    while(1)
    {
        printf("Enter message: ");
        char *result = fgets(str, sizeof(str), stdin);
        char len = strlen(str);
        
        if(result != NULL && str[len - 1] == '\n')
        {
          str[len - 1] = '\0';
        }
        else
        {
          // handle error
        }

         
        //send the str
        if (sendto(s, str, strlen(str) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
         
        puts(buf);
    }
 
    close(s);
    return 0;
}
