# tuya_cloud_daemon
Daemon program, that uses Tuya IoT Core SDK in C to connect and control your IoT products and devices in the cloud.

# Usage
Add wanted changes in ./src/tuya_daemon.c <br>
In ./ execute: 

```
make clean
make all
cd bin
./tuya_daemon [device_id] [device_secret] [product_id]
```

# Credits & References
https://developer.tuya.com/en/docs/iot/introduction-of-tuya?id=K914joffendwh <br>
https://github.com/andgva2/daemon_program
