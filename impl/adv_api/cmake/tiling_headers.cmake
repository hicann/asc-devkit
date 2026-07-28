# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set(ASCENDC_API_PATH @INSTALL_LIBRARY_DIR@)
set(ASCENDC_DIR @ASCENDC_DIR@)
set(ASCENDC_INSTALL_BASE_PATH $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${ASCENDC_API_PATH})

file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/ascendc/include/highlevel_api/kernel_tiling)
file(
    CREATE_LINK ../../../../asc/include/adv_api/kernel_tiling.h
     ${ASCENDC_INSTALL_BASE_PATH}/ascendc/include/highlevel_api/kernel_tiling/kernel_tiling.h
     SYMBOLIC)

file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling)
file(
  CREATE_LINK ../adv_api/utils
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/utils
  SYMBOLIC)

file(
  CREATE_LINK ../adv_api/activation
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/activation
  SYMBOLIC)
file(
  CREATE_LINK ../adv_api/activation
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/activation
  SYMBOLIC)
file(CREATE_LINK ../adv_api/filter
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/filter
     SYMBOLIC)
file(CREATE_LINK ../adv_api/index
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/index
     SYMBOLIC)
file(CREATE_LINK ../adv_api/math
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/math
     SYMBOLIC)
file(CREATE_LINK ../adv_api/matmul
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matmul
     SYMBOLIC)
file(CREATE_LINK ../adv_api/normalization
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/normalization
     SYMBOLIC)
file(CREATE_LINK ../adv_api/pad
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/pad
     SYMBOLIC)
file(CREATE_LINK ../adv_api/quantization
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/quantization
     SYMBOLIC)
file(CREATE_LINK ../adv_api/pad
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/pad
     SYMBOLIC)
file(CREATE_LINK ../adv_api/quantization
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/quantization
     SYMBOLIC)
file(CREATE_LINK ../adv_api/reduce
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reduce
     SYMBOLIC)
file(CREATE_LINK ../adv_api/select
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/select
     SYMBOLIC)
file(CREATE_LINK ../adv_api/sort
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sort
     SYMBOLIC)
file(CREATE_LINK ../adv_api/transpose
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/transpose
     SYMBOLIC)

# arithprogression
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/arithprogression
)
file(
  CREATE_LINK
  ../../adv_api/index/arithprogression_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/arithprogression/arithprogression_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../adv_api/index/arithprogression_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/arithprogression_tiling_intf.h
  SYMBOLIC)

# ascend_antiquant
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_antiquant
)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_antiquant_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_antiquant/ascend_antiquant_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_antiquant_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_antiquant/ascend_antiquant_tiling_intf.h
  SYMBOLIC)

# ascend_dequant
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_dequant
)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_dequant_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_dequant/ascend_dequant_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_dequant_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_dequant/ascend_dequant_tiling_intf.h
  SYMBOLIC)

# ascend_quant
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_quant
)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_quant_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_quant/ascend_quant_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/quantization/ascend_quant_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/ascend_quant/ascend_quant_tiling_intf.h
  SYMBOLIC)

# batchnorm
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/batchnorm)
file(
  CREATE_LINK
  ../../adv_api/normalization/batchnorm_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/batchnorm/batchnorm_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/batchnorm_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/batchnorm/batchnorm_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/batchnorm_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/batchnorm/batchnorm_tilingdata.h
  SYMBOLIC)

# broadcast
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/broadcast)
file(
  CREATE_LINK
  ../../adv_api/pad/broadcast_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/broadcast/broadcast_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/pad/broadcast_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/broadcast/broadcast_tiling.h
  SYMBOLIC)

# deepnorm
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/deepnorm)
file(
  CREATE_LINK
  ../../adv_api/normalization/deepnorm_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/deepnorm/deepnorm_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/deepnorm_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/deepnorm/deepnorm_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/deepnorm_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/deepnorm/deepnorm_tilingdata.h
  SYMBOLIC)

# dropout
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/dropout)
file(
  CREATE_LINK
  ../../adv_api/filter/dropout_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/dropout/dropout_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../adv_api/filter/dropout_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/dropout_tiling_intf.h
  SYMBOLIC)

# gelu
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/gelu)
file(
  CREATE_LINK
  ../../adv_api/activation/gelu_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/gelu/gelu_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/gelu_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/gelu/gelu_tiling_intf.h
  SYMBOLIC)

# layernorm
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernorm)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernorm/layernorm_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernorm/layernorm_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernorm/layernorm_tilingdata.h
  SYMBOLIC)

# welfordfinalize
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/welfordfinalize
)
file(
  CREATE_LINK
  ../../adv_api/normalization/welfordfinalize_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/welfordfinalize/welfordfinalize_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/welfordfinalize_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/welfordfinalize/welfordfinalize_tiling_intf.h
  SYMBOLIC)
# normalize
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/normalize)
file(
  CREATE_LINK
  ../../adv_api/normalization/normalize_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/normalize/normalize_tiling.h
  SYMBOLIC)

# layernormgrad
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad
)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_tilingdata.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_beta_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_beta_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_beta_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_beta_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/layernorm_grad_beta_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/layernormgrad/layernorm_grad_beta_tilingdata.h
  SYMBOLIC)

# matmul
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matrix)
file(
  CREATE_LINK
  ../../adv_api/matmul/bmm_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matrix/bmm_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/matmul/matmul_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matrix/matmul_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/matmul/matmul_tiling_base.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matrix/matmul_tiling_base.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/matmul/matmul_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/matrix/matmul_tilingdata.h
  SYMBOLIC)

# hccl
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/hccl)
file(
  CREATE_LINK
  ../../adv_api/hccl/hccl_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/hccl/hccl_tilingdata.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/hccl/hccl_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/hccl/hccl_tiling.h
  SYMBOLIC)

# mean
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/mean)
file(
  CREATE_LINK
  ../../adv_api/reduce/mean_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/mean/mean_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/reduce/mean_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/mean/mean_tiling.h
  SYMBOLIC)

# reglu
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reglu)
file(
  CREATE_LINK
  ../../adv_api/activation/reglu_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reglu/reglu_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/reglu_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reglu/reglu_tiling_intf.h
  SYMBOLIC)

# rmsnorm
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/rmsnorm)
file(
  CREATE_LINK
  ../../adv_api/normalization/rmsnorm_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/rmsnorm/rmsnorm_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/rmsnorm_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/rmsnorm/rmsnorm_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/normalization/rmsnorm_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/rmsnorm/rmsnorm_tilingdata.h
  SYMBOLIC)

# sigmoid
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sigmoid)
file(
  CREATE_LINK
  ../../adv_api/activation/sigmoid_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sigmoid/sigmoid_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/sigmoid_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sigmoid/sigmoid_tiling_intf.h
  SYMBOLIC)

# silu
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/silu)
file(
  CREATE_LINK
  ../../adv_api/activation/silu_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/silu/silu_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/silu_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/silu/silu_tiling.h
  SYMBOLIC)

# reduce_xor_sum
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reduce_xor_sum
)
file(
  CREATE_LINK
  ../../adv_api/reduce/reduce_xor_sum_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/reduce_xor_sum/reduce_xor_sum_tiling.h
  SYMBOLIC)

# softmax
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax)
file(
  CREATE_LINK
  ../../adv_api/activation/softmax_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/softmax_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/softmax_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/softmax_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/softmax_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/softmax_tilingdata.h
  SYMBOLIC)

file(
  CREATE_LINK
  ../../adv_api/activation/logsoftmax_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/logsoftmax_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/logsoftmax_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/logsoftmax_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/logsoftmax_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/softmax/logsoftmax_tilingdata.h
  SYMBOLIC)

# sum
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sum)
file(
  CREATE_LINK
  ../../adv_api/reduce/sum_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sum/sum_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/reduce/sum_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/sum/sum_tiling.h
  SYMBOLIC)

# swiglu
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swiglu)
file(
  CREATE_LINK
  ../../adv_api/activation/swiglu_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swiglu/swiglu_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/swiglu_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swiglu/swiglu_tiling.h
  SYMBOLIC)

# swish
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swish)
file(
  CREATE_LINK
  ../../adv_api/activation/swish_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swish/swish_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/activation/swish_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/swish/swish_tiling.h
  SYMBOLIC)

# topk
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/topk)
file(
  CREATE_LINK
  ../../adv_api/sort/topk_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/topk/topk_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/sort/topk_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/topk/topk_tilingdata.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/sort/topk_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/topk/topk_tiling_intf.h
  SYMBOLIC)

# xor
file(MAKE_DIRECTORY
     ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/xor)
file(
  CREATE_LINK
  ../../adv_api/math/xor_tiling_intf.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/xor/xor_tiling_intf.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/math/xor_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/xor/xor_tiling.h
  SYMBOLIC)

# conv3d
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv/conv3d)
file(
  CREATE_LINK
  ../../../adv_api/conv/conv3d/conv3d_tiling_base.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv/conv3d/conv3d_tiling_base.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../../adv_api/conv/conv3d/conv3d_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv/conv3d/conv3d_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../../adv_api/conv/conv3d/conv3d_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv/conv3d/conv3d_tilingdata.h
  SYMBOLIC)

# conv3d_backprop
file(
  MAKE_DIRECTORY
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop
)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_tiling_base.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_tiling_base.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_filter_tiling_base.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_filter_tiling_base.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_filter_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_filter_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_filter_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_filter_tilingdata.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_input_tiling.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_input_tiling.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_input_tiling_base.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_input_tiling_base.h
  SYMBOLIC)
file(
  CREATE_LINK
  ../../adv_api/conv_backprop/conv3d_bp_input_tilingdata.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/conv_backprop/conv3d_bp_input_tilingdata.h
  SYMBOLIC)

file(
  CREATE_LINK ../adv_api/tiling_api.h
  ${ASCENDC_INSTALL_BASE_PATH}/asc/include/tiling/tiling_api.h
  SYMBOLIC)
