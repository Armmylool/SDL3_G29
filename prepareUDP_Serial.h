#ifndef UDP_SERIAL_H
#define UDP_SERIAL_H

#include <stdint.h>
#include <windows.h>

// ฟังก์ชันสำหรับการตั้งค่า Serial
HANDLE Serial_Begin(uint32_t baudrate, uint8_t byte_size, uint8_t parity, uint8_t stop_bits);

// ฟังก์ชันการส่งค่าผ่าน Serial
void Serial_Write(HANDLE hSerial, uint8_t *data, size_t length);

// ฟังก์ชันการอ่านค่าจาก Serial
void Serial_Read(HANDLE hSerial, uint8_t *buffer, size_t buffer_size, DWORD *bytes_read);

#endif // UDP_SERIAL_H
