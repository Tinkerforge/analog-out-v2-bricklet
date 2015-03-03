/* analog-out-v2-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * analog-out.h: Implementation of Analog Out 2.0 Bricklet messages
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef ANALOG_OUT_H
#define ANALOG_OUT_H

#include <stdint.h>
#include "bricklib/com/com_common.h"

#define ANALOG_MAX_VOLTAGE 5000

#define I2C_EEPROM_ADDRESS_HIGH 84
#define I2C_ADDRESS_HIGH 97 //0b1100001
#define I2C_ADDRESS_LOW 96 //0b1100000

#define FID_SET_VOLTAGE 1
#define FID_GET_VOLTAGE 2
#define FID_SET_MODE    3
#define FID_GET_MODE    4

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) StandardMessage;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) SetVoltage;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetVoltage;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) GetVoltageReturn;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) SetMode;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMode;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) GetModeReturn;

void set_voltage(const ComType com, const SetVoltage *data);
void get_voltage(const ComType com, const GetVoltage *data);
void set_mode(const ComType com, const SetMode *data);
void get_mode(const ComType com, const GetMode *data);


void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);

void update(void);

#endif
