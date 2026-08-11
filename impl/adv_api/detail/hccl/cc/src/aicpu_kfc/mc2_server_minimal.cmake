# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

function(_mc2_server_get_include_list out_var)
    if(MC2_SERVER_CCL_INCLUDE_LIST)
        set(${out_var} ${MC2_SERVER_CCL_INCLUDE_LIST} PARENT_SCOPE)
    else()
        set(${out_var} ${CCL_KERNEL_INCLUDE_LIST} PARENT_SCOPE)
    endif()
endfunction()

function(_mc2_server_get_orion_head_list out_var)
    if(MC2_SERVER_ORION_HEAD_LIST)
        set(${out_var} ${MC2_SERVER_ORION_HEAD_LIST} PARENT_SCOPE)
    else()
        set(${out_var} ${ORION_HEAD_LIST} PARENT_SCOPE)
    endif()
endfunction()

function(_add_mc2_server_object target_name attach_to_mc2_server)
    if(NOT TARGET mc2_server)
        return()
    endif()

    _mc2_server_get_include_list(_mc2_server_include_list)
    _mc2_server_get_orion_head_list(_mc2_server_orion_head_list)

    add_library(${target_name} OBJECT ${ARGN})
    set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(${target_name} PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_DEFINITIONS>
        MC2_SERVER_ONLY
    )
    target_compile_options(${target_name} PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_OPTIONS>
    )
    target_include_directories(${target_name} PRIVATE
        ${_mc2_server_include_list}
        ${_mc2_server_orion_head_list}
    )
    target_link_libraries(${target_name} PRIVATE
        ascend_hal
        c_sec
        unified_dlog
        runtime_headers
        acl_rt_headers
    )
    if(TARGET json)
        target_link_libraries(${target_name} PRIVATE json)
        add_dependencies(${target_name} json)
    endif()
    if(attach_to_mc2_server)
        target_sources(mc2_server PRIVATE $<TARGET_OBJECTS:${target_name}>)
        add_dependencies(mc2_server ${target_name})
    endif()
endfunction()

function(add_mc2_server_object target_name)
    _add_mc2_server_object(${target_name} TRUE ${ARGN})
endfunction()

function(add_mc2_server_detached_object target_name)
    _add_mc2_server_object(${target_name} FALSE ${ARGN})
endfunction()

function(configure_mc2_server_minimal_target)
    if(NOT TARGET mc2_server)
        return()
    endif()

    _mc2_server_get_include_list(_mc2_server_include_list)
    _mc2_server_get_orion_head_list(_mc2_server_orion_head_list)

    if(NOT TARGET mc2_server_ccl_kernel_stub)
        set(MC2_SERVER_CCL_KERNEL_STUB_SRC
            ${CMAKE_CURRENT_BINARY_DIR}/mc2_server_ccl_kernel_stub.cc)
        file(WRITE ${MC2_SERVER_CCL_KERNEL_STUB_SRC}
            "extern \"C\" void __mc2_ccl_kernel_stub(void) {}\n")
        add_library(mc2_server_ccl_kernel_stub SHARED
            ${MC2_SERVER_CCL_KERNEL_STUB_SRC})
        set_target_properties(mc2_server_ccl_kernel_stub PROPERTIES
            OUTPUT_NAME ccl_kernel
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mc2_server_link_stub)
        target_link_options(mc2_server_ccl_kernel_stub PRIVATE
            -Wl,-soname,libccl_kernel.so)
    endif()

    if(NOT TARGET mc2_server_ccl_kernel_plf_stub)
        set(MC2_SERVER_CCL_KERNEL_PLF_STUB_SRC
            ${CMAKE_CURRENT_BINARY_DIR}/mc2_server_ccl_kernel_plf_stub.cc)
        file(WRITE ${MC2_SERVER_CCL_KERNEL_PLF_STUB_SRC}
            "extern \"C\" void __mc2_ccl_kernel_plf_stub(void) {}\n")
        add_library(mc2_server_ccl_kernel_plf_stub SHARED
            ${MC2_SERVER_CCL_KERNEL_PLF_STUB_SRC})
        set_target_properties(mc2_server_ccl_kernel_plf_stub PROPERTIES
            OUTPUT_NAME ccl_kernel_plf
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mc2_server_link_stub)
        target_link_options(mc2_server_ccl_kernel_plf_stub PRIVATE
            -Wl,-soname,libccl_kernel_plf.so)
    endif()

    target_compile_definitions(mc2_server PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_DEFINITIONS>
    )
    target_compile_options(mc2_server PRIVATE
        $<TARGET_PROPERTY:asc_ccl_kernel,INTERFACE_COMPILE_OPTIONS>
    )
    target_include_directories(mc2_server PRIVATE
        ${_mc2_server_include_list}
        ${_mc2_server_orion_head_list}
    )

    target_link_libraries(mc2_server PRIVATE
        -Wl,--no-as-needed
        ascend_hal
        c_sec
        unified_dlog
        mc2_server_ccl_kernel_stub
        ${HCCL_C_SEC_LINK}
        ${HCCL_UNIFIED_DLOG_LINK}
        mc2_server_ccl_kernel_plf_stub
        ${HCCL_MMPA_LINK}
        -Wl,--as-needed
        -lrt
        -ldl
        -lpthread
    )
    if(TARGET json)
        target_link_libraries(mc2_server PRIVATE json)
        add_dependencies(mc2_server json)
    endif()
endfunction()
