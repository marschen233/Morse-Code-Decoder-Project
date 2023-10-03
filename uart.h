/*
 * uart.h
 *
 * Created: 2023/9/29 12:50:55
 *  Author: marsc
 */ 



#ifndef UART_H
#define UART_H

void UART_init(int prescale);

void UART_send( unsigned char data);

void UART_putstring(char* StringPtr);





#endif /* UART_H_ */