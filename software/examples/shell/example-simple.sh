#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# set a voltage of 3.3V
tinkerforge call analog-out-v2-bricklet $uid set-output-voltage 3300
