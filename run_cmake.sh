
# Set environment variables, if not set by parameter
if [ -z ${BUILD_CONFIG+x} ] 
then
    # Build type not set, set RELEASE as default
    BUILD_CONFIG=Release
fi

if [ -z ${DIR_STM32_GNU_TOOLCHAIN+x} ]
then 
    # Build type not set, set RELEASE as default
    DIR_STM32_GNU_TOOLCHAIN="D:/Programs/STM32CubeIDE/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.0.0.201904181610/tools"
fi

if [ -z ${DIR_STM32_CUBE+x} ]
then
    # Build type not set, set RELEASE as default
    DIR_STM32_CUBE="C:/Users/Clemens/STM32Cube/Repository/STM32CubeF3"
fi

if [ -z ${STM32_CHIP+x} ]
then
    # Build type not set, set RELEASE as default
    STM32_CHIP="STM32F303CCT6"
fi


#For F4
#DIR_STM32_CUBE="C:/Users/Clemens/STM32Cube/Repository/STM32Cube_FW_F4_V1.24.0"
#STM32_CHIP="STM32F429ZIT6"


echo "Build Type is " $BUILD_CONFIG
echo "STM32_CHIP " $STM32_CHIP
# Just run CMake. Make sure that both CMake and make are available in the environment vars.
#cmake -DSTM32_CHIP=$STM32_CHIP -DSTM32Cube_DIR=$DIR_STM32_CUBE -DTOOLCHAIN_PREFIX=$DIR_STM32_GNU_TOOLCHAIN -DCMAKE_MODULE_PATH=cmake -DCMAKE_TOOLCHAIN_FILE=cmake/gcc_stm32.cmake -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -G "Eclipse CDT4 - Unix Makefiles" Src

