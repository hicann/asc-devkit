/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/compilation_plan_executor.h"
#include "specialization/kernel_specialization_diagnostics.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <elf.h>
#include <fstream>
#include <sys/stat.h>

namespace {
namespace fs = boost::filesystem;
using namespace ascendc::aclrtc;

class KernelRenameWorkspace {
public:
    KernelRenameWorkspace() : root(fs::temp_directory_path() / fs::unique_path("aclrtc_rename_%%%%-%%%%"))
    {
        fs::create_directories(root);
        std::ofstream script((root / "objcopy").string());
        script << "#!/bin/sh\nprintf '%s\\n' \"$@\" > '" << (root / "arguments").string() << "'\n";
        script.close();
        chmod((root / "objcopy").c_str(), 0700);
    }

    ~KernelRenameWorkspace()
    {
        boost::system::error_code ignored;
        fs::remove_all(root, ignored);
    }

    void WriteElf(
        const std::vector<std::string>& metadataSections, const std::vector<std::string>& functionNames = {"add"},
        uint32_t symbolTableType = SHT_SYMTAB, unsigned char symbolType = STT_FUNC, uint16_t symbolSection = 1)
    {
        Elf64_Ehdr header{};
        std::memcpy(header.e_ident, ELFMAG, SELFMAG);
        header.e_ident[EI_CLASS] = ELFCLASS64;
        header.e_ident[EI_DATA] = ELFDATA2LSB;
        header.e_shoff = sizeof(header);
        header.e_shentsize = sizeof(Elf64_Shdr);
        header.e_shnum = 4 + metadataSections.size();
        header.e_shstrndx = 1;
        std::vector<Elf64_Shdr> sections(header.e_shnum);
        std::string sectionNames(1, '\0');
        for (size_t index = 0; index < metadataSections.size(); ++index) {
            sections[2 + index].sh_name = sectionNames.size();
            sections[2 + index].sh_type = SHT_PROGBITS;
            sectionNames += metadataSections[index] + '\0';
        }
        std::string symbolNames(1, '\0');
        std::vector<Elf64_Sym> symbols(1);
        for (const auto& name : functionNames) {
            Elf64_Sym symbol{};
            symbol.st_name = symbolNames.size();
            symbol.st_info = ELF64_ST_INFO(STB_GLOBAL, symbolType);
            symbol.st_shndx = symbolSection;
            symbols.push_back(symbol);
            symbolNames += name + '\0';
        }
        sections[1].sh_type = SHT_STRTAB;
        sections[1].sh_offset = sizeof(header) + sections.size() * sizeof(Elf64_Shdr);
        sections[1].sh_size = sectionNames.size();
        auto& symbolStrings = sections[sections.size() - 2];
        symbolStrings.sh_type = SHT_STRTAB;
        symbolStrings.sh_offset = sections[1].sh_offset + sectionNames.size();
        symbolStrings.sh_size = symbolNames.size();
        auto& symbolTable = sections.back();
        symbolTable.sh_type = symbolTableType;
        symbolTable.sh_offset = symbolStrings.sh_offset + symbolNames.size();
        symbolTable.sh_size = symbols.size() * sizeof(Elf64_Sym);
        symbolTable.sh_entsize = sizeof(Elf64_Sym);
        symbolTable.sh_link = sections.size() - 2;
        std::ofstream output((root / "input.elf").string(), std::ios::binary);
        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        output.write(reinterpret_cast<const char*>(sections.data()), sections.size() * sizeof(Elf64_Shdr));
        output.write(sectionNames.data(), sectionNames.size());
        output.write(symbolNames.data(), symbolNames.size());
        output.write(reinterpret_cast<const char*>(symbols.data()), symbols.size() * sizeof(Elf64_Sym));
    }

    aclError Execute()
    {
        KernelSpecializationDiagnostics diagnostics(fs::path(), "rename_test");
        CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
        KernelCompilationPlan plan;
        // The file to inspect only exists after the preceding link command runs.
        CompilationCommand link{};
        link.commandKind = CompilationCommandKind::Link;
        link.diagnosticLabel = "link";
        link.executablePath = "/bin/cp";
        link.arguments = {(root / "input.elf").string(), (root / "linked.elf").string()};
        plan.compilationCommands.push_back(link);
        CompilationCommand rename{};
        rename.commandKind = CompilationCommandKind::ObjectCopy;
        rename.diagnosticLabel = "rename_output_kernel";
        rename.executablePath = root / "objcopy";
        rename.kernelSymbolRename = KernelSymbolRename{"add", "specialized"};
        rename.arguments = {(root / "linked.elf").string()};
        plan.compilationCommands.push_back(rename);
        return executor.ExecuteCompilationPlan(plan);
    }

    std::string Arguments() const
    {
        std::ifstream input((root / "arguments").string());
        return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    }

    fs::path root;
};

TEST(KernelSymbolRenamingTest, RenamesOnlyExistingFunctionsAndMetadataSections)
{
    const std::vector<std::vector<std::string>> cases{
        {},
        {".ascend.meta.add"},
        {".ascend.meta.add_mix_aic", ".ascend.meta.add_mix_aiv"},
        {".ascend.meta.add_mix_aic", ".ascend.meta.unrelated"}};
    const std::vector<std::vector<std::string>> functions{
        {"add"}, {"add"}, {"add_mix_aic", "add_mix_aiv"}, {"add_mix_aic"}};
    for (size_t index = 0; index < cases.size(); ++index) {
        const auto& metadata = cases[index];
        KernelRenameWorkspace workspace;
        workspace.WriteElf(metadata, functions[index]);
        ASSERT_EQ(workspace.Execute(), ACLRTC_SUCCESS);
        std::string expected;
        for (const char* suffix : {"", "_mix_aic", "_mix_aiv"}) {
            if (std::find(functions[index].begin(), functions[index].end(), "add" + std::string(suffix)) ==
                functions[index].end()) {
                continue;
            }
            expected += "--redefine-sym=add" + std::string(suffix) + "=specialized" + suffix + "\n";
            const std::string section = ".ascend.meta.add" + std::string(suffix);
            if (std::find(metadata.begin(), metadata.end(), section) != metadata.end()) {
                expected += "--rename-section=" + section + "=.ascend.meta.specialized" + suffix + "\n";
            }
        }
        expected += (workspace.root / "linked.elf").string() + "\n";
        EXPECT_EQ(workspace.Arguments(), expected);
    }
}

TEST(KernelSymbolRenamingTest, RejectsMissingOrUndefinedKernelFunctionsBeforeObjcopy)
{
    for (const auto& names : std::vector<std::vector<std::string>>{{}, {"unrelated"}, {"add_sk"}}) {
        KernelRenameWorkspace workspace;
        workspace.WriteElf({".ascend.meta.add"}, names);
        EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
        EXPECT_FALSE(fs::exists(workspace.root / "arguments"));
    }
    for (const auto symbolType : {STT_FUNC, STT_OBJECT}) {
        KernelRenameWorkspace workspace;
        workspace.WriteElf(
            {".ascend.meta.add"}, {"add"}, SHT_SYMTAB, symbolType, symbolType == STT_FUNC ? SHN_UNDEF : 1);
        EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
        EXPECT_FALSE(fs::exists(workspace.root / "arguments"));
    }
}

TEST(KernelSymbolRenamingTest, ReadsDynamicSymbolTableAndPreservesSuperKernelNames)
{
    KernelRenameWorkspace workspace;
    workspace.WriteElf(
        {".ascend.meta.add", ".ascend.meta.add_sk"},
        {"add", "add_sk", "add_sk_split1", "add_sk_split2", "add_sk_split3"}, SHT_DYNSYM);
    ASSERT_EQ(workspace.Execute(), ACLRTC_SUCCESS);
    EXPECT_EQ(
        workspace.Arguments(), "--redefine-sym=add=specialized\n"
                               "--rename-section=.ascend.meta.add=.ascend.meta.specialized\n" +
                                   (workspace.root / "linked.elf").string() + "\n");
}

TEST(KernelSymbolRenamingTest, RejectsMalformedSymbolTablesBeforeObjcopy)
{
    for (const auto field :
         {offsetof(Elf64_Shdr, sh_entsize), offsetof(Elf64_Shdr, sh_link), offsetof(Elf64_Shdr, sh_offset),
          offsetof(Elf64_Shdr, sh_size)}) {
        SCOPED_TRACE(field);
        KernelRenameWorkspace workspace;
        workspace.WriteElf({});
        std::fstream elf((workspace.root / "input.elf").string(), std::ios::binary | std::ios::in | std::ios::out);
        elf.seekp(sizeof(Elf64_Ehdr) + 3U * sizeof(Elf64_Shdr) + field);
        const uint64_t invalid = UINT64_MAX;
        elf.write(reinterpret_cast<const char*>(&invalid), sizeof(invalid));
        elf.close();
        EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
        EXPECT_FALSE(fs::exists(workspace.root / "arguments"));
    }
    KernelRenameWorkspace workspace;
    workspace.WriteElf({}, {"add"}, SHT_NULL);
    EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
    EXPECT_FALSE(fs::exists(workspace.root / "arguments"));
}

TEST(KernelSymbolRenamingTest, RejectsMalformedElfBeforeObjcopy)
{
    KernelRenameWorkspace workspace;
    std::ofstream((workspace.root / "input.elf").string()) << "not an ELF";
    EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
    EXPECT_FALSE(fs::exists(workspace.root / "arguments"));

    workspace.WriteElf({".ascend.meta.add"});
    std::fstream invalidElf((workspace.root / "input.elf").string(), std::ios::binary | std::ios::in | std::ios::out);
    const uint16_t invalidStringTableIndex = SHN_XINDEX;
    invalidElf.seekp(offsetof(Elf64_Ehdr, e_shstrndx));
    invalidElf.write(reinterpret_cast<const char*>(&invalidStringTableIndex), sizeof(invalidStringTableIndex));
    invalidElf.close();
    EXPECT_EQ(workspace.Execute(), ACLRTC_ERROR_COMPILATION);
    EXPECT_FALSE(fs::exists(workspace.root / "arguments"));
}
} // namespace
