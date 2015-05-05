function matlab_example_simple()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletAnalogOutV2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'brY'; % Change to your UID
    
    ipcon = IPConnection(); % Create IP connection
    ao = BrickletAnalogOutV2(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set output voltage to 3.3V
    ao.setOutputVoltage(3300);

    input('Press any key to exit...\n', 's');
    ipcon.disconnect();
end
