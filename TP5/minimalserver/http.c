#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_REQUEST_SIZE 2048
#define PORT 8080

int main()
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error creating socket\n");
        return -1;
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        printf("Error setting socket options\n");
        close(sock);
        return -1;
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        printf("Error binding socket\n");
        close(sock);
        return -1;
    }

    if (listen(sock, 1) != 0)
    {
        printf("Error listening\n");
        close(sock);
        return -1;
    }

    struct sockaddr_in client_address;
    unsigned int client_address_len = sizeof(client_address);
    int client_fd;
    while (1)
    {
        if ((client_fd = accept(sock, (struct sockaddr *)&client_address, &client_address_len)) == -1)
        {
            printf("Error accepting connection\n");
            continue;
        }

        char buffer[MAX_REQUEST_SIZE];
        int size;
        if ((size = read(client_fd, buffer, MAX_REQUEST_SIZE)) != -1)
        {
            printf("%.*s", size, buffer);
            memset(buffer, 0, size);
        }

        char hostname[_SC_HOST_NAME_MAX + 1];
        gethostname(hostname, _SC_HOST_NAME_MAX + 1);

        char html_content[64] = "<p>";
        strcat(html_content, hostname);
        strcat(html_content, "</p>");

        char response[128] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n";

        int content_length = strlen(html_content);
        char buf[32];
        sprintf(buf, "%d", content_length);

        char content_length_str[32] = "Content-Length: ";
        strcat(content_length_str, buf);
        strcat(content_length_str, "\r\n\r\n");

        strcat(response, content_length_str);
        strcat(response, html_content);

        printf("%s\n------------\n", response);
        write(client_fd, response, strlen(response));
        close(client_fd);
    }
    close(sock);
    return 0;
}
