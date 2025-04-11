#include <stdio.h>
#include <SDL3/SDL.h>  
#include <prepareUDP_Serial.h>

uint16_t value_cali ;

int main(int argc, char *argv[]) {
    // Initialize SDL with video and joystick support.
    // SDL_Init returns 0 on success.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Get the array of joystick IDs and the count of connected joysticks.
    int joystickCount = 0;
    SDL_JoystickID *joystickIDs = SDL_GetJoysticks(&joystickCount);
    if (joystickIDs == NULL || joystickCount == 0) {
        printf("No joysticks connected.\n");
    } else {
        printf("Found %d joystick(s):\n", joystickCount);

        // Iterate through all joystick IDs.
        for (int i = 0; i < joystickCount; i++) {
            SDL_JoystickID jid = joystickIDs[i];
            printf("Joystick at index %d has ID: %d\n", i, jid);

            // Open the joystick using its ID.
            SDL_Joystick *joystick = SDL_OpenJoystick(jid);
            if (joystick == NULL) {
                fprintf(stderr, "Failed to open joystick with ID %d: %s\n", jid, SDL_GetError());
            } else {
                printf("Opened joystick: %s\n", SDL_GetJoystickName(joystick));
                
               
                while (true) {  
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) { 
                        // Check for joystick axis motion events.
                        if (event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
                            // Access only after confirming the event type.
                            uint8_t axis = event.jaxis.axis;
                            int16_t value = event.jaxis.value;
                            if (axis == 1) {
                                value_cali = ((value - 32768) * -1) - 1;
                                printf("Speed moved to %d\n", value_cali);
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
