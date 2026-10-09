/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file elf_tool.c
 * \brief
 */

#include "ascendc_elf_tool.h"

// Check whether the elf file is valid.
#if !defined(UT_TEST) && !defined(ST_TEST)
static int32_t ElfHeaderCheck(uint8_t* elf, size_t elfSize, bool checkProgHeader)
#else
int32_t ElfHeaderCheck(uint8_t* elf, size_t elfSize, bool checkProgHeader)
#endif
{
    if (elf == NULL) {
        printf("[Error] input elf buffer is NULL.\n");
        return ELF_ERR_NULL_POINTER;
    }
    // check elf min size
    if (elfSize < sizeof(Elf_Ehdr)) {
        printf("[Error] elf file size %zu is too small!\n", elfSize);
        return ELF_ERR_BUFFER_TOO_SMALL;
    }

    if (elf[EI_CLASS] != ELFCLASS64) {
        printf("[Error] elf CLASS %u\n", elf[EI_CLASS]);
        return ELF_ERR_NOT_64BIT;
    }

    // check keyword e_phoff
    if (!checkProgHeader) {
        if (((Elf_Ehdr*)(elf))->e_phoff != 0) {
            printf("[Error] Contain Program header!\n");
            return ELF_ERR_UNEXPECTED_PROG_HEADER;
        }
    }
    return ELF_SUCCESS;
}

size_t ElfAddSection(
    uint8_t* elf, size_t elfSize, uint8_t* jit, size_t jitSize, uint8_t* sec, size_t secSize, uint32_t type)
{
    ElfHeaderCheck(elf, elfSize, false);
    Elf_Ehdr* eh = (Elf_Ehdr*)elf;
    errno_t err = memcpy_s(jit, jitSize, elf, elfSize);
    CHECK_COND_AND_DO((err != EOK), {
        printf(
            "[Error] 1.ElfAddSection: jit size %lu, jit residual size %lu, entry size %u exceeded the maximum "
            "capacity!\n",
            jitSize, jitSize, eh->e_ehsize);
        return 0;
    });

    char* sec_header_tab = (char*)(jit + eh->e_shoff);
    Elf_Shdr* sh_str_tab = NULL;
    char* sh_strtbl = (char*)(((Elf_Shdr*)(sec_header_tab + eh->e_shentsize * eh->e_shstrndx))->sh_offset + jit);
    for (int i = 0; i < eh->e_shnum; i++) {
        Elf_Shdr* sh = (Elf_Shdr*)(sec_header_tab + eh->e_shentsize * i);
        char* sh_name = sh_strtbl + sh->sh_name;
        if (sh->sh_type == SHT_NOBITS) {
            continue;
        }
        if (strncmp(sh_name, ".ascend.kernel", 0xE) == 0) {
            sh_str_tab = sh;
            break;
        }
    }
    if (sh_str_tab == NULL) {
        return 0;
    }
    uint8_t* sectionStartAddr = (uint8_t*)(jit + sh_str_tab->sh_offset);
    struct AscendKernelHeader* kernelHeader = (struct AscendKernelHeader*)sectionStartAddr;
    CHECK_COND_AND_DO((kernelHeader->version != 1), {
        printf("[Error] ascend_kernel verision %u is err\n", kernelHeader->version);
        return 0;
    })
    uint32_t typeCount = kernelHeader->typeCnt;
    uint8_t* beginAddr = (uint8_t*)(sectionStartAddr + sizeof(struct AscendKernelHeader));
    uint32_t secType;
    uint32_t secLen;
    uint32_t fileLen;
    for (uint32_t i = 0; i < typeCount; i++) {
        secType = *((uint32_t*)beginAddr);
        secLen = *((uint32_t*)beginAddr + ASCEND_KERNEL_SECTION_LEN_POS);
        if (secType == type) {
            fileLen = *((uint32_t*)beginAddr + ASCEND_KERNEL_FILE_LEN_POS);
            CHECK_COND_AND_DO((fileLen < secSize), {
                printf("[Error] ascend_kernel fileLen %u is less than actual size %lu\n", fileLen, secSize);
                return 0;
            });
            CHECK_COND_AND_DO((secLen < secSize), {
                printf("[Error] ascend_kernel fileLen %u is less than secLen %lu\n", fileLen, secSize);
                return 0;
            });
            err = memcpy_s(beginAddr + sizeof(uint32_t) * ASCEND_KERNEL_HEADER_CNT, secLen, sec, secSize);
            CHECK_COND_AND_DO((err != EOK), {
                printf("[Error] memcpy ascend_kernel file err\n");
                return 0;
            });
        } else {
            beginAddr = beginAddr + sizeof(uint32_t) * ASCEND_KERNEL_HEADER_CNT + sizeof(uint8_t) * secLen;
        }
    }
    return elfSize;
}

int32_t ElfGetSymbolOffset(uint8_t* elf, size_t elfSize, const char* symbolName, size_t* offset, size_t* size)
{
    if (ElfHeaderCheck(elf, elfSize, true) != ELF_SUCCESS) {
        return ELF_NO_TABLE;
    }
    size_t symbolNameLen = strnlen(symbolName, MAX_SYMNAME_LEN);
    Elf_Ehdr* eh = (Elf_Ehdr*)elf;
    char* sec_header_tab = (char*)(elf + eh->e_shoff);
    Elf_Shdr* sh_sym_tab = NULL;
    Elf_Shdr* sh_str_tab = NULL;
    char* sh_strtbl = (char*)(((Elf_Shdr*)(sec_header_tab + eh->e_shentsize * eh->e_shstrndx))->sh_offset + elf);
    for (int i = 0; i < eh->e_shnum; i++) {
        Elf_Shdr* sh = (Elf_Shdr*)(sec_header_tab + eh->e_shentsize * i);
        char* sh_name = sh_strtbl + sh->sh_name;
        if (sh->sh_type == SHT_NOBITS) {
            continue;
        }
        if (strncmp(sh_name, ".symtab", 0x8) == 0) {
            sh_sym_tab = sh;
        } else if (strncmp(sh_name, ".strtab", 0x8) == 0) {
            sh_str_tab = sh;
        }
    }
    if (sh_sym_tab == NULL || sh_str_tab == NULL) {
        return ELF_NO_TABLE;
    }
    Elf64_Sym* sym_tab = (Elf64_Sym*)(elf + sh_sym_tab->sh_offset);
    char* str_tab_addr = (char*)(elf + sh_str_tab->sh_offset);
    int symbol_count = (int)(sh_sym_tab->sh_size / sizeof(Elf64_Sym));
    for (int i = 0; i < symbol_count; i++) {
        if (strncmp(sym_tab[i].st_name + str_tab_addr, symbolName, symbolNameLen + 1) == 0) {
            Elf_Shdr* sh = (Elf_Shdr*)(sec_header_tab + eh->e_shentsize * sym_tab[i].st_shndx);
            *offset = sym_tab[i].st_value - sh->sh_addr + sh->sh_offset;
            *size = sym_tab[i].st_size;
            return ELF_SUCCESS;
        }
    }
    return ELF_NO_SYMBOL;
}
