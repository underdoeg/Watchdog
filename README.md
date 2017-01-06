# Watchdog
Watchdog application for linux with an optional GTK interface. App output will be routed into daily log files that can optionally be uploaded onto an ftp server.

<img src="https://cloud.githubusercontent.com/assets/243820/21504955/3c6922d4-cc63-11e6-923e-6aa07ab502aa.png" width="550" />

## dependencies
- libconfig++
- pstreams
- boost filesystem
- libCURL

## building
```
mkdir build
cd build
cmake ..
cp ../config.sample.cfg config.cfg
make
```

To build with the gtk interface, run cmake with ``` cmake -DWITH_GTK_GUI=ON .. ```  
The gui can still be deactivated by adding the --nogui argument.  
