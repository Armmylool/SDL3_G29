#include <stdio.h>
#include <stdint.h>

#define BUFLEN 8192 

uint16_t checksum ;
uint8_t Move_Pre_command[8] = {0x08, 0x01, 0x13} ;
unsigned char low_byte ;
unsigned char high_byte ;
unsigned char byteArray[2] ;

uint8_t calculate_checksum(uint8_t *data, size_t length) {
    checksum = 0;
    for (size_t i = 0; i < length - 1; i++) {
        checksum += data[i];
    }
    return (uint8_t)(checksum & 0xFF);
}

int main() {
    unsigned int data = 32767; 
    unsigned char byteArray[2];

    unsigned char lowerByte = data & 0xFF;          
    unsigned char higherByte = (data >> 8) & 0xFF;    

    byteArray[0] = higherByte;  
    byteArray[1] = lowerByte;   

    Move_Pre_command[3] = lowerByte ;
    Move_Pre_command[4] = higherByte ;
    Move_Pre_command[7] = calculate_checksum(Move_Pre_command,sizeof(Move_Pre_command)) ;

    for (int i = 0; i < sizeof(Move_Pre_command); i++) {
        printf("%02x\n", Move_Pre_command[i]);
    }
    return 0;
}