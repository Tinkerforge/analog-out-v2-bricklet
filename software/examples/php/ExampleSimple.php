<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletAnalogOutV2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletAnalogOutV2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Analog Out Bricklet 2.0

$ipcon = new IPConnection(); // Create IP connection
$ao = new BrickletAnalogOutV2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Set output voltage to 3.3V
$ao->setOutputVoltage(3300);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
