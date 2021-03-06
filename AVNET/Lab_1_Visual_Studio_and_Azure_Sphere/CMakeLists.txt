﻿#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzureSphereIoTCentral C)

# add_subdirectory("../libs/MT3620_Grove_Shield/MT3620_Grove_Shield_Library" out)

set(Source
    "main.c"
    "../shared/globals.c"
    "../shared/azure_iot.c"
    "../shared/peripheral.c"
    "../shared/device_twins.c"
    "../shared/direct_methods.c"
    "../shared/timer.c"
    "../shared/terminate.c"
    "../shared/eventloop_timer_utilities.c"
    "../shared/parson.c"
    "../shared/inter_core.c"
    "../shared/oem/board.c"
)
source_group("Source" FILES ${Source})

set(Avnet
    "../shared/oem/lps22hh_reg.c"
    "../shared/oem/lsm6dso_reg.c"
    "../shared/oem/imu_temp_pressure.c"
    "../shared/oem/light_sensor.c"
)
source_group("Avnet" FILES ${Avnet})

set(ALL_FILES
    ${Source}
    ${Avnet}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azureiot)

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unknown-pragmas)
set_source_files_properties(../shared/oem/lsm6dso_reg.c PROPERTIES COMPILE_FLAGS -Wno-conversion)
set_source_files_properties(../shared/oem/lps22hh_reg.c PROPERTIES COMPILE_FLAGS -Wno-conversion)

set_source_files_properties(../shared/oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-float-conversion)
set_source_files_properties(../shared/oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types)
set_source_files_properties(../shared/oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-conversion)

# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
