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
	BC->input_voltage_sum = 0;

    PIN_REF_VOLTAGE.type = PIO_INPUT;
    BA->PIO_Configure(&PIN_REF_VOLTAGE, 1);

	update();
}

void destructor(void) {
	adc_channel_disable(BS->adc_channel);
}

void invocation(const ComType com, const uint8_t *data) {
	switch(((StandardMessage*)data)->header.fid) {
		case FID_SET_OUTPUT_VOLTAGE: {
			set_output_voltage(com, (SetOutputVoltage*)data);
			break;
		}

		case FID_GET_OUTPUT_VOLTAGE: {
			get_output_voltage(com, (GetOutputVoltage*)data);
			break;
		}

		case FID_GET_INPUT_VOLTAGE: {
			get_input_voltage(com, (GetInputVoltage*)data);
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
		BC->input_voltage_sum += BA->adc_channel_get_data(BS->adc_channel);
		if(BC->counter == 0) {
			BC->last_input_voltage_sum = BC->input_voltage_sum;
			BC->input_voltage_sum = 0;
		}
	}
}

void set_output_voltage(const ComType com, const SetOutputVoltage *data) {
	if(data->voltage > ANALOG_MAX_VOLTAGE) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	BC->last_output_voltage = data->voltage;
	update();

	logbli("set_output_voltage: %d\n\r", data->voltage);

	BA->com_return_setter(com, data);
}

void get_output_voltage(const ComType com, const GetOutputVoltage *data) {
	GetOutputVoltageReturn govr;
	govr.header        = data->header;
	govr.header.length = sizeof(GetOutputVoltageReturn);
	govr.voltage       = BC->last_output_voltage;

	BA->send_blocking_with_timeout(&govr, sizeof(GetOutputVoltageReturn), com);
	logbli("get_output_voltage: %d\n\r", govr.voltage);
}

void get_input_voltage(const ComType com, const GetInputVoltage *data) {
	GetInputVoltageReturn givr;
	givr.header        = data->header;
	givr.header.length = sizeof(GetInputVoltageReturn);
	// voltage = input*3300*68/(4095*12*256) = input*935/52416
	givr.voltage       = BC->last_input_voltage_sum*935/52416;

	BA->send_blocking_with_timeout(&givr, sizeof(GetInputVoltageReturn), com);
	logbli("get_input_voltage: %d\n\r", givr.voltage);
}

void update(void) {
	uint8_t address;
	if(BS->address == I2C_EEPROM_ADDRESS_HIGH) {
		address = I2C_ADDRESS_HIGH;
	} else {
		address = I2C_ADDRESS_LOW;
	}

	const uint8_t port = BS->port - 'a';
	const uint16_t output_voltage = BC->last_output_voltage*4095/12000;

	uint8_t byte1 = output_voltage >> 8;
	uint8_t byte2 = output_voltage & 0xFF;

	logbli("update %d\n\r", output_voltage);

	BA->mutex_take(*BA->mutex_twi_bricklet, MUTEX_BLOCKING);
	BA->bricklet_select(port);
    BA->TWID_Write(BA->twid, address, byte1, 1, &byte2, 1, NULL);
    BA->bricklet_deselect(port);
    BA->mutex_give(*BA->mutex_twi_bricklet);
}
