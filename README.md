# Nissan Sunny Rz1 Digital Cluster Conversion Project

This SW allows installing a JDM digital cluster for a Nissan Sunny B12 Rz1 into a LHD EUDM/USDM Nisssan Sunny B12. It hooks between the vehicle harness and the digital cluster and converts the signals.

Actually converted are the following signals:

* The standard fuel sensor signal doesn't match to the one the digital cluster expects. The standard fuel sensor is resistor based, while the digital cluster expects a voltage signal. This SW cyclically reads out the current fuel sensor resistor on the ADC, converts it into a voltage, and then provides the correct voltage via a DAC + OpAmp.
* The speed signal of the standard cluster is provided mechanically, using a spinning wire. The digital cluster expects the vehicle speed as a PWM signal. To make this work, a readily available speed sensor converter is used. This fits the Nissans gearbox, and the PWM generated by this sensor is read in via a capture / compare. It is then multiplied to match the correct frequency the digital cluster expects, and provided to the cluster.

For more details, please check my homepage under www.texelography.com.

To run this, you will need an STM32F303CCT6 MCU (I am using the RobotDyn board), and a PCB providing all the peripherals. The schematics are provided on my homepage.

## Details

The SW was developed with a platform approach in mind. The base OS is FreeRTOS with C++11 support, and the build environment is using CMake.
The platform also supports components that are currently not used, e.g.an AUTOSAR CAN driver can be enabled, if needed.
The tool environment is loaded via Docker.

## How to Build?

You do not need to install any toolchain; the environment is provided by using docker.

Run 
```
./run_docker.sh
```
to start the STM32 build environment docker container, and get the tool environment.

To generate CMake for a release configuration, please execute
```
./run_cmake.sh
```
If you want to build a debug variant, just execute instead:
```
./run_cmake_debug.sh
```

Afterwards, just build using
```
make
```
The binaries will be generated in bin_Release or bin_Debug.