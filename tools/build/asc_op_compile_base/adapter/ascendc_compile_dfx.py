#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
ascendc compile dfx
"""

import enum
from functools import reduce
from .ascendc_common_utility import (
    CommonUtility,
    CompileInfo,
    get_kernel_fun_name_with_tiling_key_and_kernel_type,
)
from .get_op_tiling import OpInfo, TilingInfo
from tbe.common.buildcfg import get_current_build_config
from tbe.tvm.error_mgr import TBE_DEFAULT_PYTHON_ERROR_CODE
from .ascendc_constants import (
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
    INPUT_OUTPUT_DTYPE_LEN,
    TILING_KEY_MACRO,
)
from .global_storage import global_var_storage
from .log_utils import AscendCLogLevel


def _ascendc_singleton(cls):
    instances = {}

    def get_instance(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]

    return get_instance


class DFXParamType(enum.Enum):
    """Type of the parameter."""

    TENSOR = 1  # only used by tbe
    INPUT = 2
    OUTPUT = 3
    WORKSPACE = 4
    ASCENDCLOG = 5
    MC2CTX = 6
    TILING = 7
    L1 = 8
    L2 = 9
    OVERFLOW = 10
    FFTS = 11
    SHAPE_TENSOR = 12  # for third type operator
    DEFAULT = -1


class DFXPointType(enum.Enum):
    """Type of the Point."""

    LEVEL_1 = 1
    LEVEL_2 = 2
    LEVEL_2_WITH_SHAPE = 3
    LEVEL_1_FOR_SHAPE_TENSOR = 4
    DEFAULT = -1


class ArgTypeSize(enum.Enum):
    """Type of the arg size"""

    U16 = 2
    U64 = 8


class FuncMetaType(enum.Enum):
    """Type of tlv"""

    F_TYPE_KTYPE = 1  # kernel type tlv
    F_TYPE_CROSS_CORE_SYNC = 2  # cross core sync
    F_TYPE_MIX_TASK_RATION = 3  # MIX CORE TYPE
    F_TYPE_L0_EXCEPTION_DFX = 4  # DFX tlv for header
    F_TYPE_L0_EXCEPTION_DFX_ARGSINFO = 5  # DFX tlv for args info
    F_TYPE_L0_EXCEPTION_DFX_IS_TIK = 6  # DFX tlv mark for TIK
    F_TYPE_MAX = -1


class DFXArgInfo:
    def __init__(self, param_name: str, param_type: DFXParamType):
        if param_type == DFXParamType.DEFAULT:
            CommonUtility().ascendc_raise_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, ("DFX paramter type is invaild.")
            )
        self.param_name: str = param_name
        self.param_type: DFXParamType = param_type
        self.point_type: DFXPointType = DFXPointType.LEVEL_1
        self.param_size: int = -1
        self.param_dtype: int = -1
        self.param_dim: int = -1
        self.param_shape: tuple = ()
        self.args_type: int = -1
        self.args_dfx_info: list = []


@_ascendc_singleton
class DFXSectionGenerator:
    def dfx_info_reset(self, op_info: OpInfo):
        self.index: int = 0
        self.param_to_index: dict = {}
        self.params_base: list = []  # push paramter in params_base
        self.section_size: int = 0
        self.gen_dfx_struct_flag: bool = False
        self.is_support: bool = False
        self.param_placeholder_num = 0
        self.is_support = self.is_support_dfx(op_info)

    @staticmethod
    def is_support_dfx(op_info: OpInfo) -> bool:
        # only support david and ascend910B and ascend310p dex section now
        arch_support = (
            CommonUtility.is_v200()
            or CommonUtility.is_v220()
            or CommonUtility.is_c310()
            or CommonUtility.is_v300()
        )
        option_support = True
        # dfx do not support, because of custom framework is too old or enable super kernel
        if (
            op_info.param_type_list is None
            or op_info.mc2_ctx is None
            or global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        ):
            option_support = False
        return arch_support and option_support

    def update_is_support(self, op_info: OpInfo):
        self.is_support = self.is_support_dfx(op_info)

    def insert_param(self, parameter: DFXArgInfo):
        self.params_base.append(parameter)
        self.param_to_index[parameter.param_name] = self.index
        self.index += 1
        if parameter.param_type in [
            DFXParamType.INPUT,
            DFXParamType.OUTPUT,
            DFXParamType.WORKSPACE,
            DFXParamType.SHAPE_TENSOR,
        ]:
            self.param_placeholder_num += 1

    def get_param(self, param_name: str) -> DFXArgInfo:
        try:
            param_index = self.param_to_index[param_name]
            param = self.params_base[param_index]
            return param
        except Exception as err:
            CommonUtility().ascendc_raise_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                (f"DFX paramter name: {param_name} do not currect.", err),
            )
            return DFXArgInfo("error", DFXParamType.TENSOR)

    def _set_level_1_input_output_dfx_info(self, param_info):
        parameter: DFXArgInfo = self.get_param(param_info["param_name"])
        parameter.param_size = (
            reduce(lambda x, y: x * y, param_info.get("shape"))
            * INPUT_OUTPUT_DTYPE_LEN[(param_info.get("dtype"))]
        )
        parameter.param_dim = len(param_info.get("shape"))
        parameter.param_shape = param_info.get("shape")

    def set_size_of_dfx_info(self, param_name: str, param_size: int):
        parameter: DFXArgInfo = self.get_param(param_name)
        parameter.param_size = param_size

    def _set_size_and_dim(
        self, compile_info: CompileInfo, op_info: OpInfo, tiling_info: TilingInfo
    ):
        input_output_info = []
        for io_info in [op_info.inputs, op_info.outputs]:
            if list(io_info):
                input_output_info += io_info

        for io_index, op_param in enumerate(input_output_info):
            if op_param is None:
                continue
            if op_info.param_type_list[io_index] == "dynamic":
                parameter: DFXArgInfo = self.get_param(op_param.get("param_name"))
                if CommonUtility.is_v200() or CommonUtility.is_v220():
                    parameter.param_dtype = INPUT_OUTPUT_DTYPE_LEN[
                        (op_param.get("dtype"))
                    ]
                else:
                    # for soc version not in v220 and v200, set dtype by bits
                    parameter.param_dtype = (
                        INPUT_OUTPUT_DTYPE_LEN[(op_param.get("dtype"))] * 8
                    )
                parameter.point_type = DFXPointType.LEVEL_2_WITH_SHAPE
            elif tiling_info.static_shape_flag:
                self._set_level_1_input_output_dfx_info(op_param)

        if tiling_info.static_shape_flag and tiling_info.static_workspace_size >= 0:
            self.set_size_of_dfx_info("workspace", tiling_info.static_workspace_size)

    def _tran_dfx_info_to_uint8_t(self, origin_value: int, origin_bytes: ArgTypeSize):
        return [
            ((int(origin_value) >> int((origin_bytes.value - i - 1) * 8)) & 0xFF)
            for i in range(origin_bytes.value)
        ]

    def _tran_dfx_info_to_string(self, string_value: str):
        if "oom" in get_current_build_config("tir.op_debug_config"):
            return [
                f"(((((sizeof({string_value}) + 7) / 8) * 8 + 8 + 8 * {self.param_placeholder_num} ) >> \
{int((7 - i) * 8)}) & 0xff)"
                for i in range(8)
            ]
        else:
            return [
                f"(((sizeof({string_value}) + 8) >> {int((7 - i) * 8)}) & 0xff)"
                for i in range(8)
            ]

    def _tran_dfx_info_to_value_string(self, size_value: int):
        if "oom" in get_current_build_config("tir.op_debug_config"):
            total_size = (
                ((size_value + 7) // 8) * 8 + 8 + 8 * self.param_placeholder_num
            )
        else:
            total_size = size_value + 8
        return [str((total_size >> ((7 - i) * 8)) & 0xFF) for i in range(8)]

    def _generate_binary_for_input_and_output(self, param: DFXArgInfo):
        # for static level_1 record real size dim shape, for level 2 push -1
        param.args_dfx_info.extend(
            self._tran_dfx_info_to_uint8_t(param.param_size, ArgTypeSize.U64)
        )
        # param append size to arg size: 1 * sizefof(uint64_t)
        if param.point_type in [
            DFXPointType.LEVEL_1,
            DFXPointType.LEVEL_1_FOR_SHAPE_TENSOR,
        ]:
            param.args_dfx_info.extend(
                self._tran_dfx_info_to_uint8_t(param.param_dim, ArgTypeSize.U64)
            )
            for shape in param.param_shape:
                param.args_dfx_info.extend(
                    self._tran_dfx_info_to_uint8_t(shape, ArgTypeSize.U64)
                )
        elif param.point_type == DFXPointType.LEVEL_2_WITH_SHAPE:
            param.args_dfx_info.extend(
                self._tran_dfx_info_to_uint8_t(param.param_dtype, ArgTypeSize.U64)
            )

    def _generate_binary_for_tiling(
        self, tiling_key: str, tiling_info: TilingInfo, compile_info: CompileInfo
    ):
        parameter: DFXArgInfo = self.get_param("tiling")
        # use user-defined tiling struct
        if len(compile_info.tiling_key_struct_map) > 0:
            parameter.args_dfx_info[-8:] = self._tran_dfx_info_to_string(
                compile_info.tiling_key_struct_map[tiling_key]
            )
        else:
            tiling_size = tiling_info.default_tiling_size
            if tiling_key in tiling_info.tiling_key_data_size:
                tiling_size = tiling_info.tiling_key_data_size[tiling_key]
            if "oom" in get_current_build_config("tir.op_debug_config"):
                # tiling need align to 8 bytes, dfx need 8 bytes for dfx point,
                # oom need allocate 8 * (input + output + shape_tensor+ workspace)
                tiling_size = (
                    ((tiling_size + 7) // 8) * 8 + 8 + 8 * self.param_placeholder_num
                )
            else:
                tiling_size = tiling_size + 8
            parameter.args_dfx_info[-8:] = self._tran_dfx_info_to_uint8_t(
                tiling_size, ArgTypeSize.U64
            )

    def _generate_binary_for_tiling_without_register(
        self, tiling_info: TilingInfo, tiling_key: str, tiling_key_struct_size_map: dict
    ):
        parameter: DFXArgInfo = self.get_param("tiling")
        tiling_struct_info = tiling_key_struct_size_map.get(str(tiling_key), None)
        if tiling_struct_info is not None:
            _, tiling_struct_size = tiling_struct_info
        parameter.args_dfx_info[-8:] = self._tran_dfx_info_to_value_string(
            tiling_struct_size
        )

    def _generate_binary_section_for_static(
        self, compile_info: CompileInfo, op_info: OpInfo, tiling_info: TilingInfo
    ):
        self._set_size_and_dim(compile_info, op_info, tiling_info)
        for param in self.params_base:
            param.args_type = ((param.point_type.value & 0xFF) << 16) | (
                param.param_type.value & 0xFF
            )
            param.args_dfx_info.extend(
                self._tran_dfx_info_to_uint8_t(param.args_type, ArgTypeSize.U64)
            )
            if (
                param.param_type == DFXParamType.INPUT
                or param.param_type == DFXParamType.OUTPUT
            ):
                self._generate_binary_for_input_and_output(param)
            elif param.param_type in [
                DFXParamType.WORKSPACE,
                DFXParamType.SHAPE_TENSOR,
            ]:
                param.args_dfx_info.extend(
                    self._tran_dfx_info_to_uint8_t(param.param_size, ArgTypeSize.U64)
                )

    def _generate_binary_section_for_dynamic(
        self, compile_info: CompileInfo, op_info: OpInfo, tiling_info: TilingInfo
    ):
        self._set_size_and_dim(compile_info, op_info, tiling_info)
        for param in self.params_base:
            param.args_type = ((param.point_type.value & 0xFF) << 16) | (
                param.param_type.value & 0xFF
            )
            param.args_dfx_info.extend(
                self._tran_dfx_info_to_uint8_t(param.args_type, ArgTypeSize.U64)
            )
            if param.param_type in [
                DFXParamType.INPUT,
                DFXParamType.OUTPUT,
                DFXParamType.SHAPE_TENSOR,
            ]:
                # for dynamic push size to -1
                param.args_dfx_info.extend(
                    self._tran_dfx_info_to_uint8_t(param.param_size, ArgTypeSize.U64)
                )
                # for dynamic point, append dtype of point
                if param.point_type == DFXPointType.LEVEL_2_WITH_SHAPE:
                    param.args_dfx_info.extend(
                        self._tran_dfx_info_to_uint8_t(
                            param.param_dtype, ArgTypeSize.U64
                        )
                    )
            elif param.param_type == DFXParamType.TILING:
                # tiling need set by placeholder
                param.args_dfx_info.extend(
                    self._tran_dfx_info_to_uint8_t(0, ArgTypeSize.U64)
                )

    def generate_dfx_binary(
        self, compile_info: CompileInfo, op_info: OpInfo, tiling_info: TilingInfo
    ):
        if self.is_support is True:
            if tiling_info.static_shape_flag:
                self._generate_binary_section_for_static(
                    compile_info, op_info, tiling_info
                )
            else:
                self._generate_binary_section_for_dynamic(
                    compile_info, op_info, tiling_info
                )

    def generate_kernel_type_section(self, compile_info: CompileInfo, kernel_name: str):
        section_var = ""
        if CommonUtility.is_v220() or CommonUtility.is_c310():
            short_soc_version = global_var_storage.get_variable(
                "ascendc_short_soc_version"
            )
            if compile_info.code_channel == CORE_TYPE_MIX:
                section_var += f"static const struct FunLevelMixCoreType {kernel_name}_kernel_type_section __attribute__ "
                section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
                section_var += (
                    "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
            {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 2} };\n"
                )
            elif compile_info.hard_sync:
                if compile_info.code_channel in [CORE_TYPE_VEC]:
                    section_var += f"static const struct FunLevelMixCoreType {kernel_name}_kernel_type_section __attribute__ "
                    section_var += (
                        f'((used, section (".ascend.meta.{kernel_name}"))) = '
                    )
                    section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIV_MAIN},\
                                    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 0, 1} };\n"
                elif compile_info.code_channel in [CORE_TYPE_CUBE]:
                    section_var += f"static const struct FunLevelMixCoreType {kernel_name}_kernel_type_section __attribute__ "
                    section_var += (
                        f'((used, section (".ascend.meta.{kernel_name}"))) = '
                    )
                    section_var += (
                        "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
                {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 0} };\n"
                    )
            elif compile_info.code_channel in [CORE_TYPE_VEC]:
                section_var += f"static const struct FunLevelKType {kernel_name}_kernel_type_section __attribute__ "
                section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
                section_var += (
                    "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_AIV} };\n"
                )
            elif compile_info.code_channel in [CORE_TYPE_CUBE]:
                section_var += f"static const struct FunLevelKType {kernel_name}_kernel_type_section __attribute__ "
                section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
                section_var += (
                    "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_AIC} };\n"
                )
            else:
                CommonUtility().ascendc_raise_python_err(
                    TBE_DEFAULT_PYTHON_ERROR_CODE,
                    f"invalid kernel type in {short_soc_version}, kernel type: {compile_info.code_channel}, \
                    is hard sync {compile_info.hard_sync}",
                )
        elif CommonUtility.is_v200() or CommonUtility.is_v300():
            section_var += f"static const struct FunLevelKType {kernel_name}_kernel_type_section __attribute__ "
            section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
            section_var += (
                "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_AICORE} };\n"
            )
        return section_var

    def check_section_size(self):
        if self.section_size > 65535:
            err_str = (
                f"DFX section size is too large. section size is {self.section_size}."
            )
            CommonUtility.print_compile_log("", err_str, AscendCLogLevel.LOG_ERROR)
            CommonUtility().ascendc_raise_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, (err_str)
            )

    def generate_meta_info_func_section(self, tiling_key, compile_info, kernel_name):
        section_var = ""

        # deterministic
        deterministic_value = (
            1 if get_current_build_config("enable_deterministic_mode") == 1 else 0
        )
        section_var += "static const struct FuncMetaDeterministic "
        section_var += (
            f"{kernel_name}_kernel_metainfo_deterministic_section __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
        section_var += f"{{{{F_TYPE_DETERMINISTIC_INFO, 4}}, {deterministic_value}}};\n"

        # blocknum
        section_var += "static const struct FuncMetaNumBlocks "
        section_var += f"{kernel_name}_kernel_metainfo_numblocks_section __attribute__ "
        section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
        section_var += "{{F_TYPE_BLOCK_NUM_INFO, 4}, 0xFFFFFFFF};\n"

        # functionentry
        section_var += "static const struct FuncMetaFunctionEntry "
        section_var += (
            f"{kernel_name}_kernel_metainfo_functionentry_section __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{kernel_name}"))) = '
        section_var += f"{{{{F_TYPE_FUNCTION_ENTRY_INFO, 12}}, 0, {tiling_key}UL}};\n"

        return section_var

    def generate_dfx_section_for_one_tiling_key(
        self,
        tiling_key: str,
        kernel_name: str,
        compile_info: CompileInfo,
        kernel_type_section: bool = False,
    ):
        section_content = ""
        # generate kernel type tlv section
        if kernel_type_section:
            section_content += self.generate_kernel_type_section(
                compile_info, kernel_name
            )

        # generate AscendCInfoMetaDFX when tiling struct is register
        if not global_var_storage.get_variable("ascendc_tiling_no_register"):
            section_content += f"static const struct AscendCInfoMetaDFX {kernel_name}_dfx_section __attribute__ "
            section_content += f'((used, section (".ascend.meta.{kernel_name}"))) = '
            section_content += "{"
            binary_u8 = []
            for param in self.params_base:
                # binary of arg info tlv
                binary_u8.extend(
                    self._tran_dfx_info_to_uint8_t(
                        FuncMetaType.F_TYPE_L0_EXCEPTION_DFX_ARGSINFO.value,
                        ArgTypeSize.U16,
                    )
                )
                # binary of sizeof dfx info by uint64_t
                binary_u8.extend(
                    self._tran_dfx_info_to_uint8_t(
                        int(len(param.args_dfx_info) / 8), ArgTypeSize.U16
                    )
                )
                binary_u8.extend(param.args_dfx_info)
            self.section_size = len(binary_u8)
            self.check_section_size()
            # binary of dfx tlv
            section_content += f"{{{FuncMetaType.F_TYPE_L0_EXCEPTION_DFX.value}, {self.section_size}}}, "
            # generate binary for dfx value array
            section_content += "{ "
            for binary in binary_u8:
                section_content += f"{binary}, "
            section_content += "} "
            section_content += "};\n"
        if CommonUtility.is_c310():
            section_content += self.generate_meta_info_func_section(
                tiling_key, compile_info, kernel_name
            )
        return section_content

    def generate_dfx_section(
        self,
        tiling_key: str,
        tiling_info: TilingInfo,
        kernel_name: str,
        compile_info: CompileInfo,
        kernel_type_section: bool = False,
    ):
        if self.is_support is False:
            return ""

        if not tiling_info.static_shape_flag and not global_var_storage.get_variable(
            "ascendc_tiling_no_register"
        ):
            self._generate_binary_for_tiling(tiling_key, tiling_info, compile_info)

        section_content = f"// generate dfx section for tiling_key:{tiling_key}"
        if CommonUtility.is_v220() or CommonUtility.is_c310():
            if CommonUtility.is_v220():
                cube_core_marco = "(defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201)"
                vec_core_marco = "(defined(__DAV_VEC__) && __NPU_ARCH__ == 2201)"
            elif CommonUtility.is_c310():
                cube_core_marco = "(defined(__DAV_CUBE__) && __NPU_ARCH__ == 3510)"
                vec_core_marco = "(defined(__DAV_VEC__) && __NPU_ARCH__ == 3510)"
        else:
            # for v200 cube_core_type is aicore type
            cube_core_marco = "defined(__DAV_M200__)"
            vec_core_marco = "defined(__DAV_M200_VEC__)"

        section_content_body = self.generate_dfx_section_for_one_tiling_key(
            tiling_key, kernel_name, compile_info, kernel_type_section
        )

        # generate struct for dfx section
        if self.gen_dfx_struct_flag == False:
            if not global_var_storage.get_variable("ascendc_tiling_no_register"):
                section_content += self._generate_dfx_info_struct()

        if section_content_body is None or section_content_body == "":
            return section_content + "#endif\n"

        if compile_info.sub_core_type == CORE_TYPE_CUBE:
            section_content += (
                f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && {cube_core_marco}\n"
            )
        elif compile_info.sub_core_type == CORE_TYPE_VEC:
            section_content += (
                f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && {vec_core_marco}\n"
            )
        else:
            section_content += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL\n"

        section_content += section_content_body
        if compile_info.tiling_key_group_map is not None:
            if tiling_key in compile_info.tiling_key_group_map.keys():
                for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                    kernel_name_slave = (
                        get_kernel_fun_name_with_tiling_key_and_kernel_type(
                            compile_info, tiling_key_slave
                        )
                    )
                    section_content += self.generate_dfx_section_for_one_tiling_key(
                        tiling_key_slave,
                        kernel_name_slave,
                        compile_info,
                        kernel_type_section,
                    )

        section_content += "#endif\n"
        return section_content

    def _generate_dfx_info_struct(self):
        content = f"""
struct AscendCInfoMetaDFX {{
    BaseTlv head;
    uint8_t value[{self.section_size}];
}};\n\n\n
"""
        self.gen_dfx_struct_flag = True
        return content

    def generate_dfx_section_without_tiling_register(
        self,
        tiling_key: str,
        tiling_info: TilingInfo,
        tiling_key_struct_size_map: dict,
        kernel_name: str,
    ):
        if not tiling_info.static_shape_flag:
            self._generate_binary_for_tiling_without_register(
                tiling_info, tiling_key, tiling_key_struct_size_map
            )
        section_content = f"static const struct AscendCInfoMetaDFX {kernel_name}_dfx_section __attribute__ "
        section_content += f'((used, section (".ascend.meta.{kernel_name}"))) = '
        section_content += "{"
        binary_u8 = []
        for param in self.params_base:
            # binary of arg info tlv
            binary_u8.extend(
                self._tran_dfx_info_to_uint8_t(
                    FuncMetaType.F_TYPE_L0_EXCEPTION_DFX_ARGSINFO.value, ArgTypeSize.U16
                )
            )
            # binary of sizeof dfx info by uint64_t
            binary_u8.extend(
                self._tran_dfx_info_to_uint8_t(
                    int(len(param.args_dfx_info) / 8), ArgTypeSize.U16
                )
            )
            binary_u8.extend(param.args_dfx_info)
        self.section_size = len(binary_u8)
        self.check_section_size()
        # binary of dfx tlv
        section_content += (
            f"{{{FuncMetaType.F_TYPE_L0_EXCEPTION_DFX.value}, {self.section_size}}}, "
        )
        # generate binary for dfx value array
        section_content += "{ "
        for binary in binary_u8:
            section_content += f"{binary}, "
        section_content += "} "
        section_content += "};\n"
        if self.gen_dfx_struct_flag == False:
            section_content = self._generate_dfx_info_struct() + section_content
        return section_content
