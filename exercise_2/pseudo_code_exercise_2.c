#include <wiringPi.h>


#define NUM_CONNECTORS 3
#define PINS_PER_CONNECTOR 6
#define LED_PIN 8


int expectedPinning[PINS_PER_CONNECTOR] = {1, 2, 3, 4, 5, 6};


void setupGPIO() {
    wiringPiSetup();  // Initialize wiringPi library
    pinMode(LED_PIN, OUTPUT);  // Set the LED pin as an output


    // Set the connector pins as inputs
    int connectorPins[NUM_CONNECTORS][PINS_PER_CONNECTOR] = {
        {2, 3, 4, 5, 6, 7},  // GPIO pins connected to the cable harness connectors
        // Add the other connectors' GPIO pins here
    };
    for (int connector = 0; connector < NUM_CONNECTORS; connector++) {
        for (int pin = 0; pin < PINS_PER_CONNECTOR; pin++) {
            pinMode(connectorPins[connector][pin], INPUT);
        }
    }
}


int checkPinning() {
    int detectedPinning[PINS_PER_CONNECTOR];
    int connectorPins[NUM_CONNECTORS][PINS_PER_CONNECTOR] = {
        {2, 3, 4, 5, 6, 7},  // GPIO pins connected to the cable harness connectors
        // Add the other connectors' GPIO pins here
    };


    for (int pin = 0; pin < PINS_PER_CONNECTOR; pin++) {
        detectedPinning[pin] = digitalRead(connectorPins[0][pin]);
    }


    // Check for pin conflicts
    for (int connector = 1; connector < NUM_CONNECTORS; connector++) {
        for (int pin = 0; pin < PINS_PER_CONNECTOR; pin++) {
            if (detectedPinning[pin] != digitalRead(connectorPins[connector][pin])) {
                return 0;  // Incorrect pinning detected
            }
        }
    }


    // Sort the detected pin values
    for (int i = 0; i < PINS_PER_CONNECTOR - 1; i++) {
        for (int j = 0; j < PINS_PER_CONNECTOR - i - 1; j++) {
            if (detectedPinning[j] > detectedPinning[j + 1]) {
                int temp = detectedPinning[j];
                detectedPinning[j] = detectedPinning[j + 1];
                detectedPinning[j + 1] = temp;
            }
        }
    }


    // Compare with the correct pinning sequence
    for (int pin = 0; pin < PINS_PER_CONNECTOR; pin++) {
        if (detectedPinning[pin] != expectedPinning[pin]) {
            return 0;  // Incorrect pinning detected
        }
    }


    return 1;  // Correct pinning detected
}


int main() {
    setupGPIO();


    while (1) {
        if (checkPinning()) {
            digitalWrite(LED_PIN, HIGH);  // Turn on the LED
        } else {
            digitalWrite(LED_PIN, LOW);   // Turn off the LED
        }
    }


    return 0;
}