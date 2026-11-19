/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "cache_digest.h"

#include "securec.h"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace ascendc {
namespace aclrtc {
namespace {
constexpr std::array<uint32_t, 64> ROUND_CONSTANTS = {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
    0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
    0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU, 0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
    0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
    0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
    0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
    0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U, 0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U};

uint32_t RotateRight(uint32_t value, unsigned int count) { return (value >> count) | (value << (32U - count)); }

void Compress(const uint8_t* block, std::array<uint32_t, 8>& state)
{
    std::array<uint32_t, 64> words{};
    for (size_t i = 0; i < 16U; ++i) {
        words[i] = (static_cast<uint32_t>(block[i * 4U]) << 24U) | (static_cast<uint32_t>(block[i * 4U + 1U]) << 16U) |
                   (static_cast<uint32_t>(block[i * 4U + 2U]) << 8U) | static_cast<uint32_t>(block[i * 4U + 3U]);
    }
    for (size_t i = 16U; i < words.size(); ++i) {
        const uint32_t x = words[i - 15U];
        const uint32_t y = words[i - 2U];
        words[i] = words[i - 16U] + (RotateRight(x, 7U) ^ RotateRight(x, 18U) ^ (x >> 3U)) + words[i - 7U] +
                   (RotateRight(y, 17U) ^ RotateRight(y, 19U) ^ (y >> 10U));
    }
    auto working = state;
    for (size_t i = 0; i < words.size(); ++i) {
        const uint32_t first =
            working[7] + (RotateRight(working[4], 6U) ^ RotateRight(working[4], 11U) ^ RotateRight(working[4], 25U)) +
            ((working[4] & working[5]) ^ (~working[4] & working[6])) + ROUND_CONSTANTS[i] + words[i];
        const uint32_t second =
            (RotateRight(working[0], 2U) ^ RotateRight(working[0], 13U) ^ RotateRight(working[0], 22U)) +
            ((working[0] & working[1]) ^ (working[0] & working[2]) ^ (working[1] & working[2]));
        for (size_t j = 7U; j > 0U; --j) {
            working[j] = working[j - 1U];
        }
        working[4] += first;
        working[0] = first + second;
    }
    for (size_t i = 0; i < state.size(); ++i) {
        state[i] += working[i];
    }
}
} // namespace

std::string ComputeSha256(const void* bytes, size_t size)
{
    std::array<uint32_t, 8> state = {0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
                                     0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U};
    const auto* input = static_cast<const uint8_t*>(bytes);
    size_t offset = 0;
    for (; size - offset >= 64U; offset += 64U) {
        Compress(input + offset, state);
    }
    std::array<uint8_t, 128> tail{};
    const size_t remaining = size - offset;
    if (remaining != 0U) {
        const errno_t result = memcpy_s(tail.data(), tail.size(), input + offset, remaining);
        if (result != EOK) {
            throw std::runtime_error("Failed to copy SHA-256 tail block");
        }
    }
    tail[remaining] = 0x80U;
    const size_t tailSize = remaining < 56U ? 64U : 128U;
    const uint64_t bits = static_cast<uint64_t>(size) * 8U;
    for (size_t i = 0; i < 8U; ++i) {
        tail[tailSize - 1U - i] = static_cast<uint8_t>(bits >> (i * 8U));
    }
    Compress(tail.data(), state);
    if (tailSize == 128U) {
        Compress(tail.data() + 64U, state);
    }
    constexpr char HEX[] = "0123456789abcdef";
    std::string result;
    result.reserve(64U);
    for (const uint32_t word : state) {
        for (int shift = 28; shift >= 0; shift -= 4) {
            result.push_back(HEX[(word >> shift) & 0xfU]);
        }
    }
    return result;
}

std::string ComputeCacheIdentityDigest(const std::string& domain, const nlohmann::json& payload)
{
    const std::string encoded = domain + '\0' + payload.dump();
    return ComputeSha256(encoded.data(), encoded.size());
}

} // namespace aclrtc
} // namespace ascendc
