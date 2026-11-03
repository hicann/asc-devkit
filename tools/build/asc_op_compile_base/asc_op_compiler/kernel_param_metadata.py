# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Generate Runtime parameter metadata for ordinary GM_ADDR kernel entries."""

import logging
import re
import struct
from typing import NamedTuple

from .ascendc_compile_dfx import FuncMetaType


class KernelParamMetadata(NamedTuple):
    """Entry-independent summary and parameter records, prepared once."""

    count: int
    total_size: int
    infos: tuple


def log_param_metadata_failure(error):
    """A supplement failure must not change the existing compiler failure path."""
    try:
        logging.getLogger(__name__).warning("Parameter metadata was not generated: %s", error)
    except Exception:
        pass


def build_kernel_param_metadata_from_formals(parameters):
    """Build the immutable pointer layout from the ordinary wrapper's final formals."""
    try:
        if parameters is None:
            return None
        count = len(parameters)
        if count > 0xFFFFFFFF // 8:
            raise ValueError("parameter block exceeds the Runtime offset range")
        # The caller generates GM_ADDR formals; additional qualifiers do not change their pointer layout.
        # INFO field order: index, ordinal, offset, size, align, paramType,
        #                   spaceIndex, spaceType, space, resv.
        # ordinal is zero-based; offset = ordinal * 8 bytes; size = align = 8 bytes.
        # paramType = 2 denotes a pointer; index and the last four fields are reserved zeros.
        infos = tuple((0, ordinal, ordinal * 8, 8, 8, 2, 0, 0, 0, 0) for ordinal in range(count))
        return KernelParamMetadata(count, count * 8, infos)
    except Exception as error:
        log_param_metadata_failure(error)
        return None


def generate_param_metadata(entry, description):
    """Return a complete declaration, or no supplement if generation fails.

    The caller supplies the numeric layout prepared from the final formals.
    TLVs describe 8-byte pointer slots, not the pointed-to data.
    """
    try:
        if description is None:
            return ""
        return _generate_param_metadata(entry, description)
    except Exception as error:
        log_param_metadata_failure(error)
        return ""


def _serialize_param_metadata(description):
    """Encode each TLV separately, preserving record boundaries for source generation."""
    # Payload lengths exclude the four-byte TLV header: SUMMARY=12, INFO=36.
    # Explicit little-endian encoding avoids C++ structure padding and host byte order.
    records = [
        struct.pack("<HHIQ", FuncMetaType.F_TYPE_PARAM_SUMMARY.value, 12, description.count, description.total_size)
    ]
    records.extend(
        struct.pack("<HHIIIIHHIIII", FuncMetaType.F_TYPE_PARAM_INFO.value, 36, *parameter)
        for parameter in description.infos
    )
    return records


def _generate_param_metadata(entry, description):
    if not re.fullmatch(r"[A-Za-z_][A-Za-z_0-9]*", entry):
        raise ValueError("invalid parameter metadata entry name")
    records = _serialize_param_metadata(description)
    # Keep each TLV on its own line so generated source remains easy to inspect.
    values = "\n".join("    " + ", ".join(f"0x{value:02x}" for value in record) + "," for record in records)
    # The existing compiler command renames the main entry with -D. Expand it before stringifying.
    return f"""
#ifndef ASCENDC_PARAM_STRINGIFY
#define ASCENDC_PARAM_STRINGIFY_IMPL(name) #name
#define ASCENDC_PARAM_STRINGIFY(name) ASCENDC_PARAM_STRINGIFY_IMPL(name)
#endif
#ifndef ASCENDC_PARAM_METADATA_{entry}
#define ASCENDC_PARAM_METADATA_{entry}
static const unsigned char g_ascendc_param_metadata_{entry}[]
    __attribute__((used, section(".ascend.meta." ASCENDC_PARAM_STRINGIFY({entry})), aligned(4))) = {{
{values}
}};
#endif
"""
