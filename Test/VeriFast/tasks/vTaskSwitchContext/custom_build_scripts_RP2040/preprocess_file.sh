#!/bin/bash
SRC_FILE="$1"
OUT_FILE="$2"
ERR_FILE="$3"
REPO_BASE_DIR="$4"
VF_PROOF_BASE_DIR="$5"
VF_DIR="$6"

PP_SCRIPT_WD=`pwd`
VF_PROOF_MOD_SRC_DIR="$VF_PROOF_BASE_DIR/src"
VF_PROOF_MOD_HEADER_DIR="$VF_PROOF_BASE_DIR/include"


PROOF_SETUP_DIR="$VF_PROOF_BASE_DIR/proof_setup"
PROOF_FILES_DIR="$VF_PROOF_BASE_DIR/proof"
PICO_SDK_DIR="$VF_PROOF_BASE_DIR/sdks/pico-sdk"
SMP_DEMO_DIR="$VF_PROOF_BASE_DIR/demos/FreeRTOS-SMP-Demos"


#LOG_DIR="`pwd`/build_logs"
#BUILD_LOG="$LOG_DIR/build_log--`date +'%y_%m_%d--%H_%M'`.txt"
TIMESTAMP=`date +'%y_%m_%d--%H_%M'`
LOG_PP_OUT_DIR="$PP_SCRIPT_WD/log_preprocessed_files"
LOG_PP_TASK_C="$LOG_PP_OUT_DIR/tasks--pp--$TIMESTAMP.c"
LOG_PP_ERR="$LOG_PP_OUT_DIR/error--$TIMESTAMP.c"

LOG_VF_RW_TASK_C="$LOG_PP_OUT_DIR/tasks--vf_rw--$TIMESTAMP.c"

PP_OUT_DIR="$VF_PROOF_BASE_DIR/preprocessed_files"
PP_TASK_C="$PP_OUT_DIR/tasks__pp.c"


# Flags to SKIP expensive proofs:
# - VERIFAST_SKIP_BITVECTOR_PROOF__STACK_ALIGNMENT

declare -a BUILD_FLAGS
BUILD_FLAGS=(
    -DFREE_RTOS_KERNEL_SMP=1
    -DLIB_FREERTOS_KERNEL=1
    -DLIB_PICO_BIT_OPS=1
    -DLIB_PICO_BIT_OPS_PICO=1
    -DLIB_PICO_DIVIDER=1
    -DLIB_PICO_DIVIDER_HARDWARE=1
    -DLIB_PICO_DOUBLE=1
    -DLIB_PICO_DOUBLE_PICO=1
    -DLIB_PICO_FLOAT=1
    -DLIB_PICO_FLOAT_PICO=1
    -DLIB_PICO_INT64_OPS=1
    -DLIB_PICO_INT64_OPS_PICO=1
    -DLIB_PICO_MALLOC=1
    -DLIB_PICO_MEM_OPS=1
    -DLIB_PICO_MEM_OPS_PICO=1
    -DLIB_PICO_MULTICORE=1
    -DLIB_PICO_PLATFORM=1
    -DLIB_PICO_PRINTF=1
    -DLIB_PICO_PRINTF_PICO=1
    -DLIB_PICO_RUNTIME=1
    -DLIB_PICO_STANDARD_LINK=1
    -DLIB_PICO_STDIO=1
    -DLIB_PICO_STDIO_UART=1
    -DLIB_PICO_STDLIB=1
    -DLIB_PICO_SYNC=1
    -DLIB_PICO_SYNC_CORE=1
    -DLIB_PICO_SYNC_CRITICAL_SECTION=1
    -DLIB_PICO_SYNC_MUTEX=1
    -DLIB_PICO_SYNC_SEM=1
    -DLIB_PICO_TIME=1
    -DLIB_PICO_UTIL=1
    -DPICO_BOARD=\"pico\"
    -DPICO_BUILD=1
    -DPICO_CMAKE_BUILD_TYPE=\"Release\"
    -DPICO_COPY_TO_RAM=0
    -DPICO_CXX_ENABLE_EXCEPTIONS=0
    -DPICO_NO_FLASH=0
    -DPICO_NO_HARDWARE=0
    -DPICO_ON_DEVICE=1
    -DPICO_STACK_SIZE=0x1000
    -DPICO_TARGET_NAME=\"on_core_one\"
    -DPICO_USE_BLOCKED_RAM=0
    -DmainRUN_FREE_RTOS_ON_CORE=1
)

delcare -a PICO_INCLUDE_FLAGS
PICO_INCLUDE_FLAGS=(
    -I"$PICO_SDK_DIR/src/boards/include"
    -I"$PICO_SDK_DIR/src/common/pico_base/include"
    -I"$PICO_SDK_DIR/src/common/pico_binary_info/include"
    -I"$PICO_SDK_DIR/src/common/pico_bit_ops/include"
    -I"$PICO_SDK_DIR/src/common/pico_divider/include"
    -I"$PICO_SDK_DIR/src/common/pico_stdlib/include"
    -I"$PICO_SDK_DIR/src/common/pico_sync/include"
    -I"$PICO_SDK_DIR/src/common/pico_time/include"
    -I"$PICO_SDK_DIR/src/common/pico_util/include"
    -I"$PICO_SDK_DIR/src/rp2040/hardware_regs/include"
    -I"$PICO_SDK_DIR/src/rp2040/hardware_structs/include"
    -I"$PICO_SDK_DIR/src/rp2_common/boot_stage2/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_base/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_claim/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_clocks/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_divider/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_exception/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_gpio/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_irq/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_pll/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_resets/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_sync/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_timer/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_uart/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_vreg/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_watchdog/include"
    -I"$PICO_SDK_DIR/src/rp2_common/hardware_xosc/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_bootrom/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_double/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_float/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_int64_ops/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_malloc/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_multicore/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_platform/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_printf/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_runtime/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_stdio/include"
    -I"$PICO_SDK_DIR/src/rp2_common/pico_stdio_uart/include" 
)

declare -a RP2040_INCLUDE_FLAGS
RP2040_INLCUDE_FLAGS=(
    -I"$SMP_DEMO_DIR/FreeRTOS/Demo/CORTEX_M0+_RP2040/OnEitherCore" 
    -I"$SMP_DEMO_DIR/FreeRTOS/Demo/CORTEX_M0+_RP2040/build/generated/pico_base" 
    -I"$REPO_BASE_DIR/portable/ThirdParty/GCC/RP2040/include" 
    -I"$REPO_BASE_DIR/portable/ThirdParty/GCC/RP2040" 
)

declare -a VERIFAST_FLAGS
VERIFAST_FLAGS=(
    -DVERIFAST
    -DVERIFAST_SKIP_BITVECTOR_PROOF__STACK_ALIGNMENT
    -I"$VF_DIR/bin"
    -I"$VF_PROOF_MOD_HEADER_DIR"
    -I"$VF_PROOF_MOD_SRC_DIR"
    -I"$PROOF_SETUP_DIR"
    -I"$PROOF_FILES_DIR" 
)

mkdir $LOG_PP_OUT_DIR

# Relevant clang flags:
# -E : Run preprocessor
# -C : Include comments in output
# -P : Surpresses line/file pragmas

echo start preprocessor
clang -E -C \
\
${BUILD_FLAGS[@]} \
${VERIFAST_FLAGS[@]} \
${RP2040_INLCUDE_FLAGS[@]} \
${PICO_INCLUDE_FLAGS[@]} \
-I"$REPO_BASE_DIR/include" \
\
-c "$SRC_FILE" \
1>"$OUT_FILE" 2>"$ERR_FILE"