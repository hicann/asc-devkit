/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#ifndef ASC_COMPILE_EXPORTER_TEST_SUPPORT_H
#define ASC_COMPILE_EXPORTER_TEST_SUPPORT_H

#include <gtest/gtest.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

#include "file_utils.h"

namespace asc_compile_exporter_test {

inline std::runtime_error SystemError(const std::string& operation)
{
    return std::runtime_error(operation + ": " + std::strerror(errno));
}

inline void WriteTestFile(const std::string& path, const std::string& content)
{
    if (!ascendc::FileUtils::CreateDirectories(ascendc::FileUtils::ParentPath(path))) {
        throw std::runtime_error("failed to create test directory for: " + path);
    }
    std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("failed to create unit test file: " + path);
    }
    output.write(content.data(), static_cast<std::streamsize>(content.size()));
    output.close();
    if (!output) {
        throw std::runtime_error("failed to write unit test file: " + path);
    }
}

inline std::string ReadTestFile(const std::string& path)
{
    std::ifstream input(path.c_str(), std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

inline bool LstatPathExists(const std::string& path)
{
    struct stat info {};
    return lstat(path.c_str(), &info) == 0;
}

class ScopedEnvironment final {
public:
    ScopedEnvironment(const char* name, const char* value) : name_(name)
    {
        const char* current = std::getenv(name);
        existed_ = current != nullptr;
        if (current != nullptr) {
            oldValue_ = current;
        }
        if (value == nullptr) {
            if (unsetenv(name_.c_str()) != 0) {
                throw SystemError("failed to unset test environment variable");
            }
        } else if (setenv(name_.c_str(), value, 1) != 0) {
            throw SystemError("failed to set test environment variable");
        }
    }

    ~ScopedEnvironment()
    {
        if (existed_) {
            (void)setenv(name_.c_str(), oldValue_.c_str(), 1);
        } else {
            (void)unsetenv(name_.c_str());
        }
    }

    ScopedEnvironment(const ScopedEnvironment&) = delete;
    ScopedEnvironment& operator=(const ScopedEnvironment&) = delete;

private:
    std::string name_;
    std::string oldValue_;
    bool existed_ = false;
};

class ModuleTest : public testing::Test {
protected:
    void SetUp() override
    {
        const char* temporary = std::getenv("TMPDIR");
        const std::string parent = temporary == nullptr || temporary[0] == '\0' ? "/tmp" : temporary;
        std::string pathTemplate = ascendc::FileUtils::JoinPath(parent, "asc_compile_exporter_module_ut_XXXXXX");
        std::vector<char> buffer(pathTemplate.begin(), pathTemplate.end());
        buffer.push_back('\0');
        char* created = mkdtemp(buffer.data());
        ASSERT_NE(created, nullptr);
        root_ = created;
    }

    void TearDown() override { (void)ascendc::FileUtils::RemoveAll(root_); }

    void CreateManifest(const std::string& unit, const std::string& name) const
    {
        WriteTestFile(
            ascendc::FileUtils::JoinPath(unit, name + "_manifest.json"),
            "{\"base_dir\":\"resources\",\"name\":\"" + name + "\"}");
    }

    std::string root_;
};

} // namespace asc_compile_exporter_test

#endif // ASC_COMPILE_EXPORTER_TEST_SUPPORT_H
