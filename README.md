# Watchdog
Watchdog application for linux with an optional GTK interface

<img src="https://cloud.githubusercontent.com/assets/243820/21504587/8233a95e-cc60-11e6-9342-10c3f22b947c.png" width="450" />

## dependencies
- libconfig++
- pstreams
- boost filesystem

## building
```
mkdir build
cd build
cmake ..
cp ../config.sample.cfg config.cfg
make
```

To build with the gtk interface, run cmake with ``` cmake -DWITH_GTK_GUI=ON .. ```  
The gui can still be deaktivated by adding the --nogui argument.  
