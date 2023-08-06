/*
 * LED74HC595.h
 *
 *  Created on: Aug 6, 2023
 *      Author: Korzhak
 */

#ifndef INC_LED74HC595_H_
#define INC_LED74HC595_H_

#include "main.h"
#include "stdint.h"
#include "stdbool.h"

#define SEG7_OFF 0xFF


typedef enum {
  __DASH,
  __UNDERSCORE,
  __C,
  __E,
  __F,
  __DEGREE
} SegChars;


typedef struct {
	uint16_t sclkPin;
	GPIO_TypeDef * sclkPort;

	uint16_t rclkPin;
	GPIO_TypeDef * rclkPort;

	uint16_t dioPin;
	GPIO_TypeDef * dioPort;

	uint8_t _digitSets[4];
	uint8_t _digitValues[4];
	uint8_t _digitDots;
} LED74HC595;


// PRIVATE FUNCTIONS
void shift(uint8_t value);
void setInt(int number, bool zeroPadding);

// PUBLIC FUNCTIONS
void setUp(LED74HC595 *ledObj, uint16_t sclkPin, GPIO_TypeDef * sclkPort, uint16_t rclkPin,
		GPIO_TypeDef * rclkPort, uint16_t dioPin, GPIO_TypeDef * dioPort);
void clear();
void setDot(int pos);
void setNumber(int pos, int value);
void setChar(int pos, SegChars value);
void show();
void loop();
void printInt(int number, bool zero_padding);
void printFloat(float number, int decimal_place, bool zero_padding);


#endif /* INC_LED74HC595_H_ */
