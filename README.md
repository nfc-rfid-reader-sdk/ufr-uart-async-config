D-Logic uFR UID asynchronous mode configuration tool

Parameters:
-c              - Com port name, on witch uFR is attached. Default is "/dev/ttyUSB0".
-m [on|off|get
          |init]- Turn uFR asynchronous mode on or off. Default is [on].
                  Use [get] to read settings stored in uFR.
                  Use [init] for new devices to set default parameters.
-s com_speed    - Set communication baud_rate. Default is 1Mbps.
-e [on|off]     - Enable prefix character.
-r [on|off]     - Enable transmit on tag remove event.
-p 00           - Define prefix character (ascii hex representation).
-f 00           - Define sufix character (ascii hex representation).
-v              - Print tool version.

Tool will read existing parameters from the uFR device, apply changes and if everything is Ok, write back configuration to the device.

Note:
1.) -r setting depends on -e option and vice versa. Tool is aware about it.
2.) Default port is "/dev/ttyUSB0". If this is different on some host machine, use -c parameter.

Examples:
./ufr-uart-async-config
will print help screen.

./ufr-uart-async-config -m get
will print UID asynchronous mode settings stored in the attached uFR device.

./ufr-uart-async-config -c /dev/ttyUSB4 -m get
will print UID asynchronous mode settings stored in the attached uFR device on the /dev/ttyUSB4 port.

./ufr-uart-async-config -m on -s 115200 -f 0d
will activate UID asynchronous mode on the attached uFR device and set asynchronous mode baud rate to 115200 bps and set sufix character to 0x0D (carriage return control character).

./ufr-uart-async-config -f 30
will set sufix character to 0x30 ('0').
