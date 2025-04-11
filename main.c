#include <stdio.h>
#include <SDL3/SDL.h>  
#include <prepareUDP_Serial.h>

#define BUFLEN 8192
HANDLE hSerial ;
uint16_t checksum;
uint16_t lastvalue;
uint8_t Move_Pre_command[8] = {0x08, 0x01, 0x13, 0x00, 0x00};
unsigned char low_byte;
unsigned char high_byte;
unsigned char byteArray[2];

uint8_t calculate_checksum(uint8_t *data, size_t length) {
    checksum = 0;
    for (size_t i = 0; i < length - 1; i++) {
        checksum += data[i];
    }
    return (uint8_t)(checksum & 0xFF);
}

void process_data(int raw_data) {
    unsigned char lowerByte = raw_data & 0xFF ;          
    unsigned char higherByte = (raw_data >> 8) & 0xFF ;    

    Move_Pre_command[3] = lowerByte ;
    Move_Pre_command[4] = higherByte ;
    Move_Pre_command[7] = calculate_checksum(Move_Pre_command,sizeof(Move_Pre_command)) ;
    Serial_Write(hSerial, Move_Pre_command, sizeof(Move_Pre_command)) ;

}

int main() {
    hSerial = Serial_Begin(115200, 8, EVENPARITY, ONESTOPBIT);

    if ((hSerial == INVALID_HANDLE_VALUE)) {
        printf("Failed to open serial port.\n");
        CloseHandle(hSerial);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    int joystickCount = 0;
    SDL_JoystickID *joystickIDs = SDL_GetJoysticks(&joystickCount);
    if (joystickIDs == NULL || joystickCount == 0) {
        printf("No joysticks connected.\n");
    } else {
        printf("Found %d joystick(s):\n", joystickCount);

        for (int i = 0; i < joystickCount; i++) {
            SDL_JoystickID jid = joystickIDs[i];
            printf("Joystick at index %d has ID: %d\n", i, jid);

            SDL_Joystick *joystick = SDL_OpenJoystick(jid);
            if (joystick == NULL) {
                fprintf(stderr, "Failed to open joystick with ID %d: %s\n", jid, SDL_GetError());
            } else {
                printf("Opened joystick: %s\n", SDL_GetJoystickName(joystick));
                while (true) {  
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) { 
                        if (event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
                            uint8_t axis = event.jaxis.axis;
                            int16_t value = event.jaxis.value;
                            if (axis == 1) {
                                uint16_t value_cali = ((value - 32768) * -1) - 1;
                                printf("Speed moved to %d\n", value_cali);
                                process_data(value_cali) ;
                            }
                            else if (axis == 2) {
                                uint16_t value_cali_brake = ((value - 32768) * -1) - 1;
                                printf("Brake moved to %d\n", value_cali_brake);
                            }
                        }
                    } 
                }

                // Close the joystick when done.
                SDL_CloseJoystick(joystick);
            }
        }
    }

    // Clean up SDL subsystems.
    SDL_Quit();
    return 0;
}
