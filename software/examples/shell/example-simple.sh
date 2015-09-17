#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change to your UID

# Set output voltage to 3.3V
tinkerforge call analog-out-v2-bricklet $uid set-output-voltage 3300
