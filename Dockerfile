FROM ubuntu:22.04 as base

LABEL maintainer="Clemens Zangl" \
      description="Dockerfile for the Nissan Sunny Rz1 digital cluster conversion project" \
      version="0.1"

ARG TOOLS_PATH=/home/stm32/lib
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y make gcc cmake git curl xz-utils g++

    # Temporary directory
RUN mkdir -p /home/stm32/temp \
    && cd /home/stm32/temp \
# Download gcc-arm-none-eabi toolchain for compiling STM32
    && mkdir ${TOOLS_PATH} \
    && curl -Lo gcc-arm-none-eabi.tar.xz "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz" \
	&& tar xf gcc-arm-none-eabi.tar.xz -C ${TOOLS_PATH} \
	&& rm gcc-arm-none-eabi.tar.xz

# Download stm32-cmake project for using CMake for STM32. G4 family not supported.
RUN cd ${TOOLS_PATH} \
    && git clone --depth=1 https://github.com/ObKo/stm32-cmake \
# Download Firmware for F0, F1, F3, F4, F7, L0, L1, L4, G0, G4 and H7 families.
# Clean not needed files from repository to reduce size.
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF0 \
    #&& mv STM32CubeF0/Drivers Drivers && rm -rf STM32CubeF0 && mkdir STM32CubeF0 && mv Drivers STM32CubeF0/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF1 \
    #&& mv STM32CubeF1/Drivers Drivers && rm -rf STM32CubeF1 && mkdir STM32CubeF1 && mv Drivers STM32CubeF1/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF2 \
    #&& mv STM32CubeF2/Drivers Drivers && rm -rf STM32CubeF2 && mkdir STM32CubeF2 && mv Drivers STM32CubeF2/Drivers \
    && git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF3 \
    && mv STM32CubeF3/Drivers Drivers && rm -rf STM32CubeF3 && mkdir STM32CubeF3 && mv Drivers STM32CubeF3/Drivers
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF4 \
    #&& mv STM32CubeF4/Drivers Drivers && rm -rf STM32CubeF4 && mkdir STM32CubeF4 && mv Drivers STM32CubeF4/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeF7 \
    #&& mv STM32CubeF7/Drivers Drivers && rm -rf STM32CubeF7 && mkdir STM32CubeF7 && mv Drivers STM32CubeF7/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeL0 \
    #&& mv STM32CubeL0/Drivers Drivers && rm -rf STM32CubeL0 && mkdir STM32CubeL0 && mv Drivers STM32CubeL0/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeL1 \
    #&& mv STM32CubeL1/Drivers Drivers && rm -rf STM32CubeL1 && mkdir STM32CubeL1 && mv Drivers STM32CubeL1/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeL4 \
    #&& mv STM32CubeL4/Drivers Drivers && rm -rf STM32CubeL4 && mkdir STM32CubeL4 && mv Drivers STM32CubeL4/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeG0 \
    #&& mv STM32CubeG0/Drivers Drivers && rm -rf STM32CubeG0 && mkdir STM32CubeG0 && mv Drivers STM32CubeG0/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeG4 \
    #&& mv STM32CubeG4/Drivers Drivers && rm -rf STM32CubeG4 && mkdir STM32CubeG4 && mv Drivers STM32CubeG4/Drivers \
    #&& git clone --depth=1 https://github.com/STMicroelectronics/STM32CubeH7 \
    #&& mv STM32CubeH7/Drivers Drivers && rm -rf STM32CubeH7 && mkdir STM32CubeH7 && mv Drivers STM32CubeH7/Drivers \

RUN apt-get autoclean 
RUN apt-get autoremove 
RUN apt-get clean 
RUN rm -rf /var/lib/apt/lists/*


# Create working directory for sharing with repository
RUN mkdir /home/stm32/ws

WORKDIR /home/stm32/ws

# Define paths
ENV STM32_CMAKE_MODULES=/home/stm32/lib/stm32-cmake/cmake \
    STM32_FW_F0=/home/stm32/lib/STM32CubeF0 \
    STM32_FW_F1=/home/stm32/lib/STM32CubeF1 \
    STM32_FW_F2=/home/stm32/lib/STM32CubeF2 \
    STM32_FW_F3=/home/stm32/lib/STM32CubeF3 \
    STM32_FW_F4=/home/stm32/lib/STM32CubeF4 \
    STM32_FW_F7=/home/stm32/lib/STM32CubeF7 \
    STM32_FW_L0=/home/stm32/lib/STM32CubeL0 \
    STM32_FW_L1=/home/stm32/lib/STM32CubeL1 \
    STM32_FW_L4=/home/stm32/lib/STM32CubeL4 \
    STM32_FW_G0=/home/stm32/lib/STM32CubeG0 \
    STM32_FW_G4=/home/stm32/lib/STM32CubeG4 \
    STM32_FW_H7=/home/stm32/lib/STM32CubeH7 \
    ARM_TOOLCHAIN=${TOOLS_PATH}/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi

# Clone GoogleTest
RUN git clone https://github.com/google/googletest.git -b v1.13.0 /home/dependencies/googletest

ENV GTEST_ROOT=/home/dependencies/googletest

# Start from a Bash prompt
CMD /bin/bash -c "chmod +x ./buildall.sh && ./buildall.sh"