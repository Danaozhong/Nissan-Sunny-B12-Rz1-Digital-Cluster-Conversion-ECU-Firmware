docker run --rm -i -t -v /${PWD}:/home/stm32/ws cortesja/stm32-cmake:latest bash
#docker run --rm --privileged -h stm32_bash -i -t -v $(pwd):/build lpodkalicki/stm32-toolchain # arm-none-eabi-cpp
#docker run -h stm32_bash -i -t --rm --privileged -v $(pwd):/home/stm32/ws cortesja/stm32-cmake:latest bash # -c "sh run_cmake.sh"
#docker run -h stm32_bash -i -t -v H:/Projekte/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303_copy/home/stm32/ws cortesja/stm32-cmake:latest bash 
#rem docker run --rm -v ${PWD}:/home/stm32/ws cortesja/stm32-cmake:latest bash -c "sh docker/compile.sh`