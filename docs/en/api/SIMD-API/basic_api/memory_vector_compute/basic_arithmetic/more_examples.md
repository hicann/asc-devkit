# More Examples<a name="ZH-CN_TOPIC_0000001482470496"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T06:45:43.135Z pushedAt=2026-11-18T01:25:07.028Z nl=2 -->

-   Use the mask continuous mode of the tensor high-dimensional splitting computation API to implement non-continuous data computation.

    ```cpp
    uint64_t mask = 64;  // Only the first 64 numbers are computed in each iteration.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```bash
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dstLocal:
    [514 516 518 ... 640 undefined ... undefined
     770 772 774 ... 896 undefined ... undefined
     1026 1028 1030 ... 1152 undefined ... undefined
     1282 1284 1286 ... 1408 undefined ... undefined]
    ```

-   Use the mask bit-by-bit mode of the tensor high-dimensional partitioning computation interface to implement non-contiguous counting of data.

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0 };  // mask[0] is full and mask[1] is empty, so only the first 64 numbers are computed each time.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```bash
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dstLocal:
    [514 516 518 ... 640 undefined ... undefined
     770 772 774 ... 896 undefined ... undefined
     1026 1028 1030 ... 1152 undefined ... undefined
     1282 1284 1286 ... 1408 undefined ... undefined]
    ```

-   Implement non-contiguous computation of data by controlling the Repeat Stride parameter of the tensor high-dimensional partitioning computation interface.

    ```cpp
    uint64_t mask = 128;
    // repeatTime is set to 2, indicating that two iterations are required in total.
    // src0BlkStride and src1BlkStride are set to 1, indicating that the address interval of the data involved in the computation for src0 in each iteration is one DataBlock.
    // Set src0RepStride to 16, indicating that the src0 start address interval between adjacent iterations is 16 datablocks.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 2, { 1, 1, 1, 8, 16, 8 });
    ```

    The result is as follows:

    ```bash
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dstLocal:
    [514 516 518 ...768 898 900 902 ... 1150 1152 undefined ... undefined]
    ```

-   Implement non-contiguous computation of data by controlling the DataBlock Stride and Repeat Stride parameters of the tensor high-dimensional partitioning computation interface.

    ```cpp
    uint64_t mask = 128;
    // Set repeatTime to 2, indicating that a total of 2 iterations are required.
    // Set src0BlkStride to 2, indicating that the data address interval of src0 participating in computation within each iteration is 2 datablocks.
    // Set src0RepStride to 16, indicating that the src0 start address interval between adjacent iterations is 16 datablocks.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 2, { 1, 2, 1, 8, 16, 8 });
    ```

    The result is as follows:

    ```bash
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dstLocal:
    [514 516 518 ... 544  562 564 566 ... 592  610 612 614 ... 640  658 660 662 ... 688
     706 708 710 ... 736  754 756 758 ... 784  802 804 806 ... 832  850 852 854 ... 880 
     898 900 902 ... 928  946 948 950 ... 976  994 996 998 ... 1024  1042 1044 1046 ... 1072
    1090 1092 1094 ... 1120  1138 1140 1142 ... 1168  1186 1188 1190 ... 1216 1234 1236 1238 … 1264
    undefined ... undefined]
    ```

-   Example of using an API that requires scalar parameters.

    ```cpp
    #include "kernel_operator.h"
    constexpr int32_t BUFFER_NUM = 2;
    class KernelBinaryScalar {
    public:
        __aicore__ inline KernelBinaryScalar() {}
        __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, float scalar, uint32_t totalLength, uint32_t tileNum)
        {
            this->blockLength = totalLength / AscendC::GetBlockNum();
            this->scalar = scalar;
            this->tileNum = tileNum;
            ASSERT(tileNum != 0 && "tile num can not be zero!");
            this->tileLength = this->blockLength / tileNum / BUFFER_NUM;
            xGm.SetGlobalBuffer((__gm__ DTYPE_X*)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
            zGm.SetGlobalBuffer((__gm__ DTYPE_Z*)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
            pipe.InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(DTYPE_X));
            pipe.InitBuffer(outQueueZ, BUFFER_NUM, this->tileLength * sizeof(DTYPE_Z));
        }
        __aicore__ inline void Process()
        {
            int32_t loopCount = this->tileNum * BUFFER_NUM;
            for (int32_t i = 0; i < loopCount; i++) {
                CopyIn(i);
                Compute(i);
                CopyOut(i);
            }
        }
    private:
        __aicore__ inline void CopyIn(int32_t progress)
        {
            AscendC::LocalTensor<DTYPE_X> xLocal = inQueueX.AllocTensor<DTYPE_X>();
            AscendC::DataCopy(xLocal, xGm[progress * this->tileLength], this->tileLength);
            inQueueX.EnQue(xLocal);
        }
        __aicore__ inline void Compute(int32_t progress)
        {
            AscendC::LocalTensor<DTYPE_X> xLocal = inQueueX.DeQue<DTYPE_X>();
            AscendC::LocalTensor<DTYPE_Z> zLocal = outQueueZ.AllocTensor<DTYPE_Z>();
            AscendC::Adds(zLocal, xLocal, (DTYPE_X)scalar, this->tileLength);
            outQueueZ.EnQue<DTYPE_Z>(zLocal);
            inQueueX.FreeTensor(xLocal);
        }
        __aicore__ inline void CopyOut(int32_t progress)
        {
            AscendC::LocalTensor<DTYPE_Z> zLocal = outQueueZ.DeQue<DTYPE_Z>();
            AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, this->tileLength);
            outQueueZ.FreeTensor(zLocal);
        }
    private:
        AscendC::TPipe pipe;
        AscendC::TQue<AscendC::TPosition::VECIN, BUFFER_NUM> inQueueX;
        AscendC::TQue<AscendC::TPosition::VECOUT, BUFFER_NUM> outQueueZ;
        AscendC::GlobalTensor<DTYPE_X> xGm;
        AscendC::GlobalTensor<DTYPE_Z> zGm;
        float scalar;
        uint32_t blockLength;
        uint32_t tileNum;
        uint32_t tileLength;
    };
    extern "C" __global__ __aicore__ void binary_scalar_simple_kernel(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelBinaryScalar op;
        op.Init(x, z, tilingData.scalar, tilingData.totalLength, tilingData.tileNum);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
