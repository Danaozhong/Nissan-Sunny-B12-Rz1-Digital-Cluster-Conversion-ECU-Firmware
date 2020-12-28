# Set environment variables
DIR_STM32_GNU_TOOLCHAIN="D:/Programs/STM32CubeIDE/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.5.0.202011040924/tools/"
DIR_STM32_CUBE="C:/Users/Clemens/STM32Cube/Repository/STM32CubeF3"

# Just run CMake. Make sure that both CMake and make are available in the environment vars.
cmake -DSTM32_CUBE_F3_PATH=$DIR_STM32_CUBE -DSTM32_TOOLCHAIN_PATH=$DIR_STM32_GNU_TOOLCHAIN -DTARGET_TRIPLET=arm-none-eabi -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - Unix Makefiles" Src

    