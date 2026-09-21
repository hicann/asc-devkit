#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import numpy as np
from typing import Union

DTYPE_SIZE_B16 = 2  # b16数据类型字节数
DTYPE_SIZE_B32 = 4  # b32数据类型字节数


class GlobalFMatrixConfig:
    """FMatrix全局配置管理器"""

    def __init__(self):
        self.left_l1h = 0
        self.left_l1w = 0
        self.left_pad_list = [0, 0, 0, 0]
        self.right_l1h = 0
        self.right_l1w = 0
        self.right_pad_list = [0, 0, 0, 0]
        self.pad_value = 0


global_fmatrix_config = GlobalFMatrixConfig()


def pad_to_16(length: int) -> int:
    """
    计算向上补齐到16倍数所需的填充长度

    Args:
        length: 原始长度(正整数)

    Returns:
        int: 需填充的长度(0表示已对齐)
    """
    if not isinstance(length, int) or length < 0:
        raise ValueError(f"长度必须是非负整数, 当前输入: {length}")
    remainder = length % 16
    return 0 if remainder == 0 else 16 - remainder


def ceil_aligned_to_16(length: int) -> int:
    """
    计算向上对齐到16倍数的结果

    Args:
        length: 原始长度(正整数)

    Returns:
        int: 对齐后的长度
    """
    if not isinstance(length, int) or length < 0:
        raise ValueError(f"长度必须是非负整数, 当前输入: {length}")
    return (length + 15) // 16 * 16


def set_fmatrix(l1h: int, l1w: int, pad_list: list, fmatrix_mode: int) -> None:
    """
    设置FMatrix全局配置

    Args:
        l1h: L1高度
        l1w: L1宽度
        pad_list: 填充列表 [padleft, padright, padtop, padbottom]
        fmatrix_mode: 0-左矩阵, 1-右矩阵
    """
    if fmatrix_mode == 0:
        global_fmatrix_config.left_l1h = l1h
        global_fmatrix_config.left_l1w = l1w
        global_fmatrix_config.left_pad_list = pad_list
    else:
        global_fmatrix_config.right_l1h = l1h
        global_fmatrix_config.right_l1w = l1w
        global_fmatrix_config.right_pad_list = pad_list


def set_load_data_padding_value(pad_value: Union[np.int8, np.int16, np.int32]) -> None:
    """
    设置填充值

    Args:
        pad_value: 填充值
    """
    global_fmatrix_config.pad_value = pad_value


def nc1hwc0_im2col(
    nc1hwc0_input: np.ndarray,
    pad_list: list,
    pad_value: Union[np.int8, np.int16, np.int32],
    l1h: int,
    l1w: int,
    channel_size: int,
    stride_h: int,
    stride_w: int,
    filter_h: int,
    filter_w: int,
    dilation_filter_h: int,
    dilation_filter_w: int,
    filter_size_w: bool,
    filter_size_h: bool,
) -> np.ndarray:
    """
    NC1HWC0 格式张量的 im2col 展开(卷积窗口提取)

    Args:
        nc1hwc0_input: 输入5维NC1HWC0张量, shape=[N, C1, H, W, C0]
        pad_list: 填充列表 [padleft, padright, padtop, padbottom]
        pad_value: 填充值
        l1h: feature map输入高度(需与输入张量H匹配)
        l1w: feature map输入宽度(需与输入张量W匹配)
        channel_size: 总通道数(需等于C1*C0)
        stride_h: 高度方向步长
        stride_w: 宽度方向步长
        filter_h: 卷积核高度
        filter_w: 卷积核宽度
        dilation_filter_h: 高度方向膨胀率
        dilation_filter_w: 宽度方向膨胀率
        filter_size_w: 是否扩展宽度(True则filter_w += 256)
        filter_size_h: 是否扩展高度(True则filter_h += 256)

    Returns:
        np.ndarray: im2col展开结果, shape=[N, C1, Hk*Wk, Hout*Wout, C0]

    Raises:
        ValueError: 输入维度/尺寸不合法、输出维度非法
    """
    # 1. 输入合法性校验
    if nc1hwc0_input.ndim != 5:
        raise ValueError(f"输入必须是5维NC1HWC0张量, 当前维度: {nc1hwc0_input.ndim}")

    n, c1, h_ori, w_ori, c0 = nc1hwc0_input.shape
    if h_ori != l1h or w_ori != l1w:
        raise ValueError(f"传入的L1尺寸({l1h},{l1w})与张量原始尺寸({h_ori},{w_ori})不匹配！")
    if c1 * c0 != channel_size:
        raise ValueError(f"通道数不匹配: C1*C0={c1 * c0} ≠ channel_size={channel_size}")

    # 2. 卷积核参数处理
    filter_h = filter_h + 256 if filter_size_h else filter_h
    filter_w = filter_w + 256 if filter_size_w else filter_w

    # 3. 计算膨胀后的核尺寸
    kernel_h_dilated = (filter_h - 1) * dilation_filter_h + 1
    kernel_w_dilated = (filter_w - 1) * dilation_filter_w + 1

    # 4. 填充处理
    padleft, padright, padtop, padbottom = pad_list
    pad_width = ((0, 0), (0, 0), (padtop, padbottom), (padleft, padright), (0, 0))
    nc1hwc0_padded = np.pad(nc1hwc0_input, pad_width=pad_width, mode="constant", constant_values=pad_value)
    h_padded = l1h + padtop + padbottom
    w_padded = l1w + padleft + padright

    # 5. 计算输出维度
    h_out = (h_padded - kernel_h_dilated) // stride_h + 1
    w_out = (w_padded - kernel_w_dilated) // stride_w + 1
    if h_out <= 0 or w_out <= 0:
        raise ValueError(f"输出维度非法: Hout={h_out}, Wout={w_out}")
    hw_out = h_out * w_out
    kernel_flat_dim = filter_h * filter_w

    # 6. 初始化结果
    im2col_result = np.zeros((n, c1, kernel_flat_dim, hw_out, c0), dtype=nc1hwc0_input.dtype)

    # 7. 向量化提取卷积窗口
    h_indices = np.arange(0, h_padded - kernel_h_dilated + 1, stride_h)
    w_indices = np.arange(0, w_padded - kernel_w_dilated + 1, stride_w)
    h_grid_base = np.arange(filter_h) * dilation_filter_h
    w_grid_base = np.arange(filter_w) * dilation_filter_w

    out_idx = 0
    for h_start in h_indices:
        for w_start in w_indices:
            # 生成膨胀后的网格索引
            h_grid = h_grid_base + h_start
            w_grid = w_grid_base + w_start
            h_mesh, w_mesh = np.meshgrid(h_grid, w_grid, indexing="ij")

            # 提取窗口并展平
            window = nc1hwc0_padded[:, :, h_mesh, w_mesh, :]
            window_flat = window.reshape(n, c1, kernel_flat_dim, c0)

            # 写入结果
            im2col_result[:, :, :, out_idx, :] = window_flat
            out_idx += 1

    return im2col_result


def _prepare_src_slice(
    im2col_out: np.ndarray, m_start_pt: int, k_start_pt: int, m_extension: int, k_extension: int, c0: int
) -> tuple[np.ndarray, np.ndarray]:
    """
    预处理: 从im2col结果中提取切片并重塑为分形格式

    Args:
        im2col_out: im2col展开结果, shape=[N, C1, Hk*Wk, Hout*Wout, C0]
        m_start_pt: M维度起始位置
        k_start_pt: K维度起始位置
        m_extension: M维度扩展长度
        k_extension: K维度扩展长度
        c0: C0维度大小

    Returns:
        tuple: (src_slice, src_fractal_z)
            - src_slice: 提取的切片, shape=[N, k_ext//C0, m_ext, C0]
            - src_fractal_z: 分形格式张量, shape=[N, k_ext//C0, m_ext//16, 16, C0]
    """
    # 重塑为2D格式
    n, c1, kernel_flat_dim, hw_out, c0 = im2col_out.shape
    src_2d = im2col_out.reshape(n, c1 * kernel_flat_dim, hw_out, c0)

    # 提取切片
    src_slice = src_2d[:, k_start_pt // c0 : (k_start_pt + k_extension) // c0, m_start_pt : m_start_pt + m_extension, :]

    # 重塑为分形格式
    m_aligned = ceil_aligned_to_16(src_slice.shape[2])
    src_fractal_z = src_slice.reshape(src_slice.shape[0], src_slice.shape[1], m_aligned // 16, 16, c0)

    return src_slice, src_fractal_z


def _process_b32_transpose(src_fractal_z: np.ndarray) -> np.ndarray:
    """
    B32格式转置处理(核心维度变换+填充)

    Args:
        src_fractal_z: 分形格式张量, shape=[N, C1*Hk*Wk, Wout_Hout//16, 16, C0]

    Returns:
        np.ndarray: 处理后的张量(含维度变换+填充)
    """
    # 维度重排: [N, C1*Hk*Wk, Wout_Hout//16, 16, C0] -> [N, Wout_Hout//16, 16, C1*Hk*Wk*C0]
    tmp = src_fractal_z.transpose(0, 2, 3, 1, 4)
    src_fractal_z_transposed = tmp.reshape(tmp.shape[0], tmp.shape[1], tmp.shape[2], tmp.shape[3] * tmp.shape[4])

    # 16对齐填充
    dim3 = src_fractal_z_transposed.shape[3]
    pad_dim3 = pad_to_16(dim3)
    src_fractal_z_padded = np.pad(
        src_fractal_z_transposed, pad_width=((0, 0), (0, 0), (0, 0), (0, pad_dim3)), mode="constant"
    )

    # 维度拆分: 16 -> 2×8
    tmp_shape = src_fractal_z_padded.reshape(
        src_fractal_z_padded.shape[0],
        src_fractal_z_padded.shape[1],
        src_fractal_z_padded.shape[2] // 8,
        8,
        src_fractal_z_padded.shape[3],
    )

    # 维度重排: [N, Wout_Hout//16, 2, 8, C1*Hk*Wk*C0] -> [N, Wout_Hout//16, 2, C1*Hk*Wk*C0, 8]
    src_transposed = tmp_shape.transpose(0, 1, 2, 4, 3)

    return src_transposed, dim3, pad_dim3


def move_to_l0a(
    dst: np.ndarray,
    im2col_out: np.ndarray,
    m_start_pt: int,
    k_start_pt: int,
    m_extension: int,
    k_extension: int,
    en_transpose: bool,
) -> np.ndarray:
    """
    数据搬运到L0A

    Args:
        dst: 目标数组
        im2col_out: im2col展开结果
        m_start_pt: M维度起始位置
        k_start_pt: K维度起始位置
        m_extension: M维度扩展长度
        k_extension: K维度扩展长度
        en_transpose: 是否启用转置

    Returns:
        np.ndarray: 填充后的目标数组
    """
    n, _, _, _, c0 = im2col_out.shape
    _, src_fractal_z = _prepare_src_slice(im2col_out, m_start_pt, k_start_pt, m_extension, k_extension, c0)

    if not en_transpose:
        dst = src_fractal_z.transpose(0, 2, 1, 3, 4)
    else:
        dtype_size = im2col_out.dtype.itemsize
        if dtype_size == DTYPE_SIZE_B16:
            dst = src_fractal_z.transpose(0, 1, 2, 4, 3)
        elif dtype_size == DTYPE_SIZE_B32:
            src_transposed, dim3, pad_dim3 = _process_b32_transpose(src_fractal_z)

            # 进一步维度变换
            src_reshape = src_transposed.reshape(
                src_transposed.shape[0],
                src_transposed.shape[1],
                src_transposed.shape[2],
                src_transposed.shape[3] // 16,
                16,
                8,
            )
            dst_tmp = src_reshape.transpose(0, 3, 1, 2, 4, 5)

            # 重塑目标数组并写入(跳过填充区)
            dst = dst.reshape(dst_tmp.shape)
            mask = np.ones(dst_tmp.shape, dtype=bool)
            if pad_dim3 > 0:
                mask[:, -1, :, :, pad_dim3:, :] = False
            dst[mask] = dst_tmp[mask]
        else:
            raise ValueError(f"不支持的数据类型字节数: {dtype_size}")

    return dst


def move_to_l0b(
    dst: np.ndarray,
    im2col_out: np.ndarray,
    m_start_pt: int,
    k_start_pt: int,
    m_extension: int,
    k_extension: int,
    en_transpose: bool,
) -> np.ndarray:
    """
    数据搬运到L0B

    Args:
        dst: 目标数组
        im2col_out: im2col展开结果
        m_start_pt: M维度起始位置
        k_start_pt: K维度起始位置
        m_extension: M维度扩展长度
        k_extension: K维度扩展长度
        en_transpose: 是否启用转置

    Returns:
        np.ndarray: 填充后的目标数组
    """
    n, _, _, _, c0 = im2col_out.shape
    _, src_fractal_z = _prepare_src_slice(im2col_out, m_start_pt, k_start_pt, m_extension, k_extension, c0)

    dtype_size = im2col_out.dtype.itemsize
    if dtype_size == DTYPE_SIZE_B16:
        dst = src_fractal_z.transpose(0, 2, 1, 4, 3)
    elif dtype_size == DTYPE_SIZE_B32:
        src_transposed, dim3, pad_dim3 = _process_b32_transpose(src_fractal_z)

        # 重塑目标数组并写入(跳过填充区)
        dst = dst.reshape(src_transposed.shape)
        mask = np.ones(src_transposed.shape, dtype=bool)
        if pad_dim3 > 0:
            mask[:, :, :, dim3:] = False
        dst[mask] = src_transposed[mask]
    else:
        raise ValueError(f"不支持的数据类型字节数: {dtype_size}")

    return dst


def load3d_to_l0a(
    nc1hwc0_input: np.ndarray,
    dst: np.ndarray,
    pad_list: list,
    pad_value: Union[np.int8, np.int16, np.int32],
    l1h: int,
    l1w: int,
    channel_size: int,
    m_start_pt: int,
    k_start_pt: int,
    m_extension: int,
    k_extension: int,
    stride_h: int,
    stride_w: int,
    filter_h: int,
    filter_w: int,
    dilation_filter_h: int,
    dilation_filter_w: int,
    filter_size_w: bool,
    filter_size_h: bool,
    en_transpose: bool,
    fmatrix_ctrl: bool,
    is_set_fmatrix: bool,
    is_set_padding: bool,
) -> np.ndarray:
    """
    3D数据从L1加载到L0A

    Args:
        nc1hwc0_input: 输入feature map, NC1HWC0格式张量
        dst: 目标输出
        pad_list: 填充列表
        pad_value: 填充值
        l1h: feature map输入高度
        l1w: feature map输入宽度
        channel_size: 总通道数
        m_start_pt: M维度待提取数据起始位置
        k_start_pt: K维度待提取数据起始位置
        m_extension: M维度提取数据长度
        k_extension: K维度提取数据长度
        stride_h: 卷积核在高度H维度滑动的步长
        stride_w: 卷积核在高度W维度滑动的步长
        filter_h: 卷积核高度
        filter_w: 卷积核宽度
        dilation_filter_h: 卷积核高度膨胀率
        dilation_filter_w: 卷积核宽度膨胀率
        filter_size_w: 是否扩展宽度
        filter_size_h: 是否扩展高度
        en_transpose: 是否启用转置
        fmatrix_ctrl: FMatrix控制位
        is_set_fmatrix: 是否在接口内部设置FMatrix属性描述
        is_set_padding: 是否在接口内部设置填充值

    Returns:
        np.ndarray: 填充后的目标数组
    """
    # 全局配置覆盖
    if not is_set_fmatrix:
        if not fmatrix_ctrl:
            l1h = global_fmatrix_config.left_l1h
            l1w = global_fmatrix_config.left_l1w
            pad_list = global_fmatrix_config.left_pad_list
        else:
            l1h = global_fmatrix_config.right_l1h
            l1w = global_fmatrix_config.right_l1w
            pad_list = global_fmatrix_config.right_pad_list

    if not is_set_padding:
        pad_value = global_fmatrix_config.pad_value

    # 执行im2col + 搬运
    im2col_out = nc1hwc0_im2col(
        nc1hwc0_input,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
    )
    print(f"im2col输出shape: {im2col_out.shape}")
    print("im2col_out:", im2col_out)
    dst = move_to_l0a(dst, im2col_out, m_start_pt, k_start_pt, m_extension, k_extension, en_transpose)
    return dst


def load3d_to_l0b(
    nc1hwc0_input: np.ndarray,
    dst: np.ndarray,
    pad_list: list,
    pad_value: Union[np.int8, np.int16, np.int32],
    l1h: int,
    l1w: int,
    channel_size: int,
    m_start_pt: int,
    k_start_pt: int,
    m_extension: int,
    k_extension: int,
    stride_h: int,
    stride_w: int,
    filter_h: int,
    filter_w: int,
    dilation_filter_h: int,
    dilation_filter_w: int,
    filter_size_w: bool,
    filter_size_h: bool,
    en_transpose: bool,
    fmatrix_ctrl: bool,
    is_set_fmatrix: bool,
    is_set_padding: bool,
) -> np.ndarray:
    """
    3D数据从L1加载到L0B(参数同load3d_to_l0a)
    """
    # 全局配置覆盖
    if not is_set_fmatrix:
        if not fmatrix_ctrl:
            l1h = global_fmatrix_config.left_l1h
            l1w = global_fmatrix_config.left_l1w
            pad_list = global_fmatrix_config.left_pad_list
        else:
            l1h = global_fmatrix_config.right_l1h
            l1w = global_fmatrix_config.right_l1w
            pad_list = global_fmatrix_config.right_pad_list

    if not is_set_padding:
        pad_value = global_fmatrix_config.pad_value

    # 执行im2col + 搬运
    im2col_out = nc1hwc0_im2col(
        nc1hwc0_input,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
    )
    print(f"im2col输出shape: {im2col_out.shape}")
    print("im2col输出", im2col_out)
    dst = move_to_l0b(dst, im2col_out, m_start_pt, k_start_pt, m_extension, k_extension, en_transpose)
    return dst


def test_load3d_l1_to_l0a_b8_no_transpose():
    """测试L1->L0A B8不带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 32
    input_dtype = np.int8
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 32, 0
    m_extension, k_extension = 32, 64
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 1, 1, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = False
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0a(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0A B8不带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0a_b16_no_transpose():
    """测试L1->L0A B16不带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 16
    input_dtype = np.int16
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 32, 0
    m_extension, k_extension = 32, 32
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 1, 1, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = False
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0a(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0A B16不带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0a_b16_with_transpose():
    """测试L1->L0A B16带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 16
    input_dtype = np.int16
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 32, 0
    m_extension, k_extension = 32, 32
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 1, 1, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = True
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0a(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0A B16带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0a_b32_no_transpose():
    """测试L1->L0A B32不带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 8
    input_dtype = np.int32
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 32, 0
    m_extension, k_extension = 32, 24
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 1, 1, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = False
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0a(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0A B32不带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0a_b32_with_transpose():
    """测试L1->L0A B32带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 8
    input_dtype = np.int32
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 32, 0
    m_extension, k_extension = 32, 24
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 1, 1, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = True
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0a(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0A B32带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0b_b16_with_transpose():
    """测试L1->L0B B16带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 16
    input_dtype = np.int16
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 16, 0
    m_extension, k_extension = 32, 32
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 2, 2, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = False
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0b(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0B B16带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


def test_load3d_l1_to_l0b_b32_with_transpose():
    """测试L1->L0B B32带转置场景"""
    # 配置参数
    n, c1, h, w, c0 = 1, 3, 8, 8, 8
    input_dtype = np.int32
    nc1hwc0_input = np.random.randint(1, 10, size=(n, c1, h, w, c0)).astype(input_dtype)

    pad_list = (0, 0, 0, 0)
    pad_value = 0
    l1h, l1w, channel_size = 8, 8, c1 * c0
    m_start_pt, k_start_pt = 16, 0
    m_extension, k_extension = 32, 32
    stride_h, stride_w = 1, 1
    filter_h, filter_w, dilation_filter_h, dilation_filter_w = 2, 2, 1, 1
    filter_size_w, filter_size_h = False, False
    en_transpose = False
    fmatrix_ctrl = False
    is_set_fmatrix, is_set_padding = True, True

    # 初始化目标数组
    dst = np.random.randint(
        1, 2, size=(ceil_aligned_to_16(m_extension) // 16, ceil_aligned_to_16(k_extension) // c0, 16, c0)
    ).astype(input_dtype)

    # 执行加载
    dst = load3d_to_l0b(
        nc1hwc0_input,
        dst,
        pad_list,
        pad_value,
        l1h,
        l1w,
        channel_size,
        m_start_pt,
        k_start_pt,
        m_extension,
        k_extension,
        stride_h,
        stride_w,
        filter_h,
        filter_w,
        dilation_filter_h,
        dilation_filter_w,
        filter_size_w,
        filter_size_h,
        en_transpose,
        fmatrix_ctrl,
        is_set_fmatrix,
        is_set_padding,
    )
    print(f"测试L1->L0B B32带转置完成, 目标数组shape: {dst.shape}")
    print("目标结果: ", dst)


if __name__ == "__main__":
    np.set_printoptions(threshold=np.inf)
    np.random.seed(0)  # 固定随机种子

    # 运行测试
    test_load3d_l1_to_l0a_b8_no_transpose()
    test_load3d_l1_to_l0a_b16_no_transpose()
    test_load3d_l1_to_l0a_b16_with_transpose()
    test_load3d_l1_to_l0a_b32_no_transpose()
    test_load3d_l1_to_l0a_b32_with_transpose()

    test_load3d_l1_to_l0b_b16_with_transpose()
    test_load3d_l1_to_l0b_b32_with_transpose()
