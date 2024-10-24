/*
*****************************************************
Knowledge Assessment - Q2 Resolution
Davi Wei Tokikawa
*****************************************************
*/

#include <stdio.h>	
#include <stdlib.h>									
#include <assert.h>

// ESP-IDF API/Library
#include "freertos/task.h"
#include "driver/gpio.h"

TaskHandle_t xButtonStateHandle = NULL;

#define BUTTON_GPIO_NUMBER 5
#define GPIO_HIGH 1
#define GPIO_LOW 0

typedef int gpio_num_t;
typedef enum button_state
{
    OFF,
    ON,
    PROTECTED
}button_state_e;

button_state_e button_state = OFF;

void vTaskButtonState(void *pvParameters)
{
    int protection_count = 0;
    while(1)
    {
        gpio_state = gpio_get_level(BUTTON_GPIO_NUMBER);
        switch(button_state)
        {
            case OFF:
                if(gpio_state == GPIO_HIGH)
                    button_state = ON;
                break;
            case ON:
                if(gpio_state == GPIO_LOW)
                    button_state = PROTECTED;
                break;
            case PROTECTED:
                if(++protection_count >= 100) // 100 * 100ms = 10s
                {
                    protection_count = 0;
                    button_state = OFF;
                }
                break;
            default:
                break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // 100ms delay and context yield
    }
}

// MAIN
int main(void)
{
    // STATE MACHINE TASK CREATION
    xTaskCreate(vTaskButtonState, "ButtonStateManager", 1024, NULL, tskIDLE_PRIORITY, &xButtonStateHandle);

    /************************************/
    /*** APPLICATION SCENARIO EXAMPLE ***/
    /************************************/

    assert(button_state == OFF);                        // Button state starts as OFF
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_HIGH);      // Button is pressed
    assert(button_state == OFF);                        // Button state is still OFF since update occurs at every 100ms
    vTaskDelay(100 / portTICK_PERIOD_MS);               // Delay of 100ms, ensuring minimum update step
    assert(button_state == ON);                         // Button state becomes ON
    vTaskDelay(400 / portTICK_PERIOD_MS);               // Delay of 400ms, totalizing half a second of button pressing action
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_LOW);       // Button is released
    assert(button_state == ON);                         // Button state is still ON since minimum 100ms step has been reached
    vTaskDelay(100 / portTICK_PERIOD_MS);               // Delay of 100ms, ensuring minimum update step
    assert(button_state == PROTECTED);                  // Button state becomes PROTECTED
    vTaskDelay(1000 / portTICK_PERIOD_MS);              // Delay of 1s
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_HIGH);      // Button is pressed
    assert(button_state == PROTECTED);                  // Button state is still PROTECTED since only 1s has passed
    vTaskDelay(1000 / portTICK_PERIOD_MS);              // Delay of 1s, totalizing 1s of button pressing action
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_LOW);       // Button is released
    assert(button_state == PROTECTED);                  // Button state is still PROTECTED since only 2s has passed
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_HIGH);      // Button is pressed
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_LOW);       // Button is released
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_HIGH);      // Button is pressed
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_LOW);       // Button is released
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_HIGH);      // Button is pressed
    gpio_set_level(BUTTON_GPIO_NUMBER, GPIO_LOW);       // Button is released
    assert(button_state == PROTECTED);                  // Button state is still PROTECTED after GPIO ripple
    vTaskDelay(8000 / portTICK_PERIOD_MS);              // Delay of 8s, totalizing 10s in PROTECTED state
    assert(button_state == OFF);                        // Button state returns as OFF

    return 0;
}