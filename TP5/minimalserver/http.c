#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <limits.h>

typedef struct sockaddr_in SOCKADDR_IN;

#define PORT 3000
#define MAX_REQUEST_SIZE 2048
#define NB_THREADS 10

void *accept_connection(void *sock)
{
    if (listen(*(int *)sock, 1) != 0)
    {
        printf("Error listening");
        return NULL;
    }
    SOCKADDR_IN client_address;
    unsigned int client_address_len = sizeof(client_address);
    int client_fd;
    while (1)
    {
        if ((client_fd = accept(*(int *)sock, (struct sockaddr *)&client_address, &client_address_len)) == -1)
        {
            return NULL;
        }
        char buffer[MAX_REQUEST_SIZE];
        int size;
        if ((size = read(client_fd, buffer, MAX_REQUEST_SIZE)) != -1)
        {
            printf("%.*s\n", size, buffer);
            memset(buffer, 0, size);
        }
        char hostname[_SC_HOST_NAME_MAX + 1];
        gethostname(hostname, _SC_HOST_NAME_MAX + 1);
        printf("\n%s", hostname);

        char html_content[32] = "<p>";
        strcat(html_content, hostname);
        strcat(html_content, "</p>");

        char response[128] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Set-Cookie: ok=value\r\n";

        int content_length = strlen(html_content);
        char buf[5];
        sprintf(buf, "%d", content_length);

        char content_length_str[32] = "Content-Length: ";
        strcat(content_length_str, buf);
        strcat(content_length_str, "\r\n\r\n");

        strcat(response, content_length_str);
        strcat(response, html_content);

        printf("%s\n\n", response);
        write(client_fd, response, strlen(response));
        close(client_fd);
    }
}

int main(int argc, char **argv)
{

    int port;

    if (argc > 1)
    {
        port = atoi(argv[1]);
    }
    else
    {
        port = PORT;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Error creating socket");
        exit(-1);
    }
    SOCKADDR_IN sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        printf("Error binding socket");
        exit(-1);
    }

    pthread_t threads[NB_THREADS];
    int i;
    for (i = 0; i < NB_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, accept_connection, &sock);
        i++;
    }
    for (i = 0; i < NB_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    close(sock);
}