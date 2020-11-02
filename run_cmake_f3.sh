# Set environment variables
DIR_STM32_GNU_TOOLCHAIN="D:/Programs/STM32CubeIDE/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.0.0.201904181610/tools"
DIR_STM32_CUBE="C:/Users/Clemens/STM32Cube/Repository/STM32CubeF3"
STM32_CHIP="STM32F303CCT6"

echo "STM32_CHIP " $STM32_CHIP
# Just run CMake. Make sure that both CMake and make are available in the environment vars.
cmake -DSTM32_CHIP=$STM32_CHIP -DSTM32Cube_DIR=$DIR_STM32_CUBE -DTOOLCHAIN_PREFIX=$DIR_STM32_GNU_TOOLCHAIN -DCMAKE_MODULE_PATH=cmake -DCMAKE_TOOLCHAIN_FILE=cmake/gcc_stm32.cmake -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - Unix Makefiles" Src