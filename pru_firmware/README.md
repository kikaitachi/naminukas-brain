# PRU firmware for reading SBUS via pin E4

Build and run:
```
make TARGET=sbus.pru0
```

Stop:
```
make TARGET=sbus.pru0 stop
```

Install:
```
sudo cp ./am335x-pru0-sbus-fw /lib/firmware/
```
