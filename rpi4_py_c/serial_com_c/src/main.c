// Author: Michal Balogh, xbalog06
// Date: 14-10-2024

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "display_letter.h"
#include "display.h"
#include "serial.h"

#include "examples.h"

#define BUFFER_SIZE 128

#define IP_ADRESS ""
#define PORT 80

// Connect to a socket and receive gestures
int connect_to_socket(int serial_port, unsigned char* message, char* ip_address, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Set up the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip_address);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        close(sock);
        return 1;
    }

    int i = 0;
    char gesture[BUFFER_SIZE];
    // Receive gestures from the server
    while (1) {
        memset(gesture, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, gesture, sizeof(gesture) - 1, 0);
        if (bytes_received > 0) {
            gesture[bytes_received] = '\0';
            printf("Received gesture: %s\n", gesture);

            // Set the gesture on the 7-segment display
            set_letter_at_position(message, get_segments_from_letter(gesture[0]), i, 0);
            i++;

            // Send the message to the 7-segment display
            int bytes_sent = serial_send(serial_port, message, TOTAL_MESSAGE_SIZE);
            if (bytes_sent != TOTAL_MESSAGE_SIZE) {
                fprintf(stderr, "Error: Not all bytes were sent.\n");
            }
        }
        else {
            break; // Connection closed
        }
        sleep(1);
    }

    close(sock);
    return 0;
}

int main() {
    unsigned char address = 0xFF;
    unsigned char command = 0x83;

    int serial_port = serial_open("/dev/ttyUSB0");
    if (serial_port < 0) {
        fprintf(stderr, "Error opening serial port.\n");
        return 1;
    }

    display_clear(address, command, serial_port);
    sleep(1);

    unsigned char* message = init_message(address, command);
    if (message == NULL) {
        fprintf(stderr, "Error allocating memory for message.\n");
        serial_close(serial_port);
        return 1;
    }

    connect_to_socket(serial_port, message, IP_ADRESS, PORT);

    serial_close(serial_port);
    free(message);

    return 0;
}
