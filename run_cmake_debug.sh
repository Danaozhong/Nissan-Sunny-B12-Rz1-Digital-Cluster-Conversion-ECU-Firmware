# Set environment variables, if not set by parameter
if [ -z ${BUILD_CONFIG+x} ] 
then
    # Build type not set, set RELEASE as default
    BUILD_CONFIG=Debug
fi

if [ -z ${ARM_TOOLCHAIN+x} ]
then 
    # Build type not set, set RELEASE as default
    ARM_TOOLCHAIN="F:/Programs/STM32CubeIDE/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.5.0.202011040924/tools/"
fi

if [ -z ${STM32_FW_F3=+x} ]
then
    # Build type not set, set RELEASE as default
    STM32_FW_F3=="C:/Users/Clemens/STM32Cube/Repository/STM32CubeF3"
fi

if [ -z ${STM32_CHIP+x} ]
then
    # Build type not set, set RELEASE as default
    STM32_CHIP="STM32F303CCT6"
fi

#For F4
#DIR_STM32_CUBE="C:/Users/Clemens/STM32Cube/Repository/STM32Cube_FW_F4_V1.24.0"
#STM32_CHIP="STM32F429ZIT6"
echo "ARM Toolchain path is " $ARM_TOOLCHAIN
echo "Build Type is " $BUILD_CONFIG
echo "STM32_CHIP " $STM32_CHIP
echo "STM32F3 firmware is " $STM32_FW_F3

# Just run CMake. Make sure that both CMake and make are available in the environment vars.
cmake -DSTM32_CUBE_F3_PATH=$STM32_FW_F3 -DSTM32_TOOLCHAIN_PATH=$ARM_TOOLCHAIN -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -G "Eclipse CDT4 - Unix Makefiles" src