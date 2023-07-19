# tuya_cloud_daemon
Program to control your IoT products and devices in the cloud, using Tuya IoT Core SDK in C.

# Usage
Add wanted changes in ./src/tuya_daemon.c <br>
In project root directory to build and install program run: 
```
make all
sudo make install
```
Execution description
```
tuya_daemon --help
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
To uninstall:
```
make uninstall
```
To clean build files:
```
make clean
```
# Credits & References
https://developer.tuya.com/en/docs/iot/introduction-of-tuya?id=K914joffendwh <br>
https://github.com/andgva2/daemon_program
