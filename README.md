# tuya_cloud_daemon
Program to control your IoT products and devices in the cloud, using Tuya IoT Core SDK in C.

# Usage
Add wanted changes in ./src/tuya_daemon.c <br>
In ./ execute: 
```
make clean
make all
cd bin
./tuya_daemon [OPTION...] DeviceID Device_Secret ProductID
```
To see program logs use
```
journalctl -f
```
To kill the program, find its PID with:
```
ps -ef | grep tuya_daemon
```
And kill the one with PPID of systemd. To check if PPID is systemd use:
```
ps [PPID]
```
Kill with:
```
kill [PID]
```

# Credits & References
https://developer.tuya.com/en/docs/iot/introduction-of-tuya?id=K914joffendwh <br>
https://github.com/andgva2/daemon_program
