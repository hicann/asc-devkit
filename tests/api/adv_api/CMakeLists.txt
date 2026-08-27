# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
project(ascendc_adv_api_tests)
cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 17)
find_package(tikicpulib REQUIRED)

set(ASCENDC_ADV_API_INCLUDE ${ASCENDC_DIR}/include/adv_api)
set(ASCENDC_ADV_API_IMPL ${ASCENDC_DIR}/impl/adv_api)
set(ASCENDC_UTILS_INCLUDE ${ASCENDC_DIR}/include/utils)

set(PRODUCT_TYPE_LIST_INIT ascend610 ascend310p ascend910B1_AIC ascend910B1_AIV ascend950pr_9599_AIC ascend950pr_9599_AIV ascend310B1 KirinX90_AIV Kirin9030_AIV)
set(PRODUCT_TYPE_LIST ${PRODUCT_TYPE_LIST_INIT} CACHE STRING "Supported product types")
set(TILING_PRODUCT_TYPE_LIST ${PRODUCT_TYPE_LIST})
# The shared tiling cases assert server-side matmul parameters and are not valid for Kirin platforms.
list(REMOVE_ITEM TILING_PRODUCT_TYPE_LIST KirinX90_AIV Kirin9030_AIV)

set(ASCENDC_DEP_HEADER_DIR
    ${ASCEND_CANN_PACKAGE_PATH}/include
    ${ASCEND_CANN_PACKAGE_PATH}/include/base
    ${ASCEND_CANN_PACKAGE_PATH}/pkg_inc/base
    ${CMAKE_BINARY_DIR}
    ${ASCENDC_UTILS_INCLUDE}
)

set(ASCENDC_TEST_HEADER_FILES
    ../common
    matmul/utils
    ${ASCENDC_DIR}
    ${ASCENDC_DIR}/include
    ${ASCENDC_DIR}/include/adv_api
    ${ASCENDC_DIR}/include/basic_api
    ${ASCENDC_DIR}/impl
    ${ASCENDC_DIR}/impl/basic_api
)

# # ================ adv_api kernel ut ================
# ascend610 test cases
file(GLOB ASCENDC_TEST_ascend610_CASE_SRC_FILES
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    pad/broadcast/test_operator_broadcast_v200.cpp
    math/erf/test_operator_erf.cpp
    math/erfc/test_operator_erfc.cpp
    math/round/test_operator_round.cpp
    quantization/antiquant/test_ascend_quant.cpp
    quantization/antiquant/test_ascend_quant_per_channel.cpp
)

# ascend310p test cases
file(GLOB ASCENDC_TEST_ASCEND310P_CASE_SRC_PART_FILES
    utils/init_global_memory/test_operator_init_global_memory.cpp
    quantization/antiquant/test_ascend_antiquant_m200.cpp
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    pad/broadcast/test_operator_broadcast_v200.cpp
    math/erf/test_operator_erf.cpp
    math/erfc/test_operator_erfc.cpp
    math/round/test_operator_round.cpp
    quantization/antiquant/test_ascend_quant.cpp
    quantization/antiquant/test_ascend_quant_per_channel.cpp
    quantization/dequant/test_operator_dequant_v200.cpp
    math/exp/test_operator_exp.cpp
    matmul/test_matmul_config.cpp
    matmul/test_operator_matmul_v200.cpp
    matmul/copy_cube_in/test_copy_cube_in_mdl_310p.cpp
    matmul/copy_cube_in/test_copy_cube_in_norm_310p.cpp
    math/power/test_operator_power_v200.cpp
    reduce/reduce_xor_sum/test_operator_reduce_xor_sum.cpp
    normalization/welfordupdate/test_operator_welfordupdate.cpp
    normalization/welfordfinalize/test_operator_welfordfinalize.cpp
    normalization/normalize/test_operator_normalize.cpp
    normalization/layernormV2/test_operator_layernormV2.cpp
    matmul/scheduler/bias_scheduler/test_bias_scheduler_v200.cpp
    matmul/scheduler/batch_scheduler/test_batch_scheduler_v200.cpp
    matmul/scheduler/test_scheduler_norm.cpp
)
file(GLOB_RECURSE ASCENDC_TEST_ASCEND310P_CASE_SRC_FILES_API_CHECK api_check/math/*.cpp)
set(ASCENDC_TEST_ascend310p_CASE_SRC_FILES
    ${ASCENDC_TEST_ASCEND310P_CASE_SRC_PART_FILES}
    ${ASCENDC_TEST_ASCEND310P_CASE_SRC_FILES_API_CHECK}
)

# ascend910B1 aiv test cases
file(GLOB ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_PART_FILES
    activation/geglu/test_operator_geglu.cpp
    activation/gelu/test_operator_fast_gelu.cpp
    activation/gelu/test_operator_fast_gelu_v2.cpp
    activation/reglu/test_operator_reglu.cpp
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    activation/silu/test_operator_silu.cpp
    activation/softmax/test_operator_softmax_v220.cpp
    activation/softmax/test_operator_softmaxflashv3_v220.cpp
    activation/swiglu/test_operator_swiglu.cpp
    activation/swish/test_operator_swish.cpp
    index/arithprogression/test_operator_arithprogression.cpp
    pad/broadcast/test_operator_broadcast_v220.cpp
    filter/dropout/test_operator_dropout.cpp
    pad/pad/test_operator_pad.cpp
    select/selectwithbytesmask/test_operator_selectwithbytesmask.cpp
    transpose/confusion_transpose/test_operator_confusion_transpose.cpp
    math/acos/test_operator_acos.cpp
    math/acosh/test_operator_acosh.cpp
    math/asin/test_operator_asin.cpp
    math/asinh/test_operator_asinh.cpp
    math/atan/test_operator_atan.cpp
    math/atanh/test_operator_atanh.cpp
    math/axpy/test_operator_axpy.cpp
    math/ceil/test_operator_ceil.cpp
    math/clamp/test_operator_clamp.cpp
    math/cos/test_operator_cos.cpp
    math/cosh/test_operator_cosh.cpp
    math/cumsum/test_operator_cumsum.cpp
    math/digamma/test_operator_digamma.cpp
    math/erf/test_operator_erf.cpp
    math/erfc/test_operator_erfc.cpp
    math/exp/test_operator_exp.cpp
    math/exp/test_operator_exphighprecision.cpp
    math/floor/test_operator_floor.cpp
    math/frac/test_operator_frac.cpp
    # math/lgamma/test_operator_lgamma.cpp
    math/log/test_operator_log.cpp
    math/power/test_operator_power.cpp
    math/round/test_operator_round.cpp
    math/sign/test_operator_sign.cpp
    math/sin/test_operator_sin.cpp
    math/sinh/test_operator_sinh.cpp
    math/tan/test_operator_tan.cpp
    math/tanh/test_operator_tanh.cpp
    math/trunc/test_operator_trunc.cpp
    math/fmod/test_operator_fmod.cpp
    math/xor/test_operator_xor.cpp
    normalization/welfordfinalize/test_operator_welfordfinalize.cpp
    normalization/batchnorm/test_operator_batchnorm.cpp
    normalization/deepnorm/test_operator_deepnorm.cpp
    normalization/layernorm/test_operator_layernorm.cpp
    normalization/groupnorm/test_operator_groupnorm.cpp
    # normalization/layernorm/test_operator_layernormgrad.cpp
    normalization/layernorm/test_operator_layernormgradbeta.cpp
    normalization/normalize/test_operator_normalize.cpp
    normalization/layernormV2/test_operator_layernormV2.cpp
    normalization/rmsnorm/test_operator_rmsnorm.cpp
    normalization/welfordupdate/test_operator_welfordupdate.cpp
    quantization/antiquant/test_ascend_antiquant_scalar.cpp
    quantization/antiquant/test_ascend_antiquant_weight_scalar.cpp
    quantization/antiquant/test_ascend_antiquant_weight.cpp
    quantization/antiquant/test_ascend_antiquant.cpp
    quantization/dequant/test_operator_dequant.cpp
    quantization/quant/test_operator_quant.cpp
    quantization/quant/test_operator_quant_per_channel.cpp
    reduce/mean/test_operator_mean.cpp
    reduce/sum/test_operator_sum.cpp
    reduce/reduce_prod/test_operator_reduce_prod.cpp
    reduce/reduce_max/test_operator_reduce_max.cpp
    reduce/reduce_min/test_operator_reduce_min.cpp
    reduce/reduce_any/test_operator_reduce_any.cpp
    reduce/reduce_all/test_operator_reduce_all.cpp
    reduce/reduce_sum/test_operator_reduce_sum.cpp
    reduce/reduce_mean/test_operator_reduce_mean.cpp
    reduce/reduce_xor_sum/test_operator_reduce_xor_sum.cpp
    sort/topk/test_operator_topk.cpp
    utils/init_global_memory/test_operator_init_global_memory.cpp
    transpose/transdata/*cpp
)
file(GLOB_RECURSE ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_FILES_API_CHECK api_check/*.cpp)
set(ASCENDC_TEST_ascend910B1_AIV_CASE_SRC_FILES
    ${ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_PART_FILES}
    ${ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_FILES_API_CHECK}
)

# ascend910B1 aic test cases
file(GLOB ASCENDC_TEST_ascend910B1_AIC_CASE_SRC_FILES
    matmul/test_operator_matmul_v220.cpp
    matmul/test_operator_matmul_v220_batch.cpp
    matmul/test_matmul_l0db.cpp
    matmul/test_matmul_config.cpp
    matmul/test_matmul_channel_split.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_normal.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_single_buffer.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_double_buffer.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_single_global_buffer.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_double_global_buffer.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_n_buffer.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_bmm_db.cpp
    matmul/test_matmul_l0c_buffer.cpp
    matmul/test_matmul_shape_info.cpp
    matmul/test_matmul_shape_info_left.cpp
    hccl/test_hccl.cpp
    matmul/copy_cube_in/test_copy_cube_in_norm.cpp
    matmul/copy_cube_in/test_copy_cube_in_mdl.cpp
    matmul/copy_cube_in/test_copy_cube_in_params.cpp
    matmul/iterator/test_k_loop_norm.cpp
    matmul/iterator/test_k_loop_mdl.cpp
    matmul/iterator/test_k_loop_mdl_reorder.cpp
    matmul/iterator/test_k_loop_n_buffer.cpp
    matmul/iterator/test_m_loop_norm.cpp
    matmul/iterator/test_n_loop_norm.cpp
    matmul/iterator/test_m_loop_basic.cpp
    matmul/iterator/test_n_loop_basic.cpp
    matmul/iterator/test_m_loop_mdl.cpp
    matmul/iterator/test_n_loop_mdl.cpp
    matmul/iterator/test_m_loop_mdl_outer_product.cpp
    matmul/iterator/test_n_loop_mdl_outer_product.cpp
    matmul/iterator/test_m_loop_n_buffer.cpp
    matmul/iterator/test_batch_loop.cpp
    matmul/iterator/test_batch_loop_single.cpp
    matmul/iterator/test_batch_m_loop.cpp
    matmul/iterator/test_batch_n_loop.cpp
    matmul/iterator/test_matmul_iterator.cpp
    matmul/load_buffer/test_matmul_load_to_l0a.cpp
    matmul/load_buffer/test_matmul_load_to_l0b.cpp
    matmul/load_buffer/test_matmul_mmad_compute.cpp
    matmul/load_buffer/test_matmul_tbufpool_l0.cpp
    matmul/bias/test_bias_c1_buffer.cpp
    matmul/bias/test_bias_c2_buffer.cpp
    matmul/bias/test_copy_bias_in.cpp
    matmul/scheduler/test_scheduler_mdl.cpp
    matmul/scheduler/test_scheduler_special_mdl.cpp
    matmul/scheduler/test_scheduler_norm.cpp
    matmul/scheduler/test_scheduler_intrablock.cpp
    matmul/scheduler/test_scheduler_norm_outer_product.cpp
    matmul/scheduler/test_scheduler_mdl_outer_product.cpp
    matmul/scheduler/test_scheduler_n_buffer.cpp
    matmul/scheduler/test_scheduler_mdl_fullload.cpp
    matmul/scheduler/bias_scheduler/test_bias_scheduler_v220.cpp
    matmul/scheduler/batch_scheduler/test_batch_scheduler.cpp
    matmul/scheduler/batch_scheduler/test_batch_scheduler_single.cpp
    matmul/kfc/test_matmul_server_c220.cpp
)

# KirinX90 aiv test cases
file(GLOB ASCENDC_TEST_KirinX90_AIV_CASE_SRC_PART_FILES
    math/asin/test_operator_asin.cpp
    math/acos/test_operator_acos.cpp
    math/atan/test_operator_atan.cpp
    math/cos/test_operator_cos.cpp
    math/cosh/test_operator_cosh.cpp
    math/erf/test_operator_erf.cpp
    math/erfc/test_operator_erfc.cpp
    math/floor/test_operator_floor.cpp
    math/log/test_operator_log.cpp
    math/sign/test_operator_sign.cpp
    math/sin/test_operator_sin.cpp
    math/tan/test_operator_tan.cpp
    math/tanh/test_operator_tanh.cpp
    select/selectwithbytesmask/test_operator_selectwithbytesmask.cpp
    index/arithprogression/test_operator_arithprogression.cpp
    reduce/sum/test_operator_sum.cpp
    activation/gelu/test_operator_gelu.cpp
    activation/silu/test_operator_silu.cpp
    activation/reglu/test_operator_reglu.cpp
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    filter/dropout/test_operator_dropout.cpp
    pad/broadcast/test_operator_broadcast_v200.cpp
    matmul/test_matmul_shape_info.cpp
    matmul/test_matmul_shape_info_left.cpp
    matmul/test_matmul_l0c_buffer.cpp
    normalization/rmsnorm/test_operator_rmsnorm.cpp
    math/sinh/test_operator_sinh.cpp
    sort/sort/test_operator_radix_sort.cpp
    sort/topk/test_operator_radix_topk.cpp
    transpose/confusion_transpose/test_operator_confusion_transpose.cpp
    activation/geglu/test_operator_geglu.cpp
    activation/swiglu/test_operator_swiglu.cpp
    quantization/antiquant/test_ascend_antiquant.cpp
    quantization/dequant/test_ascend_dequant.cpp
    activation/gelu/test_operator_fast_gelu.cpp
    activation/softmax/test_operator_softmax_v220.cpp
)

set(ASCENDC_TEST_KirinX90_AIV_CASE_SRC_FILES
    ${ASCENDC_TEST_KirinX90_AIV_CASE_SRC_PART_FILES}
)

# Kirin9030 aiv test cases
file(GLOB ASCENDC_TEST_Kirin9030_AIV_CASE_SRC_PART_FILES
    math/asin/test_operator_asin.cpp
    math/acos/test_operator_acos.cpp
    math/atan/test_operator_atan.cpp
    math/cos/test_operator_cos.cpp
    math/cosh/test_operator_cosh.cpp
    math/erf/test_operator_erf.cpp
    math/erfc/test_operator_erfc.cpp
    math/floor/test_operator_floor.cpp
    math/log/test_operator_log.cpp
    math/sign/test_operator_sign.cpp
    math/sin/test_operator_sin.cpp
    math/tan/test_operator_tan.cpp
    math/tanh/test_operator_tanh.cpp
    select/selectwithbytesmask/test_operator_selectwithbytesmask.cpp
    index/arithprogression/test_operator_arithprogression.cpp
    reduce/sum/test_operator_sum.cpp
    activation/reglu/test_operator_reglu.cpp
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    filter/dropout/test_operator_dropout.cpp
    pad/broadcast/test_operator_broadcast_v200.cpp
    matmul/test_matmul_shape_info.cpp
    matmul/test_matmul_shape_info_left.cpp
    matmul/test_matmul_l0c_buffer.cpp
    normalization/rmsnorm/test_operator_rmsnorm.cpp
    math/sinh/test_operator_sinh.cpp
    sort/sort/test_operator_radix_sort.cpp
    sort/topk/test_operator_radix_topk.cpp
    transpose/confusion_transpose/test_operator_confusion_transpose.cpp
    activation/geglu/test_operator_geglu.cpp
    activation/swiglu/test_operator_swiglu.cpp
    quantization/antiquant/test_ascend_antiquant.cpp
    quantization/dequant/test_ascend_dequant.cpp
    activation/gelu/test_operator_fast_gelu.cpp
    activation/softmax/test_operator_softmax_v220.cpp
    activation/gelu/test_operator_gelu.cpp
    activation/silu/test_operator_silu.cpp
)

set(ASCENDC_TEST_Kirin9030_AIV_CASE_SRC_FILES
    ${ASCENDC_TEST_Kirin9030_AIV_CASE_SRC_PART_FILES}
)

# ascend950pr_9599 test cases
file(GLOB ASCENDC_TEST_ascend950pr_9599_AIC_CASE_SRC_FILES
    matmul/resource/cube_out_buffer/test_cube_out_buffer_co1_shared.cpp
    matmul/param/test_matmul_cross_core_sync.cpp
    matmul/kfc/test_matmul_server_c310.cpp
    matmul/copy_cube_in/test_copy_cube_in_ub_to_l1_ss.cpp
    matmul/tiling/test_matmul_constant_tiling_impl.cpp
    matmul/test_operator_matmul_mx_c310.cpp
    matmul/test_mx_matmul_impl.cpp
    matmul/utils/test_mx_matmul_utils.cpp
    matmul/scheduler/test_scheduler_norm_mx.cpp
    matmul/scheduler/test_scheduler_mdl_mx.cpp
    matmul/iterator/test_k_loop_mdl_mx.cpp
    matmul/copy_cube_in/test_copy_cube_in_norm_mx.cpp
    matmul/copy_cube_in/test_copy_cube_in_mdl_mx.cpp
    matmul/copy_cube_in/test_copy_cube_in_params_mx.cpp
    matmul/copy_cube_in/copy_tile_to_cube/test_copy_tile_to_cube_mx.cpp
    matmul/param/test_matmul_shape_tiling.cpp
    matmul/cube_in_buffer/test_cube_in_buffer_utils.cpp
    matmul/copy_cube_out/test_copy_cube_out_fixpipe.cpp
    matmul/copy_cube_out/test_quant_processor_fixpipe.cpp
    matmul/load_buffer/test_matmul_load_to_l0aV2.cpp
    matmul/load_buffer/test_matmul_load_to_l0bV2.cpp
    matmul/load_buffer/test_matmul_load_to_l0a_gemv_mx.cpp
    matmul/lib/test_matmul_client.cpp
)
file(GLOB ASCENDC_TEST_ascend950pr_9599_AIV_CASE_SRC_FILES
    #utils
    utils/init_global_memory/test_operator_init_global_memory.cpp
    #activate
    activation/gelu/test_operator_fast_gelu.cpp
    activation/gelu/test_operator_fast_gelu_v2.cpp
    activation/sigmoid/test_operator_vec_sigmoid.cpp
    activation/softmax/test_operator_softmax_c310.cpp
    api_check/activation/softmax/kernel_softmax_flashv3_check.cpp
    activation/gelu/test_operator_gelu.cpp
    activation/geglu/test_operator_geglu.cpp
    activation/reglu/test_operator_reglu.cpp
    activation/swish/test_operator_swish.cpp
    activation/silu/test_operator_silu.cpp
    activation/swiglu/test_operator_swiglu.cpp
    #filter
    filter/dropout/test_operator_dropout.cpp
    #index
    index/arithprogression/test_operator_arithprogression.cpp
    #math
    math/clamp/test_operator_clamp.cpp
    math/bitwise_and/test_operator_bitwise_and.cpp
    math/logical_and/test_operator_logical_and.cpp
    math/logical_ands/test_operator_logical_ands.cpp
    math/logical_or/test_operator_logical_or.cpp
    math/logical_ors/test_operator_logical_ors.cpp
    math/logical_not/test_operator_logical_not.cpp
    math/isnan/test_operator_isnan.cpp
    math/isinf/test_operator_isinf.cpp
    math/fma/test_operator_fma.cpp
    math/rint/test_operator_rint.cpp
    math/sincos/test_operator_sincos.cpp
    math/isfinite/test_operator_isfinite.cpp
    math/atanh/test_operator_atan_c310.cpp
    math/cos/test_operator_cos_c310.cpp
    math/cumsum/test_operator_cumsum.cpp
    math/erf/test_operator_erf.cpp
    math/fmod/test_operator_fmod.cpp
    math/hypot/test_operator_hypot.cpp
    math/log/test_operator_log.cpp
    math/philox/test_operator_philox_c310.cpp
    math/power/test_operator_power_c310.cpp
    math/sin/test_operator_sin_c310.cpp
    math/tanh/test_operator_tanh_c310.cpp
    math/cosh/test_operator_cosh.cpp
    math/sinh/test_operator_sinh.cpp
    math/sign/test_operator_sign.cpp
    math/exp/test_operator_exphighprecision.cpp
    math/where/test_operator_where.cpp
    api_check/math/where/kernel_where_check.cpp
    math/bitwise_not/test_operator_bitwise_not.cpp
    math/bitwise_or/test_operator_bitwise_or.cpp
    math/bitwise_xor/test_operator_bitwise_xor.cpp
    math/logical_xor/test_operator_logical_xor.cpp
    # normalization
    normalization/welfordfinalize/test_operator_welfordfinalize.cpp
    normalization/normalize/test_operator_normalize_c310.cpp
    normalization/layernorm/test_operator_layernorm.cpp
    normalization/layernormV2/test_operator_layernormV2_c310.cpp
    normalization/welfordupdate/test_operator_welfordupdate.cpp
    normalization/groupnorm/test_operator_groupnorm.cpp
    # quantization
    quantization/dequantize/test_dequantize.cpp
    quantization/antiquantize/test_antiquantize.cpp
    quantization/antiquant/test_ascend_antiquant_scalar.cpp
    quantization/antiquant/test_ascend_antiquant_weight_scalar.cpp
    quantization/antiquant/test_ascend_antiquant_weight_c310.cpp
    quantization/antiquant/test_ascend_antiquant.cpp
    quantization/dequant/test_ascend_quant_per_group.cpp
    quantization/dequant/test_ascend_quant_per_group_transpose.cpp
    quantization/dequant/test_ascend_quant_per_token.cpp
    quantization/dequant/test_ascend_quant_per_token_transpose.cpp
    quantization/dequant/test_ascend_dequant.cpp
    quantization/dequant/test_ascend_dequant_per_group.cpp
    quantization/dequant/test_ascend_dequant_per_token.cpp
    quantization/quant/test_ascend_quant_per_group_scalar.cpp
    quantization/quant/test_ascend_quant_per_group.cpp
    quantization/quant/test_ascend_quant_per_token_scalar.cpp
    quantization/quant/test_ascend_quant_per_token.cpp
    quantization/quantize/test_quantize_per_tensor.cpp
    quantization/quantize/test_quantize_per_channel.cpp
    quantization/quantize/test_quantize_per_token.cpp
    quantization/quantize/test_quantize_per_group.cpp
    # reduce
    reduce/reduce_max/test_operator_reduce_max_c310.cpp
    reduce/reduce_min/test_operator_reduce_min_c310.cpp
    reduce/reduce_sum/test_operator_reduce_sum_c310.cpp
    reduce/reduce_all/test_operator_reduce_all_c310.cpp
    reduce/reduce_any/test_operator_reduce_any_c310.cpp
    reduce/reduce_prod/test_operator_reduce_prod_c310.cpp
    api_check/reduce/reduce_all/kernel_reduce_all_check.cpp
    api_check/reduce/reduce_any/kernel_reduce_any_check.cpp
    api_check/reduce/reduce_prod/kernel_reduce_prod_check.cpp
    reduce/mean/test_operator_mean.cpp
    reduce/sum/test_operator_sum.cpp
    reduce/reduce_xor_sum/test_operator_reduce_xor_sum.cpp
    # pad
    pad/broadcast/test_operator_broadcast_c310.cpp
    api_check/pad/broadcast/kernel_broadcast_check.cpp
    pad/pad/test_operator_pad.cpp
    # select
    select/selectwithbytesmask/test_operator_selectwithbytesmask.cpp
    # sort
    sort/sort/test_operator_radix_sort.cpp
    sort/topk/test_operator_radix_topk.cpp
    # transpose
    transpose/confusion_transpose/test_operator_confusion_transpose_c310.cpp
    # hccl
    hccl/test_hccl_aicpu_950.cpp
)

# ascend310B1 test cases
file(GLOB ASCENDC_TEST_ascend310B1_CASE_SRC_FILES
    activation/softmax/test_operator_softmax_v300.cpp
    matmul/test_operator_matmul_v300.cpp
    math/xor/test_operator_xor.cpp
    math/floor/test_operator_floor.cpp
    matmul/scheduler/test_scheduler_norm.cpp
)

foreach(product_type ${PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_adv_api_kernel_${product_type}
        ${ASCENDC_DIR}/tests/main_global.cpp
        $<$<STREQUAL:${product_type},ascend610>:../common/tik_pv_wrapper.cpp>
        $<$<STREQUAL:${product_type},ascend310p>:../common/tik_pv_wrapper.cpp>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},KirinX90_AIV>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},Kirin9030_AIV>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:../common/k3_pvwrap.cpp>
        $<$<STREQUAL:${product_type},ascend310B1>:../common/k3_pvwrap.cpp>
        ${ASCENDC_TEST_${product_type}_CASE_SRC_FILES}
    )

    target_compile_definitions(ascendc_ut_adv_api_kernel_${product_type} PRIVATE
        UT_TEST
        ASCENDC_OOM=1
        ASCENDC_DUMP=0
        $<$<STREQUAL:${product_type},ascend310p>:__NPU_ARCH__=2002;__DAV_M200__>
        $<$<STREQUAL:${product_type},ascend610>:__NPU_ARCH__=2002;__DAV_M200__>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_CUBE__;__DAV_CUBE__>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:__NPU_ARCH__=2201;__DAV_C220__;__DAV_C220_VEC__;__DAV_VEC__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_CUBE__>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:__NPU_ARCH__=3510;__DAV_C310__;__DAV_C310_VEC__>
        $<$<STREQUAL:${product_type},KirinX90_AIV>:__NPU_ARCH__=3003;__DAV_L300__;__DAV_L300_VEC__;__DAV_L300_VEC__>
        $<$<STREQUAL:${product_type},Kirin9030_AIV>:__NPU_ARCH__=3113;__DAV_L311__;__DAV_L311_VEC__;__DAV_L311_VEC__>
        $<$<STREQUAL:${product_type},ascend310B1>:__NPU_ARCH__=3002;__DAV_M300__>
        __disable_kernel_type_autoinfer__=
        __callee__=
    )

    target_include_directories(ascendc_ut_adv_api_kernel_${product_type} PRIVATE
        ${ASCENDC_TEST_HEADER_FILES}
        ${ASCENDC_DEP_HEADER_DIR}
    )

    target_compile_options(ascendc_ut_adv_api_kernel_${product_type} PRIVATE
        -fno-access-control
        -Werror
        -UASCENDC_DEBUG
    )

    target_link_libraries(ascendc_ut_adv_api_kernel_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        kernel_tiling
        $<$<STREQUAL:${product_type},ascend310p>:tikicpulib::ascend310p>
        $<$<STREQUAL:${product_type},ascend610>:tikicpulib::ascend610>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:tikicpulib::ascend910B1>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:tikicpulib::ascend910B1>
 	    $<$<STREQUAL:${product_type},KirinX90_AIV>:tikicpulib::KirinX90>
 	    $<$<STREQUAL:${product_type},Kirin9030_AIV>:tikicpulib::Kirin9030>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:tikicpulib::ascend950pr_9599>
        $<$<STREQUAL:${product_type},ascend310B1>:tikicpulib::ascend310B1>
        -Wl,--no-as-needed
        error_manager
        mmpa
        c_sec
        -Wl,--as-needed
    )

    run_llt_test(
        TARGET ascendc_ut_adv_api_kernel_${product_type}
        TASK_NUM 1
    )
endforeach()

# ================ adv_api tiling ut ================
file(GLOB ASCENDC_TILING_TEST_SRC_FILES
    tiling/test_hccl_tiling.cpp
    tiling/test_matmul_api_tiling.cpp
    tiling/test_tiling_v310.cpp
    tiling/test_tiling.cpp
    tiling/conv/*.cpp
    tiling/conv_backprop/*.cpp
)

file(GLOB ASCENDC_TILING_SRC_FILES
    ${ASCENDC_ADV_API_IMPL}/tiling/activation/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/filter/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/index/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/math/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/matmul/*cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/hccl/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/normalization/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/pad/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/quantization/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/reduce/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/select/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/sort/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/transpose/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/conv/*.cpp
    ${ASCENDC_ADV_API_IMPL}/tiling/conv_backprop/*.cpp
    ${ASCENDC_ADV_API_IMPL}/detail/host_log.cpp
)

# ascendc_tiling_utest
foreach(product_type ${TILING_PRODUCT_TYPE_LIST})
    add_executable(ascendc_ut_adv_api_tiling_${product_type}
        ${ASCENDC_DIR}/tests/main.cpp
        ../common/platform_stub.cpp
        ../common/ascendc_platform_stub.cpp
        ${ASCENDC_TILING_SRC_FILES}
        ${ASCENDC_TILING_TEST_SRC_FILES}
    )

    target_compile_definitions(ascendc_ut_adv_api_tiling_${product_type} PRIVATE
        UT_TEST
        $<$<STREQUAL:${product_type},ascend310p>:__NPU_ARCH__=2002>
        $<$<STREQUAL:${product_type},ascend610>:__NPU_ARCH__=2002>
        $<$<STREQUAL:${product_type},ascend910B1_AIC>:__NPU_ARCH__=2201>
        $<$<STREQUAL:${product_type},ascend910B1_AIV>:__NPU_ARCH__=2201>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIC>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend950pr_9599_AIV>:__NPU_ARCH__=3510>
        $<$<STREQUAL:${product_type},ascend310B1>:__NPU_ARCH__=3002>
 	    $<$<STREQUAL:${product_type},KirinX90_AIV>:__NPU_ARCH__=3003>
 	    $<$<STREQUAL:${product_type},Kirin9030_AIV>:__NPU_ARCH__=3113>
    )

    target_include_directories(ascendc_ut_adv_api_tiling_${product_type} PRIVATE
        ../common/
        ${ASCENDC_TEST_HEADER_FILES}
        ${ASCENDC_DEP_HEADER_DIR}
        ${ASCENDC_TEST_${product_type}_HEADER_FILES}
        ${ASCENDC_ADV_API_INCLUDE}/
        ${ASCENDC_ADV_API_INCLUDE}/activation/
        ${ASCENDC_ADV_API_INCLUDE}/filter/
        ${ASCENDC_ADV_API_INCLUDE}/index/
        ${ASCENDC_ADV_API_INCLUDE}/math/
        ${ASCENDC_ADV_API_INCLUDE}/matmul/
        ${ASCENDC_ADV_API_INCLUDE}/normalization/
        ${ASCENDC_ADV_API_INCLUDE}/pad/
        ${ASCENDC_ADV_API_INCLUDE}/quantization/
        ${ASCENDC_ADV_API_INCLUDE}/reduce/
        ${ASCENDC_ADV_API_INCLUDE}/select/
        ${ASCENDC_ADV_API_INCLUDE}/sort/
        ${ASCENDC_ADV_API_INCLUDE}/transpose/
        ${ASCENDC_ADV_API_INCLUDE}/conv/
        ${ASCENDC_ADV_API_INCLUDE}/conv_backprop/
        ${ASCENDC_ADV_API_INCLUDE}/conv/conv3d/
    )
    target_link_directories(ascendc_ut_adv_api_tiling_${product_type} PRIVATE
        ${ASCEND_CANN_PACKAGE_PATH}/lib64
    )

    target_link_libraries(ascendc_ut_adv_api_tiling_${product_type} PRIVATE
        $<BUILD_INTERFACE:intf_llt_pub_basic>
        kernel_tiling
        register
        platform
        graph
        graph_base
        unified_dlog
        ascend_protobuf
        error_manager
        mmpa
        c_sec
        dl
    )

    run_llt_test(
        TARGET ascendc_ut_adv_api_tiling_${product_type}
        TASK_NUM 1
    )
endforeach()

add_subdirectory(hccl/cc)
add_subdirectory(hccl/ccu)

add_subdirectory(api_host_check)
