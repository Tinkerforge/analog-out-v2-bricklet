/* analog-out-v2-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * analog-out.c: Implementation of Analog Out 2.0 Bricklet messages
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

#include "analog-out.h"

#include "bricklib/bricklet/bricklet_communication.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"
#include "bricklib/drivers/adc/adc.h"

#include "brickletlib/bricklet_debug.h"
#include "brickletlib/bricklet_entry.h"

#include "config.h"

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE, "BrickContext too big");

	adc_channel_enable(BS->adc_channel);
	BC->counter = 0;
	BC->voltage_ref = ANALOG_MAX_VOLTAGE;
	BC->voltage_sum = 0;
	BC->voltage = 0;
	BC->mode = 1;

    PIN_REF_VOLTAGE.type = PIO_INPUT;
    BA->PIO_Configure(&PIN_REF_VOLTAGE, 1);

	update();
}

void destructor(void) {
	adc_channel_disable(BS->adc_channel);
}

void invocation(const ComType com, const uint8_t *data) {
	switch(((StandardMessage*)data)->header.fid) {
		case FID_SET_VOLTAGE: {
			set_voltage(com, (SetVoltage*)data);
			break;
		}

		case FID_GET_VOLTAGE: {
			get_voltage(com, (GetVoltage*)data);
			break;
		}

		case FID_SET_MODE: {
			set_mode(com, (SetMode*)data);
			break;
		}

		case FID_GET_MODE: {
			get_mode(com, (GetMode*)data);
			break;
		}

		default: {
			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			break;
		}
	}
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		BC->counter++;
		BC->voltage_sum += ADC_TO_REF(BA->adc_channel_get_data(BS->adc_channel));
		if(BC->counter == 0) {
			BC->voltage_ref = BC->voltage_sum/256;
			BC->voltage_sum = 0;
			logbld("voltage_ref: %d\n\r", BC->voltage_ref);
		}
	}
}

void set_voltage(const ComType com, const SetVoltage *data) {
	if(data->voltage > ANALOG_MAX_VOLTAGE) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	BC->voltage = data->voltage;
	BC->mode    = 0;
	update();

	logbli("set_voltage: %d\n\r", data->voltage);

	BA->com_return_setter(com, data);
}

void get_voltage(const ComType com, const GetVoltage *data) {
	GetVoltageReturn gvr;
	gvr.header        = data->header;
	gvr.header.length = sizeof(GetVoltageReturn);
	gvr.voltage       = BC->voltage;

	BA->send_blocking_with_timeout(&gvr, sizeof(GetVoltageReturn), com);
	logbli("get_voltage: %d\n\r", gvr.voltage);
}

void set_mode(const ComType com, const SetMode *data) {
	if(data->mode > 3) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	BC->mode    = data->mode;
	BC->voltage = 0;
	update();

	logbli("set_mode: %d\n\r", data->mode);

	BA->com_return_setter(com, data);
}

void get_mode(const ComType com, const GetMode *data) {
	GetModeReturn gmr;

	gmr.header        = data->header;
	gmr.header.length = sizeof(GetModeReturn);
	gmr.mode          = BC->mode;

	BA->send_blocking_with_timeout(&gmr, sizeof(GetModeReturn), com);

	logbli("get_mode: %d\n\r", gmr.mode);
}

void update(void) {
	uint8_t address;
	if(BS->address == I2C_EEPROM_ADDRESS_HIGH) {
		address = I2C_ADDRESS_HIGH;
	} else {
		address = I2C_ADDRESS_LOW;
	}

	const uint8_t port = BS->port - 'a';

	BC->voltage = MIN(BC->voltage, BC->voltage_ref);

	uint16_t value = BC->voltage*MAX_ADC_VALUE/BC->voltage_ref;
	value |= (BC->mode << 12);

	uint8_t byte1 = value >> 8;
	uint8_t byte2 = value & 0xFF;

	BA->mutex_take(*BA->mutex_twi_bricklet, MUTEX_BLOCKING);
	BA->bricklet_select(port);
    BA->TWID_Write(BA->twid, address, byte1, 1, &byte2, 1, NULL);
    BA->bricklet_deselect(port);

    BA->mutex_give(*BA->mutex_twi_bricklet);

    logbli("update: %d %d\n\r", byte1, byte2);
}
