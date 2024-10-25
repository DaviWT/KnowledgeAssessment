/*
*****************************************************
Knowledge Assessment - Q4 Resolution
Davi Wei Tokikawa
*****************************************************
*/

#include <stdio.h>	
#include <stdlib.h>									
#include <assert.h>
#include <inttypes.h>

// ESP-IDF API/Library
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_intr_alloc.h"

#define MY_UART_ADDRESS 0x02
#define UART_RX_BUFFER_SIZE 512
#define UART_QUEUE_SIZE 10

TaskHandle_t xDataParseHandle = NULL;
QueueHandle_t xUartQueue;
static intr_handle_t handle_console;

static void IRAM_ATTR uart_intr_handle(void *arg) 
{
    // Arbitrary UART RX buffer allocation and filling
    uart_rx_buffer = fill_uart_rx_buffer();

    // Sends uart_rx_buffer to an FIFO instance
    xQueueSend(xUartQueue, (uint8_t *) uart_rx_buffer, (TickType_t) 0);

    // Clears interrupt flags
    uart_clear_intr_status(UART_NUM_0, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
}

void vTaskUartDataParser(void *pvParameters)
{
    /*
    Payload format considered for this parsing, according to PDF statement:

    0__________________2______________6___________________7______________8__________________9___________________11__________________________________N
    | [TOTAL_SIZE : 2] | [CRC_32 : 4] | [DESTINATION : 1] | [SOURCE : 1] | [COMMAND_ID : 1] | [PAYLOAD_SIZE : 2] | [COMMAND_PAYLOAD : PAYLOAD_SIZE] |
    |__________________|______________|___________________|______________|__________________|____________________|__________________________________|
    
    Where: "[FIELD NAME : AMOUNT OF BYTES]"
    */

    uint8_t *uart_rx_buffer;
    while(1)
    {
        // Yields while waiting for new FIFO element
        if(xQueueReceive(xUartQueue, uart_rx_buffer, 10 / portTICK_PERIOD_MS) == pdTRUE) 
        {
            uint16_t total_size =   uart_rx_buffer[0] |
                                    uart_rx_buffer[1] << 8;

            uint32_t obtained_crc32 =   uart_rx_buffer[2] |
                                        uart_rx_buffer[3] << 8 |
                                        uart_rx_buffer[4] << 16 |
                                        uart_rx_buffer[5] << 24;

            // Arbitrary function that calculates the CRC-32 value for a byte array
            calculated_crc32 = calculate_crc32(uart_rx_buffer, total_size);
            if(calculated_crc32 == obtained_crc32)
            {
                uint8_t dst_addr = uart_rx_buffer[6];
                if(dst_addr == MY_UART_ADDRESS)
                {
                    uint8_t src_addr = uart_rx_buffer[7];
                    uint8_t cmd_id = uart_rx_buffer[8];
                    uint16_t payload_size = uart_rx_buffer[9] |
                                            uart_rx_buffer[10] << 8;
                    int8_t *cmd_payload = (int8_t*)malloc(payload_size * sizeof(int8_t)) 
                    memcpy(cmd_payload, uart_rx_buffer + 11, payload_size * sizeof(int8_t));

                    // DO SOMETHING WITH PARSED INFORMATION AND PAYLOAD

                    free(cmd_payload);
                }
            }
            
            free(uart_rx_buffer);
        }
    }
}

// MAIN
int main(void)
{
    /* 
    "Imagine that you have a microcontroller that communicates to a generic system that may consist of several other 
    boards via UART. How do you ensure that each message is properly sent and that its content is correct?"

    --> If assuming that there are more than 2 elements requiring UART communication, TTL to RS-485 transceivers must 
    be used to allow, at least, half-duplex transmission mode. Arbitrary rules for bus allocation must be applied to 
    ensure that every message from each element is sent. Regarding content integrity, data redundancy and ACK/NACK 
    techniques can be applied to allow error detection and minimize packet loss.
    */

    // UART QUEUE CREATION
    xUartQueue = xQueueCreate(UART_QUEUE_SIZE, sizeof(uint8_t *));

    // UART DATA PARSER TASK CREATION
    xTaskCreate(vTaskUartDataParser, "UartDataParser", 1024, NULL, tskIDLE_PRIORITY, &xDataParseHandle);

    // SIMPLIFIED ESP32 UART INTERRUPT SETUP
    uart_isr_register(UART_NUM_0, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console);
	uart_enable_rx_intr(UART_NUM_0);

    return 0;
}