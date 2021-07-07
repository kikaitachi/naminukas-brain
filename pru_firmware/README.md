# PRU firmware for reading SBUS via pin E4

Build and run:
```
make TARGET=sbus.pru0
make TARGET=fport.pru0
```

Stop:
```
make TARGET=sbus.pru0 stop
make TARGET=fport.pru0 stop
```

Install:
```
sudo cp ./am335x-pru0-sbus-fw /lib/firmware/
sudo cp ./am335x-pru0-fport-fw /lib/firmware/
```
