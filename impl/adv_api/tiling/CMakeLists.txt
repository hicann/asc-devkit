# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16.0)
project(ascendc_tiling_api)

add_library(tiling_api_headers INTERFACE)
target_include_directories(
  tiling_api_headers
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${ASCENDC_DIR}>
    $<BUILD_INTERFACE:${ASCENDC_DIR}/include>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/include/adv_api>>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api>>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/../../asc/asc-devkit>>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/../../asc/asc-devkit/include>>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/../../asc/asc-devkit/include/utils>>
    $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${ASCENDC_DIR}/../../asc/asc-devkit/include/utils/tiling>>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/ascendc>
    $<INSTALL_INTERFACE:include/ascendc/include/>
    $<INSTALL_INTERFACE:include/ascendc/include/highlevel_api>
    $<INSTALL_INTERFACE:include/ascendc/include/highlevel_api/tiling>)

target_link_libraries(tiling_api_headers
                      INTERFACE
                      $<BUILD_INTERFACE:metadef_headers>
                      $<BUILD_INTERFACE:unified_dlog>
                      $<BUILD_INTERFACE:tikcfw_headers>)

add_library(
  tiling_api STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/ascend_dequant_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/ascend_quant_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/ascend_antiquant_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/quantize_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/dequantize_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/quantization/antiquantize_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/filter/dropout_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/gelu_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/matmul/bmm_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/matmul/matmul_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/matmul/matmul_tiling_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/matmul/matmul_tiling_algorithm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/matmul/math_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/hccl/hccl_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/bitwise_and_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_and_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_ands_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_not_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_or_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_ors_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/fma_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/sincos_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/rint_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/clamp_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/acos_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/acosh_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/asin_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/asinh_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/atan_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/atanh_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/is_inf_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/is_nan_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/cos_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/cosh_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/erf_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/erfc_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/exp_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/frac_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/geglu_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/lgamma_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/digamma_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/log_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/sin_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/sinh_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/power_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/sigmoid_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/round_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/tan_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/tanh_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/trunc_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/axpy_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/hypot_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/bitwise_not_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/bitwise_or_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/bitwise_xor_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/logical_xor_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/swiglu_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/ceil_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/floor_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/softmax_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/logsoftmax_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/rmsnorm_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/batchnorm_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sort/sort_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sort/topk_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/deepnorm_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/select/selectwithbytesmask_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/layernorm_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/normalize_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/layernorm_grad_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/layernorm_grad_beta_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/groupnorm_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/normalization/welfordfinalize_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/pad/pad_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/transpose/confusion_transpose_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/pad/broadcast_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/pad/broadcast_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/xor_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/cumsum_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/where_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reduce/mean_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/sign_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/reglu_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/silu_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/activation/swish_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reduce/reduce_xor_sum_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reduce/sum_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reduce/reduce_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/transpose/transdata_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/index/arithprogression_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/math/fmod_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv/conv3d_tiling_algorithm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv/conv3d_tiling_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv/conv3d_tiling_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv/conv3d_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv_backprop/conv3d_bp_filter_tiling_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv_backprop/conv3d_bp_filter_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv_backprop/conv3d_bp_input_tiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conv_backprop/conv3d_bp_input_tiling_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/experimental/math/bessel_i0_tiling_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../detail/host_log.cpp
  ${ASCENDC_DIR}/impl/utils/tiling/platform/platform_ascendc.cpp
  ${ASCENDC_DIR}/impl/utils/tiling/template_argument.cpp
  $<$<STREQUAL:${PRODUCT_SIDE},host>:${ASCENDC_DIR}/impl/utils/context/context_builder.cpp>
  $<$<STREQUAL:${PRODUCT_SIDE},host>:${ASCENDC_DIR}/impl/utils/context/context_builder_impl.cpp>
  $<$<STREQUAL:${PRODUCT_SIDE},device>:${ASCENDC_DIR}/impl/utils/stub/shape.cpp>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<BOOL:${BUILD_OPEN_PROJECT}>>:$<TARGET_OBJECTS:tilingdata_base_objs>>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>>:${TOP_DIR}/metadef/base/asc/tilingdata_base_impl.cc>
  )

target_include_directories(tiling_api PRIVATE
  ${KERNEL_TILING_DIR}
  ${ASCENDC_DIR}/include
  ${ASCENDC_DIR}/include/utils/context
  ${ASCENDC_DIR}/include/utils/tiling/platform
  ${ASCENDC_DIR}/include/utils
  ${TOP_DIR}/runtime/pkg_inc
  $<$<STREQUAL:${PRODUCT_SIDE},device>:$<BUILD_INTERFACE:${ASCENDC_DIR}/include/utils/stub>>
  $<$<STREQUAL:${PRODUCT_SIDE},device>:$<BUILD_INTERFACE:${ASCENDC_DIR}/include/utils/stub/common>>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>>:$<BUILD_INTERFACE:${TOP_DIR}/metadef/inc/external>>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>>:$<BUILD_INTERFACE:${TOP_DIR}/metadef/inc>>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>>:$<BUILD_INTERFACE:${TOP_DIR}/ace/npuruntime/runtime/platform/inc>>
  $<$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>:$<BUILD_INTERFACE:${TOP_DIR}/ace/npuruntime/runtime/dfx/include/log/toolchain>>
  $<$<AND:$<STREQUAL:${PRODUCT_SIDE},device>,$<NOT:$<BOOL:${BUILD_OPEN_PROJECT}>>>:$<BUILD_INTERFACE:${TOP_DIR}/metadef/pkg_inc>>
)

target_compile_options(
  tiling_api
  PRIVATE -fvisibility=hidden
          -Wextra
          -Wfloat-equal
          -D_FORTIFY_SOURCE=2
          -O2
          $<$<STREQUAL:${PRODUCT_SIDE},device>:-fPIC>
          $<$<STREQUAL:${PRODUCT_SIDE},device>:-fstack-protector-strong>
          $<$<STREQUAL:${PRODUCT_SIDE},device>:-fvisibility-inlines-hidden>
          $<$<STREQUAL:${PRODUCT_SIDE},device>:-DASCEND_IS_AICPU>
          $<$<STREQUAL:${PRODUCT_SIDE},device>:-fvisibility=hidden>)

target_compile_definitions(
  tiling_api PRIVATE
  $<$<STREQUAL:${PRODUCT_SIDE},device>:_FORTIFY_SOURCE=2>
  $<$<STREQUAL:${PRODUCT_SIDE},device>:ASCENDC_DEVICE_REG_STATIC>
)

target_link_libraries(
  tiling_api
  PRIVATE $<BUILD_INTERFACE:intf_pub_cxx14>
          $<BUILD_INTERFACE:slog_headers>
          -Wl,--no-as-needed
          c_sec
          -Wl,--as-needed
          $<$<STREQUAL:${PRODUCT_SIDE},host>:platform>
          $<$<STREQUAL:${PRODUCT_SIDE},host>:exe_graph>
          $<$<STREQUAL:${PRODUCT_SIDE},host>:register>
  PUBLIC $<BUILD_INTERFACE:kernel_tiling_headers>
         $<BUILD_INTERFACE:metadef_headers> tiling_api_headers
         )

if(BUILD_OPEN_PROJECT)
  set(ASCENDC_API_ADV_INSTALL_DIR ${INSTALL_LIBRARY_DIR}/lib64)
  set(ASCENDC_API_ADV_MULT_INSTALL_DIR
      ${INSTALL_LIBRARY_DIR}/devlib/linux/${CMAKE_SYSTEM_PROCESSOR})
  set(ASCENDC_TILING_INCLUDE_INSTALL_DIR ${INSTALL_LIBRARY_DIR}/include)
else()
  set(ASCENDC_API_ADV_INSTALL_DIR ${INSTALL_LIBRARY_DIR}/tiling/)
  set(ASCENDC_API_ADV_MULT_INSTALL_DIR
      ${INSTALL_LIBRARY_DIR}/${CMAKE_SYSTEM_PROCESSOR})
  set(ASCENDC_TILING_INCLUDE_INSTALL_DIR ${INSTALL_LIBRARY_DIR}/asc/include)
endif()

install(TARGETS tiling_api ARCHIVE DESTINATION ${ASCENDC_API_ADV_INSTALL_DIR}
                                   ${INSTALL_OPTIONAL} COMPONENT asc-devkit)

if(NOT "${PRODUCT_SIDE}" STREQUAL "device")
        install(TARGETS tiling_api
                ARCHIVE DESTINATION ${ASCENDC_API_ADV_MULT_INSTALL_DIR} ${INSTALL_OPTIONAL} COMPONENT asc-devkit)
        configure_file(${ASCENDC_ADV_API_CMAKE_DIR}/tiling_headers.cmake
                       ${CMAKE_CURRENT_BINARY_DIR}/tiling_headers.cmake @ONLY)
        install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/tiling_headers.cmake COMPONENT asc-devkit)
endif()
