/*
 * LED74HC595.c
 *
 *  Created on: Aug 6, 2023
 *      Author: Korzhak
 */


#include "LED74HC595.h"

LED74HC595 * _ledObj;


static const uint8_t digitPatterns[16] = {
	0b11000000,  // 0
	0b11111001,  // 1
	0b10100100,  // 2
	0b10110000,  // 3
	0b10011001,  // 4
	0b10010010,  // 5
	0b10000010,  // 6
	0b11111000,  // 7
	0b10000000,  // 8
	0b10010000,  // 9
	0b10111111,  // - (dash)
	0b11110111,  // _ (underscore)
	0b11000110,  // C
	0b10000110,  // E
	0b10001110,  // F
	0b10011100   // ° (degree)
};


void setUp(LED74HC595 *ledObj, uint16_t sclkPin, GPIO_TypeDef * sclkPort, uint16_t rclkPin,
		GPIO_TypeDef * rclkPort, uint16_t dioPin, GPIO_TypeDef * dioPort) {
	_ledObj = ledObj;
	_ledObj->sclkPin = sclkPin;
	_ledObj->sclkPort = sclkPort;
	_ledObj->rclkPin = rclkPin;
	_ledObj->rclkPort = rclkPort;
	_ledObj->dioPin = dioPin;
	_ledObj->dioPort = dioPort;

	for (int i = 0; i < 4; i++) {
	  _ledObj->_digitSets[i]   = (uint8_t) SEG7_OFF;
	  _ledObj->_digitValues[i] = (uint8_t) SEG7_OFF;
	}

	_ledObj->_digitDots      = 0x00;
}


void clear() {
	_ledObj->_digitDots = 0x00;

	for (int i = 0; i < 4; i++)
		_ledObj->_digitSets[i] = 0xFF;
}


void setDot(int pos) {
  if (pos < 1 || pos > 4)
    return;

  _ledObj->_digitDots |= 1 << (pos - 1);
}


void setNumber(int pos, int value) {
  if (pos < 1 || pos > 4)
    return;

  if (value < 0 || pos > 9)
    return;

  _ledObj->_digitSets[pos - 1] = digitPatterns[value];
}


void setChar(int pos, SegChars value) {
	if (pos < 1 || pos > 4)
		return;

	switch (value) {
		case __DASH:
			_ledObj->_digitSets[pos - 1] = digitPatterns[10];
			  break;

		case __UNDERSCORE:
			_ledObj->_digitSets[pos - 1] = digitPatterns[11];
			break;

		case __C:
			_ledObj->_digitSets[pos - 1] = digitPatterns[12];
			break;

		case __E:
			_ledObj->_digitSets[pos - 1] = digitPatterns[13];
			break;

		case __F:
			_ledObj->_digitSets[pos - 1] = digitPatterns[14];
			break;

		case __DEGREE:
			_ledObj->_digitSets[pos - 1] = digitPatterns[15];
			break;
  }
}

void show() {
	for (int i = 0; i < 4; i++)
		_ledObj->_digitValues[i] = _ledObj->_digitSets[i];
}

void loop() {
	for (int i = 0; i < 4; i++) {
		int digit = 0x08 >> i;
		int value = _ledObj->_digitValues[i];

		if (_ledObj->_digitDots & (1 << i))
			value &= 0x7F;

		shift(value);
		shift(digit);

		// RCLK LOW and HIGH level settings
		HAL_GPIO_WritePin(_ledObj->rclkPort, _ledObj->rclkPin, RESET);
		HAL_GPIO_WritePin(_ledObj->rclkPort, _ledObj->rclkPin, SET);
	}
}

void shift(uint8_t value) {
	for (uint8_t i = 8; i >= 1; i--) {
		if (value & 0x80)
			HAL_GPIO_WritePin(_ledObj->dioPort, _ledObj->dioPin, SET);
		else
			HAL_GPIO_WritePin(_ledObj->dioPort, _ledObj->dioPin, RESET);

    value <<= 1;
    HAL_GPIO_WritePin(_ledObj->sclkPort, _ledObj->sclkPin, RESET);
    HAL_GPIO_WritePin(_ledObj->sclkPort, _ledObj->sclkPin, SET);
  }
}



void setInt(int number, bool zeroPadding) {
	if (number > 9999) // trim
		number %=  10000;
	else if (number < -999) // trim
		number %= 1000;

	int digitNum = 0;
	int numberAbs = abs(number);

	if (numberAbs <= 9999 && numberAbs >= 1009)
		digitNum = 4;
	else  if (numberAbs <= 999 && numberAbs >= 100)
		digitNum = 3;
	else if (numberAbs <= 99 && numberAbs >= 10)
		digitNum = 2;
	else if (numberAbs <= 9 && numberAbs >= 0)
		digitNum = 1;

	if (number < 0)
		digitNum++;

	int denominator = 10000;
	for (int pos = 1; pos <= 4; pos++) {
		denominator /= 10;
		if (!zeroPadding && pos <= (4 - digitNum))
			continue;

		if (number < 0) {
			setChar(pos, __DASH); // set - at the 1st digit
			number *= -1;
		} else {
			int digit = number / denominator;
			number = number % denominator;
			setNumber(pos, digit);
		}
	}
}

void printInt(int number, bool zeroPadding) {
	clear();
	setInt(number, zeroPadding);
	show(); // show on the display
}

void printFloat(float number, int decimalPlace, bool zeroPadding) {
	clear();
	if (decimalPlace > 3)
		decimalPlace = 3;
	else if (decimalPlace < 0)
		decimalPlace = 0;

	if (decimalPlace != 0)
		setDot(4 - decimalPlace);

	int integer = number * pow(10, decimalPlace);

	setInt(integer, zeroPadding);
	show(); // show on the display
}
