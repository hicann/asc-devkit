# ----------------------------------------------------------------------------
# This program is free software, you can redistribute it and/or modify it.
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This file is a part of the CANN Open Software.
# Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------

add_custom_target(hostapi_headers
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include/ascendc/host_api/tiling
    COMMAND cp -rf ${ASCENDC_DIR}/include/utils/tiling/template_argument.h ${CMAKE_BINARY_DIR}/include/ascendc/host_api/tiling
 )

add_library(tikcfw_headers INTERFACE)
add_dependencies(tikcfw_headers hostapi_headers)
target_include_directories(tikcfw_headers INTERFACE
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/basic_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/basic_api/reg_compute/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/simt_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/c_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/simt_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/simt_api/cpp/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api/reg_compute/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/c_api/>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  $<INSTALL_INTERFACE:include/ascendc>
  $<INSTALL_INTERFACE:include/ascendc/include>
  $<INSTALL_INTERFACE:include/ascendc/include/basic_api>
  $<INSTALL_INTERFACE:include/asc/include>
  $<INSTALL_INTERFACE:include/asc/include/basic_api>
  $<INSTALL_INTERFACE:include/asc/include/simt_api>
  $<INSTALL_INTERFACE:include/asc/include/simt_api/cpp>
  $<INSTALL_INTERFACE:include/asc/include/basic_api/reg_compute>
  $<INSTALL_INTERFACE:include/asc/include/c_api>
  $<INSTALL_INTERFACE:include/asc/impl>
  $<INSTALL_INTERFACE:include/asc/impl/basic_api>
  $<INSTALL_INTERFACE:include/asc/impl/simt_api>
  $<INSTALL_INTERFACE:include/asc/impl/basic_api/reg_compute>
  $<INSTALL_INTERFACE:include/asc/impl/c_api>
)

install(FILES ${ASCENDC_INCLUDE_DIR}/kernel_operator.h
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

if(BUILD_OPEN_PROJECT)
  install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/basic_api/op_frame
    DESTINATION ${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api ${INSTALL_OPTIONAL} COMPONENT asc-devkit
  )
endif()

install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/basic_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)
install(DIRECTORY ${ASCENDC_IMPL_DIR}/basic_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/simt_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

install(DIRECTORY ${ASCENDC_IMPL_DIR}/simt_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl ${INSTALL_OPTIONAL} COMPONENT asc-devkit
)

install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils)" COMPONENT asc-devkit)
install(CODE "file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base)" COMPONENT asc-devkit)

# 1 asc/include/interface
install(CODE "file(CREATE_LINK ../basic_api/reg_compute \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/reg_compute SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../simt_api \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/simt_api SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../utils/debug \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/debug SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../utils/common_types.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/common_types.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../utils/base/sys_constants.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base/sys_constants.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../utils/base/sys_macros.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base/sys_macros.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/core_mng \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/core_mng SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_common.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_common.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_atomic_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_atomic_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_block_sync_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_block_sync_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_cache_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_cache_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_common_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_common_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_conv2d_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_conv2d_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_data_copy_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_data_copy_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_determine_compute_sync_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_determine_compute_sync_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_dump_tensor_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_dump_tensor_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_fixpipe_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_fixpipe_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_gemm_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_gemm_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_limits_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_limits_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_list_tensor_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_list_tensor_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_macro_bitmode_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_macro_bitmode_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_macro_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_macro_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_mm_bitmode_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_mm_bitmode_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_mm_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_mm_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_proposal_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_proposal_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_scalar_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_scalar_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_set_atomic_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_set_atomic_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_swap_mem_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_swap_mem_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_sys_var_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_sys_var_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_utils_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_utils_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_bilinearinterpolation_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_bilinearinterpolation_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_binary_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_binary_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_binary_scalar_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_binary_scalar_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_brcb_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_brcb_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_cmpsel_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_cmpsel_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_createvecindex_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_createvecindex_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_duplicate_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_duplicate_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_gather_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_gather_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_gather_mask_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_gather_mask_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_mulcast_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_mulcast_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_reduce_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_reduce_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_scatter_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_scatter_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_ternary_scalar_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_ternary_scalar_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_transpose_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_transpose_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_unary_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_unary_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_vconv_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_vconv_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_vpadding_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_vpadding_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_prof_trace_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_prof_trace_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_aipp.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_aipp.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_binary.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_binary.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_brcb.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_brcb.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_conv2d.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_conv2d.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_data_copy.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_data_copy.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_fixpipe.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_fixpipe.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_gather.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_gather.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_macro.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_macro.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_mm.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_mm.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_proposal.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_proposal.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_transpose.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_transpose.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_unary.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_unary.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_vdeq.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_vdeq.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_tensor.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_tensor.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_tpipe.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_tpipe.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_type.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_type.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_basic_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_basic_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_cube_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_cube_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_vec_intf.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_vec_intf.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../basic_api/kernel_base_types.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_base_types.h SYMBOLIC)" COMPONENT asc-devkit)

# 2 ascendc/include/basic_api
if(NOT BUILD_OPEN_PROJECT)
install(CODE "file(CREATE_LINK ../../../asc/include/basic_api/op_frame \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/op_frame SYMBOLIC)" COMPONENT asc-devkit)
endif()
install(CODE "file(CREATE_LINK ../../../asc/include/interface \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/interface SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../asc/include/kernel_operator.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/kernel_operator.h SYMBOLIC)" COMPONENT asc-devkit)

install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/reg_compute \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/reg_compute SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/simt_api \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/simt_api SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/common_types.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/common_types.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_c100 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_c100 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_c220 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_c220 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_3510 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_3510 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_l300 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_l300 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_l311 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_l311 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m200 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m200 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m300 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m300 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m310 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m310 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m510 \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m510 SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_check.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_check.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_check_util.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_check_util.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_event.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_event.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_log.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_log.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_macros.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_macros.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_atomic_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_atomic_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_common_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_common_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_conv2d_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_conv2d_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_conv2d_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_conv2d_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_coord.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_coord.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_data_copy_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_data_copy_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_data_copy_check.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_data_copy_check.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_data_copy_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_data_copy_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_determine_compute_sync_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_determine_compute_sync_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_dump_tensor_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_dump_tensor_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_fixpipe_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_fixpipe_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_gemm_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_gemm_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_gemm_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_gemm_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_layout.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_layout.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_list_tensor_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_list_tensor_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_macro_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_macro_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_macro_check.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_macro_check.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_macro_bitmode_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_macro_bitmode_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_macro_bitmode_struct.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_macro_bitmode_struct.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_macro_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_macro_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_check.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_check.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_bitmode_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_bitmode_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_bitmode_struct.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_bitmode_struct.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_load2d_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_load2d_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_proposal_base_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_proposal_base_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_proposal_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_proposal_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_scalar_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_scalar_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_set_atomic_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_set_atomic_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_symbol_override_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_symbol_override_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_sys_var_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_sys_var_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_tensor_trait.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_tensor_trait.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_bilinearinterpolation_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_bilinearinterpolation_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_binary_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_binary_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_binary_scalar_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_binary_scalar_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_brcb_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_brcb_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_cmpsel_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_cmpsel_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_createvecindex_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_createvecindex_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_duplicate_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_duplicate_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_gather_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_gather_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_gather_mask_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_gather_mask_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_mulcast_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_mulcast_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_reduce_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_reduce_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_scatter_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_scatter_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_ternary_scalar_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_ternary_scalar_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_transpose_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_transpose_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_unary_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_unary_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_vconv_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_vconv_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_vpadding_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_vpadding_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_pop_stack_buffer.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_pop_stack_buffer.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_process_lock.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_process_lock.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_prof_trace.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_prof_trace.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_prof_trace_intf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_prof_trace_intf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_reg.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_reg.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_scalar_convert.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_scalar_convert.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_scalar.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_scalar.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tbuf_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tbuf_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tbufpool_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tbufpool_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tensor_base.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tensor_base.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tensor_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tensor_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tpipe_base.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tpipe_base.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tpipe_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tpipe_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tquebind_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tquebind_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tquesync_impl.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tquesync_impl.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_type_conversion_utils.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_type_conversion_utils.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_utils.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_utils.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_utils_base.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_utils_base.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_npu_debug.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_npu_debug.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/utils \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/sys_macros.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/sys_macros.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/sys_constants.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/sys_constants.h SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/common_types.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/common_types.h SYMBOLIC)" COMPONENT asc-devkit)

# 3 tikcpp/tikcfw
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/impl \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/impl SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../asc/include/interface \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/interface SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/op_frame \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/op_frame SYMBOLIC)" COMPONENT asc-devkit)
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/kernel_operator.h \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/kernel_operator.h SYMBOLIC)" COMPONENT asc-devkit)

if(NOT BUILD_OPEN_PROJECT)
    install_package(
        PACKAGE tikcfw
        TARGETS tikcfw_headers
        DIRECTORY ${ASCENDC_DIR}/impl/basic_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl
        DIRECTORY ${ASCENDC_DIR}/impl/simt_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl
        DIRECTORY ${ASCENDC_DIR}/impl/c_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl
        DIRECTORY ${ASCENDC_DIR}/impl/tensor_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl

        DIRECTORY ${ASCENDC_DIR}/include/basic_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        DIRECTORY ${ASCENDC_DIR}/include/simt_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        FILES ${ASCENDC_DIR}/include/kernel_operator.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        DIRECTORY ${ASCENDC_DIR}/include/c_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        DIRECTORY ${ASCENDC_DIR}/include/tensor_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include

        FILES ${ASCENDC_DIR}/include/utils/tiling/platform/platform_ascendc.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling/platform
        FILES ${ASCENDC_DIR}/include/utils/context/context_builder.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling/context
        FILES ${ASCENDC_DIR}/include/utils/tiling/template_argument.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling
        FILES ${ASCENDC_DIR}/include/utils/log/asc_cpu_log.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/utils/log
        FILES ${ASCENDC_DIR}/include/aicpu_api/aicpu_api.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/aicpu_api
    )
endif()
