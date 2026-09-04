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
 * \file l2_cache_optimizer.h
 */

#ifndef MATMUL_L2CACHE_L2_CACHE_OPTIMIZER_H
#define MATMUL_L2CACHE_L2_CACHE_OPTIMIZER_H

#include "utils/std/cmath.h"

namespace CustomMatmulL2Cache {

constexpr int64_t L2_TILE_THRESHOLD = 100 * 1024 * 1024;
constexpr int64_t L1_MIN_UST_DIM = 4;
constexpr int64_t L1_MAX_UST_DIM = 8;

template <size_t I, typename T>
__aicore__ constexpr inline decltype(auto) Get(T&& t)
{
    return AscendC::Std::get<I>(AscendC::Std::forward<T>(t));
}

template <class ProblemShape, const auto& MmTiling>
class L2CacheOptimizer {
public:
    using BlockShape = AscendC::Std::tuple<int64_t, int64_t, int64_t>;
    using BlockCoord = AscendC::Std::tuple<int64_t, int64_t, int64_t>;
    static constexpr int64_t l1M = MmTiling.baseM * MmTiling.stepM;
    static constexpr int64_t l1N = MmTiling.baseN * MmTiling.stepN;
    static constexpr int64_t l0M = MmTiling.baseM;
    static constexpr int64_t l0N = MmTiling.baseN;
    static constexpr int64_t l0K = MmTiling.baseK;

private:
    int64_t mTileNum_;
    int64_t nTileNum_;
    int64_t blockNum_;
    int64_t m_; // org shape
    int64_t n_;
    int64_t k_;
    int64_t totalTileNum_;
    // l2 spilit attribute
    int64_t newBlockIdx_;
    int64_t mL2TileNumTmp_;
    int64_t nL2TileNumTmp_;
    int64_t nL2Idx_;
    int64_t mL2Idx_;
    int64_t mL2BlockNum_; // l2 m block num
    int64_t nL2BlockNum_; // l2 n block num
    int64_t mL2TileNum_;  // a1b1 m tile num of one l2 block
    int64_t nL2TileNum_;  // a1b1 n tile num of one l2 block
public:
    __aicore__ inline L2CacheOptimizer(ProblemShape shape, int64_t blockNum) : blockNum_(blockNum)
    {
        m_ = shape.m;
        n_ = shape.n;
        k_ = shape.k;
        mTileNum_ = AscendC::Std::ceil_div(m_, l1M);
        nTileNum_ = AscendC::Std::ceil_div(n_, l1N);
        totalTileNum_ = mTileNum_ * nTileNum_;
        InitL2Tile();
    }

    __aicore__ inline int64_t GetTileNum() { return totalTileNum_; }

    __aicore__ inline BlockShape GetBlockShape(int64_t tileIdx)
    {
        GetCommonTileIndex(tileIdx);
        int64_t mTileIdx = newBlockIdx_ % mL2TileNumTmp_;
        mTileIdx = mTileIdx + mL2Idx_ * mL2TileNum_;

        int64_t nTileIdx = 0;
        if (mL2TileNumTmp_ != 0 && nL2TileNumTmp_ != 0) {
            int64_t tmp = newBlockIdx_ / CalcLcm(mL2TileNumTmp_, nL2TileNumTmp_);
            nTileIdx = (newBlockIdx_ + tmp) % nL2TileNumTmp_;
        }
        nTileIdx = nTileIdx + nL2Idx_ * nL2TileNum_;

        // calc tail l1block mnk
        int64_t tailL1M = (m_ % l1M == 0) ? l1M : m_ % l1M;
        int64_t tailL1N = (n_ % l1N == 0) ? l1N : n_ % l1N;
        int64_t blockShapeM = IsMTail(mTileIdx, mTileNum_) ? tailL1M : l1M;
        int64_t blockShapeN = IsNTail(nTileIdx, nTileNum_) ? tailL1N : l1N;

        return {blockShapeM, blockShapeN, k_};
    }

    __aicore__ inline BlockCoord GetBlockCoord(int64_t tileIdx)
    {
        GetCommonTileIndex(tileIdx);
        int64_t mTileIdx = newBlockIdx_ % mL2TileNumTmp_;
        mTileIdx = mTileIdx + mL2Idx_ * mL2TileNum_;

        int64_t nTileIdx = 0;
        if (mL2TileNumTmp_ != 0 && nL2TileNumTmp_ != 0) {
            int64_t tmp = newBlockIdx_ / CalcLcm(mL2TileNumTmp_, nL2TileNumTmp_);
            nTileIdx = (newBlockIdx_ + tmp) % nL2TileNumTmp_;
        }
        nTileIdx = nTileIdx + nL2Idx_ * nL2TileNum_;

        return {mTileIdx * l1M, nTileIdx * l1N, 0};
    }

private:
    __aicore__ inline int64_t GetTotalSize(int64_t mL2, int64_t nL2, int64_t kL2)
    {
        int64_t sizeA = mL2 * kL2 * sizeof(half);
        int64_t sizeB = kL2 * nL2 * sizeof(half);
        int64_t sizeC = mL2 * nL2 * sizeof(half);
        return sizeA + sizeB + sizeC;
    }

    __aicore__ inline bool EnableL2Tile() { return GetTotalSize(m_, n_, k_) > L2_TILE_THRESHOLD; }

    __aicore__ inline int64_t GetTail(int64_t dividend, int64_t divisor)
    {
        if (divisor == 0) {
            return 0;
        }
        auto remainder = dividend % divisor;
        return (remainder == 0) ? divisor : remainder;
    }

    __aicore__ inline void InitL2TileTail()
    {
        int64_t mConflict = INT64_MAX;
        int64_t nConflict = INT64_MAX;
        constexpr bool isNMajor = l1N > l1M;
        int64_t majorDim = isNMajor ? nTileNum_ : mTileNum_;
        int64_t minorDim = isNMajor ? mTileNum_ : nTileNum_;
        int64_t maxMajor = blockNum_ > majorDim ? majorDim : blockNum_;
        int64_t maxMinor = blockNum_ > minorDim ? minorDim : blockNum_;
        int64_t minMinor = isNMajor ? L1_MIN_UST_DIM : L1_MAX_UST_DIM;
        for (int64_t i = maxMajor; i >= L1_MIN_UST_DIM; i--) { // if l1N greater than l1M, indicates n
            for (int64_t j = maxMinor; j >= minMinor; j--) {
                if (GetTotalSize(j * l1M, i * l1N, k_) > L2_TILE_THRESHOLD) {
                    continue;
                }
                int64_t curMajorDim = isNMajor ? j : i;
                int64_t curMinorDim = isNMajor ? i : j;
                int64_t mL2TileNumTailTmp = GetTail(mTileNum_, curMajorDim);
                int64_t nL2TileNumTailTmp = GetTail(nTileNum_, curMinorDim);
                uint64_t mConflictTmp = AscendC::Std::ceil_div(blockNum_, mL2TileNumTailTmp);
                uint64_t nConflictTmp = AscendC::Std::ceil_div(blockNum_, nL2TileNumTailTmp);
                if (mConflict >= mConflictTmp && nConflict >= nConflictTmp) {
                    mConflict = mConflictTmp;
                    nConflict = nConflictTmp;
                    mL2TileNum_ = curMajorDim;
                    nL2TileNum_ = curMinorDim;
                }
            }
        }
        if (mL2TileNum_ == 0 || nL2TileNum_ == 0) {
            mL2TileNum_ = mTileNum_;
            nL2TileNum_ = nTileNum_;
        }
    }

    __aicore__ inline void InitL2Tile()
    {
        bool smallDim = mTileNum_ < L1_MIN_UST_DIM && nTileNum_ < L1_MIN_UST_DIM;
        if (smallDim || (!EnableL2Tile())) {
            mL2TileNum_ = mTileNum_;
            nL2TileNum_ = nTileNum_;
            mL2BlockNum_ = 1;
            nL2BlockNum_ = 1;
            return;
        }
        mL2TileNum_ = 0;
        nL2TileNum_ = 0;
        InitL2TileTail();
        mL2BlockNum_ = AscendC::Std::ceil_div(mTileNum_, mL2TileNum_);
        nL2BlockNum_ = AscendC::Std::ceil_div(nTileNum_, nL2TileNum_);
    }

    __aicore__ inline void GetCommonTileIndex(int64_t tileIdx)
    {
        mL2Idx_ = tileIdx / (mL2TileNum_ * nTileNum_);
        mL2TileNumTmp_ = (mL2Idx_ == mL2BlockNum_ - 1) ? GetTail(mTileNum_, mL2TileNum_) : mL2TileNum_;

        nL2Idx_ = (tileIdx % (mL2TileNum_ * nTileNum_)) / (mL2TileNumTmp_ * nL2TileNum_);
        nL2TileNumTmp_ = (nL2Idx_ == nL2BlockNum_ - 1) ? GetTail(nTileNum_, nL2TileNum_) : nL2TileNum_;

        int64_t startIdx = mL2Idx_ * mL2TileNum_ * nTileNum_ + nL2Idx_ * nL2TileNum_ * mL2TileNumTmp_;
        int64_t startBlockIdx = startIdx % blockNum_;
        newBlockIdx_ = tileIdx - startIdx;
    }

    __aicore__ inline int64_t CalcLcm(int64_t a, int64_t b)
    {
        if (a == 0 || b == 0) {
            return 0;
        }
        // calc GCD
        int64_t m = a;
        int64_t n = b;
        while (n != 0) {
            int64_t tmp = m % n;
            m = n;
            n = tmp;
        }
        int64_t gcd = m;
        return (a / gcd) * b;
    }

    __aicore__ inline bool IsMTail(int64_t mTileIdx, int64_t mTileNum)
    {
        return (mTileIdx - (mTileNum - 1)) % mTileNum == 0;
    }

    __aicore__ inline bool IsNTail(int64_t nTileIdx, int64_t nTileNum) { return nTileIdx == (nTileNum - 1); }
};
} // namespace CustomMatmulL2Cache
#endif
