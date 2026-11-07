/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include "compilation_plan_executor.h"

#include "ascendc_tool_log.h"
#include "file_utils.h"
#include "kernel_specialization_diagnostics.h"
#include "process_executor.h"
#include "securec.h"

#include <algorithm>
#include <elf.h>
#include <exception>
#include <limits>
#include <new>
#include <optional>
#include <set>

namespace ascendc {
namespace aclrtc {

namespace {
bool ContainsElfRange(const std::vector<uint8_t>& elf, uint64_t offset, uint64_t size)
{
    return offset <= elf.size() && size <= elf.size() - offset;
}

template <typename T>
bool ReadElfRecord(const std::vector<uint8_t>& elf, uint64_t offset, T& record)
{
    if (!ContainsElfRange(elf, offset, sizeof(T))) {
        ASCENDLOGE(
            "ELF record is outside the file: offset=%llu record_size=%zu file_size=%zu",
            static_cast<unsigned long long>(offset), sizeof(T), elf.size());
        return false;
    }
    const errno_t result = memcpy_s(&record, sizeof(T), elf.data() + offset, sizeof(T));
    if (result != EOK) {
        ASCENDLOGE(
            "Failed to copy ELF record: offset=%llu record_size=%zu result=%d", static_cast<unsigned long long>(offset),
            sizeof(T), result);
        return false;
    }
    return true;
}

bool ReadElfName(
    const std::vector<uint8_t>& elf, const Elf64_Shdr& strings, uint32_t offset, std::set<std::string>& names)
{
    if (offset >= strings.sh_size) {
        ASCENDLOGE(
            "ELF name offset is outside the string table: name_offset=%u table_offset=%llu table_size=%llu", offset,
            static_cast<unsigned long long>(strings.sh_offset), static_cast<unsigned long long>(strings.sh_size));
        return false;
    }
    const auto* name = reinterpret_cast<const char*>(elf.data() + strings.sh_offset + offset);
    const auto* limit = name + (strings.sh_size - offset);
    const auto* end = std::find(name, limit, '\0');
    if (end == limit) {
        ASCENDLOGE(
            "ELF name is not NUL-terminated within the string table: name_offset=%u table_offset=%llu table_size=%llu",
            offset, static_cast<unsigned long long>(strings.sh_offset),
            static_cast<unsigned long long>(strings.sh_size));
        return false;
    }
    names.emplace(name, end);
    return true;
}

bool ReadElfStringTableHeader(
    const std::vector<uint8_t>& elf, const Elf64_Ehdr& header, uint32_t index, const char* nameKind,
    Elf64_Shdr& strings)
{
    if (index >= header.e_shnum) {
        ASCENDLOGE(
            "Invalid ELF %s string table index: table_index=%u section_count=%u", nameKind, index,
            static_cast<unsigned int>(header.e_shnum));
        return false;
    }
    if (!ReadElfRecord(elf, header.e_shoff + index * sizeof(Elf64_Shdr), strings)) {
        return false;
    }
    if (strings.sh_type != SHT_STRTAB || !ContainsElfRange(elf, strings.sh_offset, strings.sh_size)) {
        ASCENDLOGE(
            "Invalid ELF %s string table: table_index=%u table_type=%u table_offset=%llu table_size=%llu", nameKind,
            index, strings.sh_type, static_cast<unsigned long long>(strings.sh_offset),
            static_cast<unsigned long long>(strings.sh_size));
        return false;
    }
    return true;
}

bool ReadDefinedFunctionNames(
    const std::vector<uint8_t>& elf, const Elf64_Ehdr& header, const Elf64_Shdr& symbols,
    std::set<std::string>& functionNames)
{
    if (symbols.sh_entsize != sizeof(Elf64_Sym) || symbols.sh_size % sizeof(Elf64_Sym) != 0U ||
        !ContainsElfRange(elf, symbols.sh_offset, symbols.sh_size)) {
        ASCENDLOGE(
            "Invalid ELF symbol table: offset=%llu size=%llu entry_size=%llu",
            static_cast<unsigned long long>(symbols.sh_offset), static_cast<unsigned long long>(symbols.sh_size),
            static_cast<unsigned long long>(symbols.sh_entsize));
        return false;
    }
    Elf64_Shdr strings{};
    if (!ReadElfStringTableHeader(elf, header, symbols.sh_link, "symbol-name", strings)) {
        return false;
    }
    for (uint64_t offset = 0; offset < symbols.sh_size; offset += sizeof(Elf64_Sym)) {
        Elf64_Sym symbol{};
        if (!ReadElfRecord(elf, symbols.sh_offset + offset, symbol)) {
            return false;
        }
        if (ELF64_ST_TYPE(symbol.st_info) != STT_FUNC || symbol.st_shndx == SHN_UNDEF) {
            continue;
        }
        if (!ReadElfName(elf, strings, symbol.st_name, functionNames)) {
            return false;
        }
    }
    return true;
}

bool ReadElfNames(const std::string& path, std::set<std::string>& sectionNames, std::set<std::string>& functionNames)
{
    std::vector<uint8_t> elf;
    if (!FileUtils::ReadRegularFile(path, std::numeric_limits<size_t>::max(), elf)) {
        return false;
    }
    Elf64_Ehdr header{};
    if (!ReadElfRecord(elf, 0, header)) {
        return false;
    }
    if (!std::equal(ELFMAG, ELFMAG + SELFMAG, header.e_ident) || header.e_ident[EI_CLASS] != ELFCLASS64 ||
        header.e_ident[EI_DATA] != ELFDATA2LSB) {
        ASCENDLOGE(
            "Invalid ELF magic or unsupported ELF format: path=%s class=%u data=%u", path.c_str(),
            static_cast<unsigned int>(header.e_ident[EI_CLASS]), static_cast<unsigned int>(header.e_ident[EI_DATA]));
        return false;
    }
    if (header.e_shentsize != sizeof(Elf64_Shdr) || header.e_shnum == 0 ||
        !ContainsElfRange(elf, header.e_shoff, static_cast<uint64_t>(header.e_shnum) * sizeof(Elf64_Shdr))) {
        ASCENDLOGE(
            "Invalid ELF section table: path=%s file_size=%zu section_offset=%llu section_count=%u "
            "section_entry_size=%u",
            path.c_str(), elf.size(), static_cast<unsigned long long>(header.e_shoff),
            static_cast<unsigned int>(header.e_shnum), static_cast<unsigned int>(header.e_shentsize));
        return false;
    }
    std::optional<Elf64_Shdr> sectionNameTable;
    if (header.e_shstrndx != SHN_UNDEF &&
        !ReadElfStringTableHeader(elf, header, header.e_shstrndx, "section-name", sectionNameTable.emplace())) {
        return false;
    }
    for (uint32_t index = 0; index < header.e_shnum; ++index) {
        Elf64_Shdr section{};
        if (!ReadElfRecord(elf, header.e_shoff + index * sizeof(Elf64_Shdr), section)) {
            return false;
        }
        if (sectionNameTable && !ReadElfName(elf, *sectionNameTable, section.sh_name, sectionNames)) {
            return false;
        }
        if ((section.sh_type == SHT_SYMTAB || section.sh_type == SHT_DYNSYM) &&
            !ReadDefinedFunctionNames(elf, header, section, functionNames)) {
            return false;
        }
    }
    return true;
}

aclError BindExistingKernelRenameArguments(CompilationCommand& command)
{
    if (!command.kernelSymbolRename.has_value()) {
        return ACLRTC_SUCCESS;
    }
    if (command.arguments.size() != 1U) {
        ASCENDLOGE("Kernel symbol renaming requires exactly one input ELF path");
        return ACLRTC_ERROR_FAILURE;
    }
    const std::string elfPath = command.arguments.front();
    std::set<std::string> sectionNames;
    std::set<std::string> functionNames;
    if (!ReadElfNames(elfPath, sectionNames, functionNames)) {
        ASCENDLOGE("Cannot read section or function names from linked ELF: %s", elfPath.c_str());
        return ACLRTC_ERROR_COMPILATION;
    }
    const auto& rename = *command.kernelSymbolRename;
    std::vector<std::string> arguments;
    for (const char* suffix : {"", "_mix_aic", "_mix_aiv"}) {
        const std::string original = rename.original + suffix;
        if (functionNames.count(original) == 0U) {
            continue;
        }
        const std::string renamed = rename.renamed + suffix;
        arguments.emplace_back("--redefine-sym=" + original + "=" + renamed);
        const std::string originalSection = ".ascend.meta." + original;
        if (sectionNames.count(originalSection) != 0U) {
            arguments.emplace_back("--rename-section=" + originalSection + "=.ascend.meta." + renamed);
        }
    }
    if (arguments.empty()) {
        ASCENDLOGE(
            "No defined kernel entry matches '%s' or its MIX suffixes in linked ELF: %s", rename.original.c_str(),
            elfPath.c_str());
        return ACLRTC_ERROR_COMPILATION;
    }
    arguments.emplace_back(elfPath);
    command.arguments = std::move(arguments);
    return ACLRTC_SUCCESS;
}
} // namespace

CompilationCommandExecutionLimits CompilationCommandExecutionLimits::ProductionDefaults() noexcept
{
    return {std::chrono::minutes(5), std::chrono::minutes(1), std::chrono::seconds(5)};
}

CompilationPlanExecutor::CompilationPlanExecutor(
    KernelSpecializationDiagnostics& specializationDiagnostics, CompilationCommandExecutionLimits executionLimits)
    : specializationDiagnostics_(specializationDiagnostics), commandExecutionLimits_(executionLimits)
{}

aclError CompilationPlanExecutor::ExecuteCompilationPlan(const KernelCompilationPlan& compilationPlan) const
{
    try {
        for (const CompilationCommand& plannedCommand : compilationPlan.compilationCommands) {
            CompilationCommand compilationCommand = plannedCommand;
            const aclError preparationStatus = BindExistingKernelRenameArguments(compilationCommand);
            if (preparationStatus != ACLRTC_SUCCESS) {
                return preparationStatus;
            }
            ASCENDLOGI(
                "Executing command %s with %s", compilationCommand.diagnosticLabel.c_str(),
                compilationCommand.executablePath.c_str());
            specializationDiagnostics_.AppendCommandToReplayScript(compilationCommand);
            specializationDiagnostics_.AppendCommandExecutionHeaderToLog(compilationCommand);
            ProcessExecutorResult executorResult;
            const aclError commandExecutionStatus = ExecuteCompilationCommand(compilationCommand, executorResult);
            specializationDiagnostics_.AppendCommandExecutionResultToLog(compilationCommand, executorResult);
            if (commandExecutionStatus != ACLRTC_SUCCESS) {
                ASCENDLOGE(
                    "Command %s failed: outcome=%s code=%d elapsed_ms=%lld", compilationCommand.diagnosticLabel.c_str(),
                    executorResult.GetOutcomeName(), static_cast<int>(executorResult.terminationCode),
                    static_cast<long long>(executorResult.elapsedTime.count()));
                specializationDiagnostics_.LogCommandFailureRecoveryHint();
                return commandExecutionStatus;
            }
        }
        return ACLRTC_SUCCESS;
    } catch (const std::bad_alloc&) {
        ASCENDLOGE("Out of memory while executing kernel compilation commands");
        return ACLRTC_ERROR_OUT_OF_MEMORY;
    } catch (const std::exception& exception) {
        ASCENDLOGE("Kernel compilation command execution failed with exception: %s", exception.what());
        return ACLRTC_ERROR_FAILURE;
    } catch (...) {
        ASCENDLOGE("Kernel compilation command execution failed with an unknown exception");
        return ACLRTC_ERROR_FAILURE;
    }
}

aclError CompilationPlanExecutor::ExecuteCompilationCommand(
    const CompilationCommand& compilationCommand, ProcessExecutorResult& executorResult) const
{
    ProcessExecutorRequest executorRequest;
    executorRequest.arguments.reserve(compilationCommand.arguments.size() + 1U);
    executorRequest.arguments.emplace_back(compilationCommand.executablePath.string());
    executorRequest.arguments.insert(
        executorRequest.arguments.end(), compilationCommand.arguments.begin(), compilationCommand.arguments.end());
    executorRequest.mirroredOutputLogFilePath = specializationDiagnostics_.GetCompilationLogFilePath();
    executorRequest.executionTimeout = compilationCommand.commandKind == CompilationCommandKind::Compile ?
                                           commandExecutionLimits_.compileCommandTimeout :
                                           commandExecutionLimits_.objectCopyAndLinkCommandTimeout;
    executorRequest.terminationGracePeriod = commandExecutionLimits_.terminationGracePeriod;
    executorResult = ProcessExecutor::Execute(executorRequest);
    if (executorResult.HasSuccessfulExit()) {
        return ACLRTC_SUCCESS;
    }
    if (executorResult.outcome == ProcessOutcome::ExecutorFailure) {
        return ACLRTC_ERROR_FAILURE;
    }
    return compilationCommand.commandKind == CompilationCommandKind::Link ? ACLRTC_ERROR_LINKING :
                                                                            ACLRTC_ERROR_COMPILATION;
}

} // namespace aclrtc
} // namespace ascendc
