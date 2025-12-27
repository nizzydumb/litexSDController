#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>

#include <libbase/uart.h>
#include <libbase/console.h>
#include <generated/csr.h>
#include <generated/mem.h>


#define SDIO_BASE        0x80000000

#define SDIO_MAIN_CLOCK_FREQUENCY 		(*(volatile uint32_t*)(SDIO_BASE + 0x0000))
#define SDIO_SDIO_CLOCK_FREQUENCY 		(*(volatile uint32_t*)(SDIO_BASE + 0x1000))
#define SDIO_CMD_INDEX 					(*(volatile uint32_t*)(SDIO_BASE + 0x2000))
#define SDIO_CMD_ARGUMENT			 	(*(volatile uint32_t*)(SDIO_BASE + 0x3000))
#define SDIO_DATA_BUFFER 				( (volatile uint32_t*)(SDIO_BASE + 0x4000))


#define SDIO_SEND_CMD_OP 				(*(volatile uint32_t*)(SDIO_BASE + 0x5000))
#define SDIO_SEND_CMD_AND_READ_DATA_OP 	(*(volatile uint32_t*)(SDIO_BASE + 0x6000))
#define SDIO_SEND_CMD_AND_SEND_DATA_OP	(*(volatile uint32_t*)(SDIO_BASE + 0x7000))
#define SDIO_READ_DATA_OP		 		(*(volatile uint32_t*)(SDIO_BASE + 0x8000))
#define SDIO_SEND_DATA_OP 				(*(volatile uint32_t*)(SDIO_BASE + 0x9000))

#define SDIO_CMD_BUSY	 				(*(volatile uint32_t*)(SDIO_BASE + 0xa000))
#define SDIO_DATA_BUSY	 				(*(volatile uint32_t*)(SDIO_BASE + 0xb000))
#define SDIO_CMD_STATUS	 				(*(volatile uint32_t*)(SDIO_BASE + 0xc000))
#define SDIO_DATA_STATUS	 			(*(volatile uint32_t*)(SDIO_BASE + 0xd000))
#define SDIO_DATA_LENGTH				(*(volatile uint32_t*)(SDIO_BASE + 0xe000))

uint32_t sdio_read_main_clock_frequency()
{
	uint32_t main_clock_frequency = SDIO_MAIN_CLOCK_FREQUENCY;
	return main_clock_frequency;
}

uint32_t sdio_read_sd_clock_frequency()
{
	uint32_t sd_clock_frequency = SDIO_SDIO_CLOCK_FREQUENCY;
	return sd_clock_frequency;
}

void sdio_set_cmd_request_index(uint8_t cmd_index) 
{
	SDIO_CMD_INDEX = 0x00000000 | cmd_index;
}

uint8_t sdio_read_cmd_response_index()
{
	uint32_t cmd_index = SDIO_CMD_INDEX;
	return cmd_index & 0xFF;
}

void sdio_set_cmd_request_argument(uint32_t cmd_argument)
{
	SDIO_CMD_ARGUMENT = cmd_argument;
}

uint32_t sdio_read_cmd_response_argument()
{
	uint32_t cmd_argument = SDIO_CMD_ARGUMENT;
	return cmd_argument;
}

void sdio_write_to_data_buffer(uint32_t data, uint32_t address) 
{
	SDIO_DATA_BUFFER[address] = data;
}

uint32_t sdio_read_from_data_buffer(uint32_t address)
{
	return SDIO_DATA_BUFFER[address];
}
 

uint32_t sdio_send_cmd(uint8_t cmd_request_index, uint32_t cmd_request_argument) 
{
	sdio_set_cmd_request_index(cmd_request_index);
	sdio_set_cmd_request_argument(cmd_request_argument);	
	while(SDIO_SEND_CMD_OP != 0);
	while(SDIO_CMD_BUSY) printf("command busy\n");
	uint32_t status = SDIO_CMD_STATUS;	
	
	printf("command %08X\n", cmd_request_index);
	if(status & 0x00000001) 
		printf("command timeout\n");	
	printf("command response index: %08X\n", status >> 1);

	return status;	
}

uint32_t sdio_send_cmd_and_read_data(uint8_t cmd_request_index, uint32_t cmd_request_argument) 
{
	sdio_set_cmd_request_index(cmd_request_index);
	sdio_set_cmd_request_argument(cmd_request_argument);	
	while(SDIO_SEND_CMD_AND_READ_DATA_OP != 0);
	while(SDIO_CMD_BUSY || SDIO_DATA_BUSY) printf("command or data busy\n");
	uint32_t status = SDIO_CMD_STATUS;	
	
	printf("command %08X\n", cmd_request_index);
	if(status & 0x00000001) 
		printf("command timeout\n");	
	printf("command response index: %08X\n", status >> 1);

	return status;	
}

uint32_t sdio_send_cmd_and_send_data(uint8_t cmd_request_index, uint32_t cmd_request_argument) 
{
	sdio_set_cmd_request_index(cmd_request_index);
	sdio_set_cmd_request_argument(cmd_request_argument);	
	while(SDIO_SEND_CMD_AND_SEND_DATA_OP != 0);
	while(SDIO_CMD_BUSY || SDIO_DATA_BUSY) printf("command or data busy\n");
	uint32_t status = SDIO_CMD_STATUS;	
	
	printf("command %08X\n", cmd_request_index);
	if(status & 0x00000001) 
		printf("command timeout\n");	
	printf("command response index: %08X\n", status >> 1);

	return status;	
}

uint32_t sdio_send_data()
{
	while(SDIO_SEND_DATA_OP != 0);
	while(SDIO_DATA_BUSY) printf("data busy\n");
	
	uint32_t status = SDIO_DATA_STATUS;
	
	printf("send data status: %08X\n", status);
	return status;
}

uint32_t sdio_read_data()
{
	while(SDIO_READ_DATA_OP != 0);
	while(SDIO_DATA_BUSY) printf("data busy\n");
	
	uint32_t status = SDIO_DATA_STATUS;
	
	printf("read data status: %08X\n", status);
	return status;
}

uint32_t sdio_read_data_length()
{
	uint32_t length = SDIO_DATA_LENGTH;
	
	printf("data length set: %08X\n", length);
	return length;
}

void sdio_set_data_length(uint32_t data_length)
{
	SDIO_DATA_LENGTH = data_length;
}

int main(void) {
    irq_setmask(0);
    irq_setie(1);
    uart_init(); 
	busy_wait(5000);    
    printf("\n=== Litex SD Controller ===\n");

	uint32_t main_clock_frequency = sdio_read_main_clock_frequency();
	 // выводим 32-бит целиком
    printf("main_clock = 0x%08X\n", main_clock_frequency);
	uint32_t sd_clock_frequency = sdio_read_sd_clock_frequency();
	 // выводим 32-бит целиком
    printf("sd_clock = 0x%08X\n", sd_clock_frequency);
	
	uint32_t response = sdio_send_cmd(0, 0);
	uint32_t response_argument = 0;
	busy_wait(100);
	
	do {
		response = sdio_send_cmd(5, 0x00000000);
		if(!(response & 0x01)) {
			response_argument = sdio_read_cmd_response_argument();	
		}
		busy_wait(100);
	} while((response & 0x01) || ((response_argument >> 31) != 0x00000001));	
	
	do {
		response = sdio_send_cmd(3, 0);
		busy_wait(100);
	} while(response & 0x01);
	
	response_argument = sdio_read_cmd_response_argument();
	
	do {
		response = sdio_send_cmd(7, response_argument & 0xffff0000);
		busy_wait(100);
	} while(response & 0x01);
	
	do {
		response = sdio_send_cmd(52, 0x88000e02);
		busy_wait(100);
	} while(response & 0x01);
	
	do {
		response = sdio_send_cmd(52, 0x88000402);
		busy_wait(100);
	} while(response & 0x01);
	
	do {
		response = sdio_send_cmd(52, 0x00000600);
		busy_wait(100);
		if(!(response & 0x01)) {
			response_argument = sdio_read_cmd_response_argument();	
		}
	} while((response & 0x01) || ((response_argument & 0x02) == 0));
	
	sdio_set_data_length(32);
	sdio_read_data_length();	
	
	do {
		response = sdio_send_cmd(53, 0x90200020);
		busy_wait(100);
	} while(response & 0x01);
	
	response = sdio_send_data();
	
	if(response & 0x03) printf("data write error");
	
    return 0;

}
