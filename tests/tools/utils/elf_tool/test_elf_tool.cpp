/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <cmath>
#include <fstream>
#include <stdlib.h>
#include "securec.h"
#include <fcntl.h>
#include <ctime>
#include <cstring>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>
#include "ascendc_elf_tool.h"
#include "mockcpp/mockcpp.hpp"

namespace {
constexpr size_t kTextOffset = sizeof(Elf_Ehdr);
constexpr size_t kTextSize = 16;
constexpr size_t kSymbolOffsetInText = 4;
constexpr size_t kSymbolSize = 8;
constexpr size_t kSymtabOffset = kTextOffset + kTextSize;
constexpr size_t kStrtabOffset = kSymtabOffset + sizeof(Elf64_Sym);
constexpr size_t kStrtabSize = 32;
constexpr size_t kShstrtabOffset = kStrtabOffset + kStrtabSize;
constexpr size_t kShstrtabSize = 64;
constexpr size_t kSectionHeaderOffset = kShstrtabOffset + kShstrtabSize;
constexpr size_t kSectionNum = 5;
constexpr Elf_Addr kTextAddr = 0x1000;

std::vector<uint8_t> BuildElfWithSymbol()
{
    const char strtab[] = "\0target_symbol\0";
    const char shstrtab[] = "\0.text\0.symtab\0.strtab\0.shstrtab\0";
    std::vector<uint8_t> elf(kSectionHeaderOffset + kSectionNum * sizeof(Elf_Shdr), 0);

    auto* eh = reinterpret_cast<Elf_Ehdr*>(elf.data());
    eh->e_ident[EI_CLASS] = ELFCLASS64;
    eh->e_shoff = kSectionHeaderOffset;
    eh->e_shentsize = sizeof(Elf_Shdr);
    eh->e_shnum = kSectionNum;
    eh->e_shstrndx = 4;

    auto* sym = reinterpret_cast<Elf64_Sym*>(elf.data() + kSymtabOffset);
    sym->st_name = 1;
    sym->st_shndx = 1;
    sym->st_value = kTextAddr + kSymbolOffsetInText;
    sym->st_size = kSymbolSize;

    (void)memcpy_s(elf.data() + kStrtabOffset, kStrtabSize, strtab, sizeof(strtab));
    (void)memcpy_s(elf.data() + kShstrtabOffset, kShstrtabSize, shstrtab, sizeof(shstrtab));

    auto* sh = reinterpret_cast<Elf_Shdr*>(elf.data() + eh->e_shoff);
    sh[1].sh_name = 1;
    sh[1].sh_type = 1;
    sh[1].sh_addr = kTextAddr;
    sh[1].sh_offset = kTextOffset;
    sh[1].sh_size = kTextSize;

    sh[2].sh_name = 7;
    sh[2].sh_type = 2;
    sh[2].sh_offset = kSymtabOffset;
    sh[2].sh_size = sizeof(Elf64_Sym);

    sh[3].sh_name = 15;
    sh[3].sh_type = 3;
    sh[3].sh_offset = kStrtabOffset;
    sh[3].sh_size = sizeof(strtab);

    sh[4].sh_name = 23;
    sh[4].sh_type = 3;
    sh[4].sh_offset = kShstrtabOffset;
    sh[4].sh_size = sizeof(shstrtab);
    return elf;
}
} // namespace

std::string GetElfString()
{
    std::ifstream file("../../../../tests/tools/utils/elf_tool/elf_tools.txt");
    std::stringstream buffer;
    std::string content;
    if (!file) {
        std::cerr << "Cannot open elf_tools.txt\n";
        return "";
    }

    buffer << file.rdbuf();
    content = buffer.str();

    file.close();
    return content;
}

class TEST_ELFTOOL : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

void GetElfBuffer(const char* str, char* buf, int& len)
{
    char b2c[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char c2b[256];
    for (int i = 0; i < 16; i++) {
        c2b[b2c[i]] = i;
    }
    int nlen = strlen(str);
    for (int i = 0; i < nlen; i += 2) {
        buf[i / 2] = c2b[str[i]];
        buf[i / 2] |= c2b[str[i + 1]] << 4;
    }
    len = nlen / 2;
}

TEST_F(TEST_ELFTOOL, elfHeaderCheckClassAndPHFail)
{
    char elf[618433];
    char jit[618433];
    char sec[618433];
    int len;
    std::string elf_res_str = GetElfString();
    const char* elf_tools_test = elf_res_str.c_str();
    GetElfBuffer(elf_tools_test, elf, len);
    elf[EI_CLASS] = 0;
    size_t elfSize = 618433;
    uint32_t type = 1;
    size_t kernelFileLen = 80088;

    size_t ret = ElfAddSection((uint8_t*)elf, elfSize, (uint8_t*)jit, elfSize, (uint8_t*)sec, kernelFileLen, type);

    EXPECT_EQ(ret, elfSize);

    elf[EI_CLASS] = ELFCLASS64;
    ((Elf_Ehdr*)elf)->e_phoff = 1;

    ret = ElfAddSection((uint8_t*)elf, elfSize, (uint8_t*)jit, elfSize, (uint8_t*)sec, kernelFileLen, type);
    EXPECT_EQ(ret, elfSize);
}

TEST_F(TEST_ELFTOOL, elfHeaderCheckSizeTooSmall)
{
    char elf[618433];
    char jit[618433];
    char sec[618433];
    int len;
    std::string elf_res_str = GetElfString();
    const char* elf_tools_test = elf_res_str.c_str();
    GetElfBuffer(elf_tools_test, elf, len);
    size_t fileLen = 618433;
    size_t elfSize = 4;
    uint32_t type = 1;
    size_t ret = ElfAddSection((uint8_t*)elf, elfSize, (uint8_t*)jit, fileLen, (uint8_t*)sec, fileLen, type);
    EXPECT_EQ(ret, 0);
}

TEST_F(TEST_ELFTOOL, elfHeaderCheckDirectErrorBranches)
{
    uint8_t elf[sizeof(Elf_Ehdr)] = {0};
    EXPECT_EQ(ElfHeaderCheck(elf, EI_CLASS, true), ELF_ERR_BUFFER_TOO_SMALL);

    elf[EI_CLASS] = ELFCLASS64;
    auto* eh = reinterpret_cast<Elf_Ehdr*>(elf);
    eh->e_phoff = 1;
    EXPECT_EQ(ElfHeaderCheck(elf, sizeof(elf), false), ELF_ERR_UNEXPECTED_PROG_HEADER);
    EXPECT_EQ(ElfHeaderCheck(elf, sizeof(elf), true), ELF_SUCCESS);
}

TEST_F(TEST_ELFTOOL, elfAddSectionTest)
{
    char elf[618433];
    char jit[618433];
    char sec[618433];
    int len;
    std::string elf_res_str = GetElfString();
    const char* elf_tools_test = elf_res_str.c_str();
    GetElfBuffer(elf_tools_test, elf, len);
    size_t fileLen = 618433;
    size_t kernelFileLen = 80088;
    uint32_t type = 1;
    size_t ret = ElfAddSection((uint8_t*)elf, fileLen, (uint8_t*)jit, fileLen, (uint8_t*)sec, kernelFileLen, type);
    EXPECT_EQ(ret, fileLen);
}

TEST_F(TEST_ELFTOOL, elfGetSymbolOffsetNoTable)
{
    const size_t buf_size = sizeof(Elf_Ehdr) + 2 * sizeof(Elf_Shdr) + 20;
    uint8_t elf[buf_size] = {0};
    memcpy(
        elf,
        "\x7F"
        "ELF",
        4);
    Elf_Ehdr* eh = (Elf_Ehdr*)elf;
    eh->e_ident[EI_CLASS] = ELFCLASS32;
    eh->e_ident[EI_DATA] = ELFDATA2LSB;
    eh->e_type = ET_EXEC;
    eh->e_machine = EM_386;
    eh->e_version = 1;
    eh->e_shoff = sizeof(Elf_Ehdr);
    eh->e_shentsize = sizeof(Elf_Ehdr);
    eh->e_shnum = 2;
    eh->e_shstrndx = 1;

    Elf_Shdr* sh = (Elf_Shdr*)(elf + eh->e_shoff);
    memset(sh, 0, sizeof(Elf_Shdr));
    sh++;
    sh->sh_name = 1;
    sh->sh_type = 3; /* String table */
    sh->sh_offset = eh->e_shoff + 2 * sizeof(Elf_Shdr);
    sh->sh_size = 20;

    char* shstrtab = (char*)(elf + sh->sh_offset);
    strcpy(shstrtab, "\0.shstrtab\0");

    size_t offset;
    size_t size;
    size_t ret = ElfGetSymbolOffset((uint8_t*)elf, sizeof(elf), "testName", &offset, &size);
    EXPECT_EQ(ret, ELF_NO_TABLE);
}

TEST_F(TEST_ELFTOOL, elfGetSymbolOffsetSuccess)
{
    std::vector<uint8_t> elf = BuildElfWithSymbol();
    size_t offset = 0;
    size_t size = 0;
    size_t ret = ElfGetSymbolOffset(elf.data(), elf.size(), "target_symbol", &offset, &size);

    EXPECT_EQ(ret, ELF_SUCCESS);
    EXPECT_EQ(offset, kTextOffset + kSymbolOffsetInText);
    EXPECT_EQ(size, kSymbolSize);
}

TEST_F(TEST_ELFTOOL, elfGetSymbolOffsetNullPtr)
{
    size_t offset;
    size_t size;
    size_t ret = ElfGetSymbolOffset(nullptr, 8, "testName", &offset, &size);
    EXPECT_EQ(ret, ELF_NO_TABLE);
}

TEST_F(TEST_ELFTOOL, elfGetSymbolOffsetTruncatedHeader)
{
    uint8_t elf[EI_CLASS + 1U] = {0};
    elf[EI_CLASS] = ELFCLASS64;
    size_t offset = 0;
    size_t size = 0;

    EXPECT_EQ(ElfGetSymbolOffset(elf, sizeof(elf), "g_aicpuDumpConfig", &offset, &size), ELF_NO_TABLE);
}

TEST_F(TEST_ELFTOOL, elfGetSymbolOffsetNoSymbol)
{
    char elf[618433];
    char sec[618433];
    int len;
    std::string elf_res_str = GetElfString();
    const char* elf_tools_test = elf_res_str.c_str();
    GetElfBuffer(elf_tools_test, elf, len);
    size_t fileLen = 618433;
    uint32_t type = 1;
    size_t offset;
    size_t size;
    size_t ret = ElfGetSymbolOffset((uint8_t*)elf, fileLen, "testName", &offset, &size);
    EXPECT_EQ(ret, ELF_NO_SYMBOL);
}

TEST_F(TEST_ELFTOOL, ElfHeaderCheckNullptrTest)
{
    int32_t ret = ElfHeaderCheck(nullptr, 1, false);
    EXPECT_EQ(ret, 1);
}
