<<<<<<< HEAD
cmake_minimum_required (VERSION 3.6)

project(CMSISDSPController)

add_library(CMSISDSPController STATIC)

configdsp(CMSISDSPController ..)

include(interpol)
interpol(CMSISDSPController)

if (CONFIGTABLE AND ALLFAST)
    target_compile_definitions(CMSISDSPController PUBLIC ARM_ALL_FAST_TABLES)  
endif()

target_sources(CMSISDSPController PRIVATE arm_pid_init_f32.c)
target_sources(CMSISDSPController PRIVATE arm_pid_init_q15.c)
target_sources(CMSISDSPController PRIVATE arm_pid_init_q31.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_f32.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_q15.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_q31.c)

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_F32)
target_sources(CMSISDSPController PRIVATE arm_sin_cos_f32.c)
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_Q31)
target_sources(CMSISDSPController PRIVATE arm_sin_cos_q31.c)
endif()



### Includes
target_include_directories(CMSISDSPController PUBLIC "${DSP}/../../Include")



=======
cmake_minimum_required (VERSION 3.6)

project(CMSISDSPController)

add_library(CMSISDSPController STATIC)

configdsp(CMSISDSPController ..)

include(interpol)
interpol(CMSISDSPController)

if (CONFIGTABLE AND ALLFAST)
    target_compile_definitions(CMSISDSPController PUBLIC ARM_ALL_FAST_TABLES)  
endif()

target_sources(CMSISDSPController PRIVATE arm_pid_init_f32.c)
target_sources(CMSISDSPController PRIVATE arm_pid_init_q15.c)
target_sources(CMSISDSPController PRIVATE arm_pid_init_q31.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_f32.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_q15.c)
target_sources(CMSISDSPController PRIVATE arm_pid_reset_q31.c)

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_F32)
target_sources(CMSISDSPController PRIVATE arm_sin_cos_f32.c)
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_Q31)
target_sources(CMSISDSPController PRIVATE arm_sin_cos_q31.c)
endif()



### Includes
target_include_directories(CMSISDSPController PUBLIC "${DSP}/../../Include")



>>>>>>> 57cca87149b6a2c58469570993efa5876b986ef6
