#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define IP_COAP_ARDUINO "10.0.0.1"
#define IP_HTTP_SERVER "10.42.0.250"

// CoAP header
struct coap_hdr {
    uint8_t ver_t_tkl;
    uint8_t code;
    uint16_t id;
    char path[10];
};

// CoAP answer header
struct coap_hdr_answer {
    uint8_t ver_t_tkl;
    uint8_t code;
    uint16_t id;
    char path[10];
    char payload[20];
};

char *request_coap(const char *path) {
    // Send a CoAP request to the server at 10.0.0.1
    // CoAP

    char *res = malloc(1024);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5683);
    inet_pton(AF_INET, IP_COAP_ARDUINO, &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return NULL;
    }

    // CoAP request
    struct coap_hdr req = {0};
    req.ver_t_tkl = 0x40;
    req.code = 0x01;
    req.id = htons(0x1234);
    req.path[0] = 0xB3;
    req.path[1] = path[0];
    req.path[2] = path[1];
    req.path[3] = path[2];
    req.path[4] = path[3];
    req.path[5] = path[4];
    req.path[6] = path[5];
    req.path[7] = path[6];
    req.path[8] = path[7];
    req.path[9] = path[8];
    req.path[10] = path[9];

    if (sendto(sock, &req, sizeof(req), 0, (struct sockaddr *)&addr,
               sizeof(addr)) < 0) {
        perror("sendto");
        return NULL;
    }

    // Receive a CoAP response from the server
    struct coap_hdr_answer resp = {0};
    // Malloc the size for the strings

    socklen_t addrlen = sizeof(addr);
    if (recvfrom(sock, &resp, sizeof(resp), 0, (struct sockaddr *)&addr,
                 &addrlen) < 0) {
        perror("recvfrom");
        return NULL;
    }
    // Print the received data caractere by caractere
    int i = 0;
    int j = 0;
    while (resp.payload[i] && i < sizeof(resp.payload)) {
        if (isprint(resp.payload[i])) {
            res[j] = resp.payload[i];
            j++;
        }
        i++;
    }
    res[j] = '\n';
    res = realloc(res, strlen(res) + 1);
    return res;
}

// Function to send HTML data on HTTP
void http_server(void) {
    printf("Waiting for a connection on port 80\n");

    // Wait for a connection on port 80 and send the data
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, IP_HTTP_SERVER, &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return;
    }

    if (listen(sock, 1) < 0) {
        perror("listen");
        return;
    }

    while (1) {
        int client = accept(sock, NULL, NULL);
        if (client < 0) {
            perror("accept");
            return;
        }

        // Get the URL of the request
        char *buf = malloc(1024);
        if (recv(client, buf, 1024, 0) < 0) {
            perror("recv");
            return;
        }

        char *html = malloc(1024);
            printf("Connection accepted, getting data from sensors !\n");
            // Get data from CoAP
            char *data = malloc(1024);
            strcat(data, request_coap("TEST_CAPTs"));
            data = realloc(data, strlen(data) + 1);

            printf("Data received !\n");

            strcat(html, data);

            free(data);

        if (send(client, html, strlen(html), 0) < 0) {
            perror("send");
            return;
        }

        printf("Data sent !\n");

        close(client);

        free(buf);
        free(html);

        printf("Connection closed !\n");
    }

    close(sock);
}

int main(void) {
    http_server();

    return EXIT_SUCCESS;
}