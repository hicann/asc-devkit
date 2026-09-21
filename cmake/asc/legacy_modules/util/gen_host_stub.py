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

import io


def generate_host_stub_set_exception_dump_source() -> str:
    exception_dump_source = r"""
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
static void ascendc_set_exception_dump_info(uint32_t dumpSize)
{
    uint32_t atomicIndex = 0U;
    uint32_t addrNum = 1U;
    void *exceptionDumpAddr = Adx::AdumpGetSizeInfoAddr(addrNum + ASCENDC_EXCEPTION_DUMP_HEAD, atomicIndex);
    if (exceptionDumpAddr == nullptr) {
        printf("Get exceptionDumpAddr is nullptr.\n");
        return;
    }

    // atomic index
    uint64_t *sizeInfoAddr = reinterpret_cast<uint64_t *>(exceptionDumpAddr);
    *sizeInfoAddr = static_cast<uint64_t>(atomicIndex);
    sizeInfoAddr++;

    *sizeInfoAddr = static_cast<uint64_t>(1);
    sizeInfoAddr++;

    *sizeInfoAddr = dumpSize * 75;
    constexpr uint64_t workspaceOffset = (4ULL << 56ULL);
    *sizeInfoAddr |= workspaceOffset;

    const rtArgsSizeInfo sizeInfo = {exceptionDumpAddr, atomicIndex};
    int32_t ret = rtSetExceptionExtInfo(&sizeInfo);
    if (ret != 0) {
        printf("rtSetExceptionExtInfo failed, ret = %d.\n", ret);
    }
}
#endif
"""
    return exception_dump_source


def generate_host_stub_head_code(has_mix: bool, has_aic: bool, has_aiv: bool) -> str:
    """Generate host_stub.cpp head code."""

    type_nums = 0
    ascend_kernel_struct = []

    buff = io.StringIO()
    buff.write(r"""#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dlfcn.h>
#include <securec.h>
""")

    buff.write("\n")
    buff.write("""static char ascendcErrMsg[1024] = {0};
""")
    buff.write("\n")
    if has_mix:
        type_nums += 1
        buff.write("""static void *g_kernel_handle = nullptr;
""")
        buff.write("\n")

    if has_aiv:
        type_nums += 1
        buff.write("""static void *g_kernel_handle_aiv = nullptr;
""")
        buff.write("\n")

    if has_aic:
        type_nums += 1
        buff.write("""static void *g_kernel_handle_aic = nullptr;
""")
        buff.write("\n")

    buff.write(r"""struct ascend_kernels {
    uint32_t version;
    uint32_t type_cnt;
""")

    # version value
    ascend_kernel_struct.append(str(1))

    # type_cnt value
    ascend_kernel_struct.append(str(type_nums))

    if has_mix:
        buff.write("""    uint32_t mix_type;
    uint32_t mix_len;
    uint32_t mix_file_len;
    uint8_t mix_buf[__replaced_mix_len];
""")
        ascend_kernel_struct.append(str(0))
        ascend_kernel_struct.append("__replaced_mix_len")
        ascend_kernel_struct.append("__replaced_mix_file_len")
        ascend_kernel_struct.append(r"{0}")

    if has_aiv:
        buff.write("""    uint32_t aiv_type;
    uint32_t aiv_len;
    uint32_t aiv_file_len;
    uint8_t aiv_buf[__replaced_aiv_len];
""")
        ascend_kernel_struct.append(str(1))
        ascend_kernel_struct.append("__replaced_aiv_len")
        ascend_kernel_struct.append("__replaced_aiv_file_len")
        ascend_kernel_struct.append(r"{0}")

    if has_aic:
        buff.write("""    uint32_t aic_type;
    uint32_t aic_len;
    uint32_t aic_file_len;
    uint8_t aic_buf[__replaced_aic_len];
""")
        ascend_kernel_struct.append(str(2))
        ascend_kernel_struct.append("__replaced_aic_len")
        ascend_kernel_struct.append("__replaced_aic_file_len")
        ascend_kernel_struct.append(r"{0}")

    buff.write('} __replaced_ascend_kernel __attribute__ ((section ("__replaced_ascend_section"))) = {')

    ascend_kernel_str = ",".join(ascend_kernel_struct)
    buff.write(f"{ascend_kernel_str}")

    buff.write("};\n\n")

    buff.write("""extern "C" {
uint32_t RegisterAscendBinary(const char *fileBuf, size_t fileSize, uint32_t type, void **handle);
uint32_t LaunchAscendKernel(void *handle, const uint64_t key, const uint32_t numBlocks, void **args,
                            uint32_t size, const void *stream);
uint32_t GetAscendCoreSyncAddr(void **addr);
int UnregisterAscendBinary(void *hdl);
void StartAscendProf(const char *name, uint64_t *startTime);
void ReportAscendProf(const char *name, uint32_t numBlocks, uint32_t taskType, const uint64_t startTime);
bool GetAscendProfStatus();
uint32_t AllocAscendMemDevice(void **devMem, uint64_t size);
uint32_t FreeAscendMemDevice(void *devMem);
bool AscendCheckSoCVersion(const char *socVersion, char* errMsg);
void AscendProfRegister();
uint32_t GetCoreNumForMixVectorCore(uint32_t *aiCoreNum, uint32_t *vectorCoreNum);
uint32_t LaunchAscendKernelForVectorCore(const char *opType, void *handle, const uint64_t key, void **args, uint32_t size,
    const void *stream, bool enableProf, uint32_t aicNumBlocks, uint32_t aivNumBlocks, uint32_t aivNumBlocksOffset);
""")

    buff.write("""}\n\nnamespace Adx {""")

    buff.write("""
    void AdumpPrintWorkSpace(const void *workSpaceAddr, const size_t dumpWorkSpaceSize,
                            void *stream, const char *opType);
}
""")

    buff.write("""
    class KernelHandleGradUnregister {
    private:
        KernelHandleGradUnregister() {}

    public:
        KernelHandleGradUnregister(const KernelHandleGradUnregister&) = delete;
        KernelHandleGradUnregister& operator=(const KernelHandleGradUnregister&) = delete;

        static KernelHandleGradUnregister& GetInstance() {
            static KernelHandleGradUnregister instance;
            return instance;
        }
        ~KernelHandleGradUnregister(){
""")

    if has_mix:
        buff.write("""            if (g_kernel_handle) {
                UnregisterAscendBinary(g_kernel_handle);
                g_kernel_handle = nullptr;
            }
""")

    if has_aiv:
        buff.write("""            if (g_kernel_handle_aiv) {
                UnregisterAscendBinary(g_kernel_handle_aiv);
                g_kernel_handle_aiv = nullptr;
            }
""")

    if has_aic:
        buff.write("""            if (g_kernel_handle_aic) {
                UnregisterAscendBinary(g_kernel_handle_aic);
                g_kernel_handle_aic = nullptr;
            }
""")
    buff.write("""        }
    };
""")
    buff.write("\n")

    buff.write(r"""static void __register_kernels(void) __attribute__((constructor));
void __register_kernels(void)
{
    const char* compileSocVersion = "__replaced_ascend_compile_soc_version";
    uint32_t ret;
""")
    buff.write("\n")

    buff.write(r"""    bool checkSocVersion = AscendCheckSoCVersion(compileSocVersion, ascendcErrMsg);
    if (!checkSocVersion) {
        return;
    }
""")

    if has_mix:
        buff.write(r"""    ret = RegisterAscendBinary(
        (const char *)__replaced_ascend_kernel.mix_buf,
        __replaced_ascend_kernel.mix_file_len,
        0,
        &g_kernel_handle);
    if (ret != 0) {
        printf("RegisterAscendBinary mix ret %u \n", ret);
    }
""")
        buff.write("\n")

    if has_aiv:
        buff.write(r"""    ret = RegisterAscendBinary(
        (const char *)__replaced_ascend_kernel.aiv_buf,
        __replaced_ascend_kernel.aiv_file_len,
        1,
        &g_kernel_handle_aiv);
    if (ret != 0) {
        printf("RegisterAscendBinary aiv ret %u \n", ret);
    }
""")
        buff.write("\n")

    if has_aic:
        buff.write(r"""    ret = RegisterAscendBinary(
        (const char *)__replaced_ascend_kernel.aic_buf,
        __replaced_ascend_kernel.aic_file_len,
        2,
        &g_kernel_handle_aic);
    if (ret != 0) {
        printf("RegisterAscendBinary aic ret %u \n", ret);
    }
""")
        buff.write("\n")

    buff.write("""    AscendProfRegister();
}
""")

    return buff.getvalue()
