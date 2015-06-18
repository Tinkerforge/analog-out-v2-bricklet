function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "brY"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    ao = java_new("com.tinkerforge.BrickletAnalogOutV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set output voltage to 3.3V
    ao.setOutputVoltage(3300);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end
