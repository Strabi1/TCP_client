#ifdef WIN32
    #include <windows.h>
    #include <winsock.h>
#else
    #define closesocket close
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

/*!------------------------------------------------------------------------
 * Program:   client
 *
 * Purpose:   allocate a socket, connect to a server, and print all output
 *
 * Syntax:    client <host> <port>
 *
 *               host  - name of a computer on which server is executing
 *               port  - protocol port number server is using
 *------------------------------------------------------------------------
 */

void usage(char *prg) {
    printf("usage:%s <server> <port>",prg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
        struct  hostent  *ptrh;  /* pointer to a host table entry       */
        struct  protoent *ptrp;  /* pointer to a protocol table entry   */
        struct  sockaddr_in sad; /* structure to hold an IP address     */
        int     sd;              /* socket descriptor                   */
        int     port;            /* protocol port number                */
        char    *host;           /* pointer to host name                */
        int     n;               /* number of characters read           */
        char    buf[BUFFER_SIZE];/* buffer for data from the server     */
        char    *text;           /* pointer to user's line of text      */
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup(0x0101, &wsaData);
#endif
        if (argc == 3)              /* if protocol port specified   */
            port = atoi(argv[2]);   /* convert to binary            */
        else
            usage(argv[0]);

        memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
        sad.sin_family = AF_INET;         /* set family to Internet     */
        /* Check command-line argument for protocol port and extract    */
        /* port number if one is specified.                             */
        if (port > PUBLIC_PORT)         /* test for legal value         */
            sad.sin_port = htons((u_short)port); /* Internet is big endian */
        else {                          /* print error message and exit */
            printf("Bad port number %s\n",argv[2]);
            exit(EXIT_FAILURE);
        }
        /* Check host argument and assign host name. */
        host = argv[1];         /* if host argument specified   */
        /* Convert host name to equivalent IP address and copy to sad. */
        ptrh = gethostbyname(host);
        if ( !ptrh ) {
            printf("Invalid host: %s\n", host);
            exit(EXIT_FAILURE);
        }
        memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
        /* Map TCP transport protocol name to protocol number. */
        if ( !(ptrp = getprotobyname("tcp")) ) {
            printf("Cannot map \"tcp\" to protocol number");
            exit(EXIT_FAILURE);
        }
        /* Create a socket. */
        sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
        if (sd < 0) {
            printf("Socket creation failed\n");
            exit(EXIT_FAILURE);
        }
        /* Connect the socket to the specified server. */
        if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
            printf("Connect failed\n");
            exit(EXIT_FAILURE);
        }

        printf("Üzenet:");
        text = fgets(buf, sizeof(buf), stdin);

        if (text && *text!='.') {
            send(sd, buf, strlen(buf), 0);
            n = recv(sd, buf, sizeof(buf), 0);
            fputs(buf, stdout);
        }

        /* Close the socket. */
        closesocket(sd);
        /* Terminate the client program gracefully. */
#ifdef WIN32
        WSACleanup();
#endif
        exit(EXIT_SUCCESS);
}

