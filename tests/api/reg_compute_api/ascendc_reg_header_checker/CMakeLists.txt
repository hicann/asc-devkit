cmake_minimum_required(VERSION 3.16)
project(HeaderChecker)

set(INCLUDE_PATHS ${ASCENDC_DIR})

if (TARGET tikcfw_headers)
    message(STATUS "tikcfw_headers target exists, gathering its include dirs")
    get_target_property(TIKCFW_HEADERS_INCLUDE_DIRS tikcfw_headers INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "tikcfw_headers include dirs: ${TIKCFW_HEADERS_INCLUDE_DIRS}")
    if (TIKCFW_HEADERS_INCLUDE_DIRS)
        foreach(DIR ${TIKCFW_HEADERS_INCLUDE_DIRS})
            if(DIR MATCHES "^\\$<INSTALL_INTERFACE:.*>$")
                continue()
            endif()
            if(DIR MATCHES "^\\$<BUILD_INTERFACE:(.*)>$")
                list(APPEND INCLUDE_PATHS "${CMAKE_MATCH_1}")
            else()
                list(APPEND INCLUDE_PATHS "${DIR}")
            endif()
        endforeach()
    endif()
else()
    message(WARNING "tikcfw_headers target does not exist")
endif()

find_package(tikicpulib REQUIRED)
if(tikicpulib_FOUND)
    message(STATUS "tikicpulib found, adding include directories")
    message(STATUS "tikicpulib cmake path: ${tikicpulib_DIR}")
    get_target_property(TIKICPULIB_INCLUDE_DIRS tikicpulib_ascend910 INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "tikicpulib include dirs: ${TIKICPULIB_INCLUDE_DIRS}")
    if(TIKICPULIB_INCLUDE_DIRS)
        list(APPEND INCLUDE_PATHS ${TIKICPULIB_INCLUDE_DIRS})
    endif()
endif()

# ==========================================
# 编译配置 (CONFIGS)
# ==========================================
# 格式: "ConfigName|Arch|Define1|Define2|..."
set(CONFIGS_LIST
    "dav-c310-vec|3510|-D__DAV_C310__|-D__DAV_C310_VEC__|-DASCENDC_DEBUG=1|-DASCENDC_DUMP=1"
    "dav-c310-cube|3510|-D__DAV_C310__|-D__DAV_C310_CUBE__|-DASCENDC_DEBUG=1|-DASCENDC_DUMP=1"
    "dav-510r2|5102|-DASCENDC_DEBUG=1|-DASCENDC_DUMP=1"
)

# ==========================================
# 测试文件
# ==========================================

set(ALL_TEST_FILES "")

file(GLOB_RECURSE ALL_TEST_FILES
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT ALL_TEST_FILES)
    message(FATAL_ERROR "No test files found to test.")
endif()

list(REMOVE_DUPLICATES ALL_TEST_FILES)
list(LENGTH ALL_TEST_FILES LEN)
message(STATUS "Total test files to test: ${LEN}")

# ==========================================
# 构建逻辑
# ==========================================

# 生成 Include 参数
set(INC_FLAGS "")
list(REMOVE_DUPLICATES INCLUDE_PATHS)
list(FILTER INCLUDE_PATHS EXCLUDE REGEX "^$")
foreach(INC ${INCLUDE_PATHS})
    list(APPEND INC_FLAGS "-I${INC}")
endforeach()
message(STATUS "INC_FLAGS: ${INC_FLAGS}")

# 函数: 为指定模式生成检查 targets
# 参数: NPU_MODE - "ON" 或 "OFF"
#       MODE_SUFFIX - 模式后缀 (如 "" 或 "_single")
#       MODE_DEFINES - 额外的宏定义列表
function(create_check_targets NPU_MODE MODE_SUFFIX MODE_DEFINES)
    set(TARGET_VAR "CHECK_TARGETS_${NPU_MODE}${MODE_SUFFIX}")
    set(${TARGET_VAR})

    # 根据 NPU 模式设置编译器和参数
    if(NPU_MODE STREQUAL "ON")
        set(COMPILER "bisheng")
        set(COMPILER_FLAGS -x cce -std=c++17 --cce-aicore-only -w)
        set(COMPILER_TYPE "NPU")
    else()
        set(COMPILER "g++")
        set(COMPILER_FLAGS -x c++ -std=c++17 -w)
        set(COMPILER_TYPE "CPU")
    endif()

    message(STATUS "Creating ${COMPILER_TYPE}${MODE_SUFFIX} targets")

    # 遍历所有文件和所有配置
    foreach(TEST_FILE ${ALL_TEST_FILES})
        # 获取相对路径用于生成唯一的 Target 名字
        file(RELATIVE_PATH REL_TEST_FILE ${ASCENDC_DIR} ${TEST_FILE})

        # 清理文件名中的特殊字符
        string(REPLACE "/" "_" TEST_FILE_ID ${REL_TEST_FILE})
        string(REPLACE "." "_" TEST_FILE_ID ${TEST_FILE_ID})
        string(REPLACE "-" "_" TEST_FILE_ID ${TEST_FILE_ID})

        # 使用索引遍历，因为包含分号的字符串会被 foreach 自动分割
        list(LENGTH CONFIGS_LIST CONFIGS_COUNT)
        math(EXPR CONFIGS_LAST "${CONFIGS_COUNT} - 1")
        foreach(IDX RANGE ${CONFIGS_LAST})
            list(GET CONFIGS_LIST ${IDX} CONFIG_STR)

            # 解析配置字符串: "name|arch|def1|def2"
            # 将管道符替换为空格，然后用 separate_arguments 转成列表
            string(REPLACE "|" " " CONFIG_STR_SPACE "${CONFIG_STR}")
            separate_arguments(CONFIG_ITEMS UNIX_COMMAND "${CONFIG_STR_SPACE}")

            list(GET CONFIG_ITEMS 0 CONFIG_NAME)

            list(LENGTH CONFIG_ITEMS ITEMS_LEN)
            if(ITEMS_LEN GREATER 1)
                list(GET CONFIG_ITEMS 1 CONFIG_ARCH)
            else()
                set(CONFIG_ARCH "")
            endif()

            if(ITEMS_LEN GREATER 2)
                list(SUBLIST CONFIG_ITEMS 2 -1 CONFIG_DEFS)
            else()
                set(CONFIG_DEFS "")
            endif()

            # 合并所有定义 (NPU 使用 --cce-aicore-arch=CONFIG_NAME, CPU 使用 -D__NPU_ARCH__=CONFIG_ARCH)
            if(NPU_MODE STREQUAL "ON")
                set(ALL_DEFINES ${MODE_DEFINES} ${CONFIG_DEFS})
                set(ARCH_FLAG "--cce-aicore-arch=${CONFIG_NAME}")
            else()
                set(ALL_DEFINES ${MODE_DEFINES} ${CONFIG_DEFS} "-D__NPU_ARCH__=${CONFIG_ARCH}" "-DASCENDC_CPU_DEBUG=1" "-D__callee__=")
                set(ARCH_FLAG "")
            endif()

            # 创建子 Target (NPU 和 CPU 使用不同的前缀)
            if(NPU_MODE STREQUAL "ON")
                set(TARGET_NAME "check_npu${MODE_SUFFIX}_${CONFIG_NAME}_${TEST_FILE_ID}")
            else()
                set(TARGET_NAME "check_cpu${MODE_SUFFIX}_${CONFIG_NAME}_${TEST_FILE_ID}")
            endif()

            add_custom_target(${TARGET_NAME}
                COMMAND ${COMPILER} ${COMPILER_FLAGS} ${INC_FLAGS} ${ALL_DEFINES}
                    -fsyntax-only ${TEST_FILE} ${ARCH_FLAG}
                COMMENT "Checking (${COMPILER_TYPE})${MODE_SUFFIX} ${CONFIG_NAME} ${REL_TEST_FILE}"
                VERBATIM
                USES_TERMINAL
            )

            list(APPEND ${TARGET_VAR} ${TARGET_NAME})
        endforeach()
    endforeach()

    # 创建汇总 Target
    if(NPU_MODE STREQUAL "ON")
        set(NPU_PREFIX "_npu")
    else()
        set(NPU_PREFIX "_cpu")
    endif()
    set(RUN_ALL_TARGET "ascendc_run_all_reg_header_checks${NPU_PREFIX}${MODE_SUFFIX}")

    if(MODE_SUFFIX STREQUAL "")
        set(MODE_DESC "normal")
    else()
        string(REPLACE "_" " " MODE_DESC "${MODE_SUFFIX}")
        string(STRIP "${MODE_DESC}" MODE_DESC)
    endif()
    add_custom_target(${RUN_ALL_TARGET}
        DEPENDS ${${TARGET_VAR}}
        COMMENT "Running all header checks (${COMPILER_TYPE} ${MODE_DESC} mode)..."
    )
endfunction()

# 模式 1: CPU 普通模式
set(NORMAL_DEFINES)
create_check_targets("OFF" "" "${NORMAL_DEFINES}")

# 模式 2: CPU 单一头文件模式
set(SINGLE_DEFINES -DVERIFY_SINGLE_HEADER)
create_check_targets("OFF" "_single" "${SINGLE_DEFINES}")

# 模式 3: NPU 普通模式
create_check_targets("ON" "" "${NORMAL_DEFINES}")

# 模式 4: NPU 单一头文件模式
create_check_targets("ON" "_single" "${SINGLE_DEFINES}")

add_custom_target(ascendc_run_all_reg_header_checks ALL
    DEPENDS
        ascendc_run_all_reg_header_checks_cpu
        ascendc_run_all_reg_header_checks_cpu_single
        ascendc_run_all_reg_header_checks_npu
        ascendc_run_all_reg_header_checks_npu_single
    COMMENT "Running all header checks (all modes, all architectures)..."
)
