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

int main(void) {
    irq_setmask(0);
    irq_setie(1);
    uart_init(); 
	busy_wait(1000);    
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
		response = sdio_send_cmd(8, 0x000001aa);
		busy_wait(100);
	} while(response & 0x01);
		
	uint8_t inited = 0;
	do {
		do {
			response = sdio_send_cmd(55, 0);
			busy_wait(100);
		} while(response & 0x01);
	
		response = sdio_send_cmd(41, 0x40100000);
		busy_wait(100);
		response_argument = sdio_read_cmd_response_argument();
    	printf("response argument = 0x%08X\n", response_argument);
		if(!(response & 0x01) && (response_argument >> 31 & 0x00000001))
			inited = 1; 
			
	} while(!inited);
	
	
	do {
		response = sdio_send_cmd(2, 0);
		busy_wait(100);
	} while(response & 0x01) ;
	
	do {
		response = sdio_send_cmd(3, 0);
		busy_wait(100);
	} while(response & 0x01);
	
	response_argument = sdio_read_cmd_response_argument();
	
	do {
		response = sdio_send_cmd(7, response_argument & 0xffff0000);
		busy_wait(100);
	} while(response & 0x01);
	
	inited = 0;
	do {
		do {
			response = sdio_send_cmd(55, response_argument & 0xffff0000);
			busy_wait(100);
		} while(response & 0x01);
	
		response = sdio_send_cmd(6, 0x00000002);
		busy_wait(100);
		response_argument = sdio_read_cmd_response_argument();
		if(!(response & 0x01))
			inited = 1; 
			
	} while(!inited);
	
	do {
		response = sdio_send_cmd(16, 0x00000200);
		busy_wait(100);
	} while(response & 0x01);

	
	do {
		response = sdio_send_cmd_and_read_data(17, 0x00000000);
		busy_wait(100);
	} while(response & 0x01);

	uint32_t read_address = 0;
	uint32_t read_data = 0;
	
	busy_wait(1000);
	
	for(int i = 0; i < 32; i++) 
	{
		for(int j = 0; j < 4; j++)
		{
			read_data = sdio_read_from_data_buffer(read_address);
			printf("0x%08X ", read_data);
			read_address++;		
		}
		printf("\n");	
	}
	
	uint32_t write_address = 0;
	
	for(int i = 0; i < 128; i++) 
	{
		sdio_write_to_data_buffer(0x77777777, write_address);	
		write_address++;
	}
	
	do {
		response = sdio_send_cmd(24, 0x00000000);
		busy_wait(100);
	} while(response & 0x01);
	
	response = sdio_send_data();
	
	if(response & 0x03) printf("data write error");
	
	do {
		response = sdio_send_cmd_and_read_data(17, 0x00000000);
		busy_wait(100);
	} while(response & 0x01);
	
	read_address = 0;
	
	for(int i = 0; i < 32; i++) 
	{
		for(int j = 0; j < 4; j++)
		{
			read_data = sdio_read_from_data_buffer(read_address);
			printf("0x%08X ", read_data);
			read_address++;		
		}
		printf("\n");	
	}
	
	printf("end\n");
	
    return 0;

}
