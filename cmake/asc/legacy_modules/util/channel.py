#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import os
import subprocess
from typing import Tuple

CODE_DEFAULT = 0
CODE_AIC = 1
CODE_AIV = 2
CODE_MAX = 3


def v220_mode(inst) -> int:
    if inst[6] == "f":
        # MATRIX
        return CODE_AIC
    elif inst[6] == "c":
        # FIXP
        return CODE_AIC
    elif inst[6] == "8":
        if inst[4] == "4" and inst[5] == "0" and inst[7] == "0":
            # MOVEMASK
            return 0
        # SIMD INST
        return CODE_AIV
    elif inst[6] == "9":
        # SIMD INST
        return CODE_AIV
    # DMA MOVE
    elif inst[6] == "6":
        if inst[7] == "b" and (int(inst[4], 16) & 0x8) == 0x8:
            # MOV_{SRC}_TO_{DST}_ALIGN
            return CODE_AIV
        else:
            # MOV_CUB1
            return CODE_AIC
    # DMA MOVE
    elif inst[6] == "7":
        if inst[7] == "0" and (int(inst[4], 16) & 0x8) == 0x8:
            # MOV_UB_TO_XX
            return CODE_AIV
        elif (int(inst[0], 16) & 0x7) == 0 and (int(inst[1], 16) & 0x8) == 0x8:
            # MOV_XX_TO_UB
            return CODE_AIV
        else:
            # MOV_CUB2
            return CODE_AIC
    # SCALAR
    return 0


def v310_mode_vec_ofile(little_endian, binary_32) -> int:
    vf_high_low_map = {
        "01000010011000100": "00",  # wait_intra_block
        "01000010010000100": "00",  # set_intra_block
        "01000000101000100": "00",  # set_flag
        "01000000110000100": "00",  # wait_flag
    }
    # DMA
    vec_high_low_map = {
        "0110101010": "1"  # ND_DMA_DCI
    }
    vec_high_map = {
        "0110111001",  # ND_DMA_OUT_TO_UB
        "0111010010",  # MOV_OUT_TO_UB_ALIGN_V2
        "0111010011",  # MOV_UB_TO_OUT_ALIGN_V2
    }
    vec_high_mid_map = {
        "011100001": "0100"  # MOV_UB_TO_L1
    }
    high_9 = binary_32[:9]
    high_10 = binary_32[:10]
    mid_36 = binary_32[25:29]
    low_1 = binary_32[31]

    conditions = [
        # PIPE_V: exclude movemask
        (
            little_endian[0] == "1"
            and little_endian[1] == "5"
            and not (binary_32[:11] == "00010101110" and binary_32[16:26] == "0000000000" and binary_32[27:] == "10011")
        ),
        # set, wait
        (binary_32[:17] in vf_high_low_map and binary_32[30:] == vf_high_low_map[binary_32[:17]]),
        # DMA
        (high_10 in vec_high_low_map and low_1 == vec_high_low_map[high_10]),
        (high_10 in vec_high_map),
        (high_9 in vec_high_mid_map and mid_36 == vec_high_mid_map[high_9]),
    ]

    if any(conditions):
        return CODE_AIV

    return 0


def v310_mode_cube_ofile(little_endian, binary_32) -> int:
    cube_high_low_map = {
        "0110000001": "10",  # SET_L1_2D
        "0110011000": "10",  # LOAD_OUT_TO_L1_2Dv2
    }
    cube_high_low2_map = {
        "0110110100": "1",  # LOAD_L1_TO_L0B_2D_TRANSPOSE
        "0110110101": "1",
        "0110110110": "1",
        "0110110111": "1",
    }
    cube_high_map = {
        "0110110000",  # LOAD_L1_TO_L0A_2Dv2和LOAD_L1_TO_L0B_2Dv2
        "0110110001",
        "0110110010",
        "0110110011",
        "0110101000",  # MOV_OUT _TO_L1 _MULTI_DN2NZ
        "0110101100",  # MOV_OUT _TO_L1 _MULTI_ND2NZ
        "0110111010",  # MOV_OUT_TO_L1_V2
        "0111011000",  # LOAD_L1_TO_L0A_MX_2Dv2和LOAD_L1_TO_L0B_MX_2Dv2
        "0110011100",  # LOAD_L1_TO_L0A_3Dv2和LOAD_L1_TO_L0B_3Dv2
        "0110011101",
        "0110010100",
        "0110010101",
    }
    high_9 = binary_32[:9]
    high_10 = binary_32[:10]
    mid_36 = binary_32[25:29]

    conditions = [
        # Fixpipe
        (little_endian[0] == "c" and little_endian[1] in "0123"),
        # matrix instr
        (little_endian[0] == "e" and little_endian[1] in "012345" and binary_32[30:] == "00"),
        (little_endian[0] == "f" and little_endian[1] in "01"),
        (little_endian[0] == "f" and little_endian[1] == "6" and binary_32[30:] == "00"),
        (little_endian[0] == "f" and little_endian[1] in "2345abcd" and binary_32[30] == "0"),
        # DMA
        (high_9 == "011100100" and mid_36 in ("0001", "0101")),  # DMA move inst, include MOV L1 TO UB
        (high_10 in cube_high_low_map and binary_32[30:] == cube_high_low_map[high_10]),
        (high_10 in cube_high_low2_map and binary_32[31] == cube_high_low2_map[high_10]),
        (high_10 in cube_high_map),
    ]

    if any(conditions):
        return CODE_AIC

    return 0


def v310_mode(inst, cubemode) -> int:
    if len(inst) != 8:
        return 0

    little_endian = f"{int.from_bytes(int(inst, 16).to_bytes(4, 'little'), 'big'):08x}"
    binary_32 = bin(int(little_endian, 16))[2:].zfill(32)

    return v310_mode_cube_ofile(little_endian, binary_32) if cubemode else v310_mode_vec_ofile(little_endian, binary_32)


def get_code_channel(dst_file: str, c310mode_cubemode_tuple: Tuple) -> Tuple[bool, int, str]:
    c310mode, cubemode = c310mode_cubemode_tuple
    if not os.path.isfile(dst_file):
        return False, CODE_DEFAULT, f"file {dst_file} doesn't exist."

    objdump_cmd = ["llvm-objdump", "-s", "-j", ".text", dst_file]
    proc = subprocess.run(objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False)
    out = proc.stdout.decode()
    if proc.returncode != 0:
        return False, CODE_DEFAULT, f"llvm-objdump error, message is {out}"
    mode = 0
    lines = out.split("\n")
    for line in lines:
        insts = line.strip().split()
        if len(insts) < 5:
            continue
        for inst in insts[1:5]:
            if len(inst) != 8:
                continue
            if c310mode:
                mode |= v310_mode(inst, cubemode)
            else:
                mode |= v220_mode(inst)

    if mode >= CODE_MAX:
        return False, mode, f"unknown code mode {mode}."
    return True, mode, ""
