#include "prepareUDP_Serial.h"
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>

#pragma comment(lib, "ws2_32.lib")


SOCKET UDP_Setting(const char *server_ip, uint16_t port) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = (server_ip) ? inet_addr(server_ip) : INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    int buffer_size = 8192;  // ขนาด buffer ที่ต้องการ
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&buffer_size, sizeof(buffer_size)) == SOCKET_ERROR) {
        printf("Failed to set receive buffer size. Error Code: %d\n", WSAGetLastError());
    }   

    printf("UDP server setup complete. Listening on port %d...\n", port);
    return sock;
}

HANDLE Serial_Begin(uint32_t baudrate, uint8_t byte_size, uint8_t parity, uint8_t stop_bits) {
    HANDLE hSerial = CreateFile("COM5", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening COM port. Error Code: %d\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting COM state. Error Code: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = baudrate;
    dcbSerialParams.ByteSize = byte_size;
    dcbSerialParams.Parity = parity;
    dcbSerialParams.StopBits = stop_bits;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting COM state. Error Code: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = MAXDWORD; 
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error setting timeouts. Error Code: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    if (!FlushFileBuffers(hSerial)) {
            printf("Error flushing serial port buffer. Error Code: %d\n", GetLastError());
    } else {
            printf("Flushed serial port buffer successfully.\n");
    }

    printf("Serial port initialized with Baudrate: %d\n", baudrate);
    return hSerial;
}


void Serial_Write(HANDLE hSerial, uint8_t *data, size_t length) {
    DWORD bytes_written;
    if (!WriteFile(hSerial, data, length, &bytes_written, NULL)) {
        printf("Error writing to serial port. Error Code: %d\n", GetLastError());
    } else {
        printf("Sent %ld bytes to serial port.\n", bytes_written);
    }
}

// การอ่านค่าจาก Serial
void Serial_Read(HANDLE hSerial, uint8_t *buffer, size_t buffer_size, DWORD *bytes_read) {
    if (!ReadFile(hSerial, buffer, buffer_size, bytes_read, NULL)) {
        printf("Error reading from serial port. Error Code: %d\n", GetLastError());
    } else if (*bytes_read > 0) {
        printf("Read %ld bytes from serial port.\nRead :", *bytes_read);
        for (DWORD i = 0; i < *bytes_read; i++) {
            printf("%02X ", buffer[i]);
        }
        printf("\n");
    } else {
        printf("No data received.\n");
    }
}

