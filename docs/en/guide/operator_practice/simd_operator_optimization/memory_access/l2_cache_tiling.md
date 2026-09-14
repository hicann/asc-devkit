# L2 Cache Tiling<a name="ZH-CN_TOPIC_0000001893038949"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:22:41.380Z -->

[Priority] High

[Description] Assume that the L2 cache of an AI processor is 192 MB, the mixed read/write bandwidth of the L2 cache is about 7 TB/s, and the bandwidth of GM is about 1.6 TB/s. There is a large gap between the two. When moving in or moving out the same amount of data, reading and writing data from the L2 cache is faster than from GM. If data cannot hit the L2 cache, that is, the data to be accessed is not in the L2 cache, the data must be read from or written to GM, resulting in low bandwidth utilization. Eventually, moving data in or out becomes the performance bottleneck of the entire operator execution. Tiling strategy recommendation: When the total amount of input and output data exceeds the L2 cache size, enable the L2 cache Tiling strategy in Tiling.

[Negative Example]

Assume that the input data size is InputTotalSize, the L2 cache size is L2CacheSize, InputTotalSize = L2CacheSize \* 2, the total number of cores is 20, and the data is not tiled, so the entire computation is completed at once. Assume that 20 cores can process a total of L2CacheSize data at a time, then each core reads the input data at least twice.

**Figure 1**  L2 cache tiling not enabled<a name="fig126831114552"></a>  
![](../../../figures/l2_cache_tiling_disabled.png "L2 cache Tiling not enabled")

```
constexpr int32_t TOTAL_LENGTH = InputTotalSize / sizeof(half);
constexpr int32_t USE_CORE_NUM = 20;
constexpr int32_t TILE_NUM = 2;
constexpr int32_t BLOCK_LENGTH = TOTAL_LENGTH / USE_CORE_NUM;
constexpr int32_t TILE_LENGTH = BLOCK_LENGTH / TILE_NUM;

class KernelSample {
public:
    __aicore__ inline KernelSample() {}
    __aicore__ inline void Init(GM_ADDR x)
    {
        xGm.SetGlobalBuffer((__gm__ half*)x + BLOCK_LENGTH * GetBlockIdx(), BLOCK_LENGTH);
        yGm.SetGlobalBuffer((__gm__ half*)y + BLOCK_LENGTH * GetBlockIdx(), BLOCK_LENGTH);
        pipe.InitBuffer(inQueueX, 1, BLOCK_LENGTH * sizeof(half));
        pipe.InitBuffer(inQueueY, 1, BLOCK_LENGTH * sizeof(half));
    }
    __aicore__ inline void Process()
    {
        // The example demonstrates the operation of adding 2 to the input data
        constexpr int32_t loopCount = 2;
        for (int32_t i = 0; i < loopCount; i++) {
            // Each iteration of the outer loop adds 1 to the input data
            for (int32_t j = 0; j < TILE_NUM; j++) {
                // The inner loop processes block 0 and block 1 of each core separately
                CopyIn(j);
                Compute();
                CopyOut(j);
            }
        }
    }
private:
    __aicore__ inline void CopyIn(int32_t process)
    {
        LocalTensor<half> xLocal = inQueueX.AllocTensor<half>();
        // For each core, except for the first read, when reading block 0, the L2 cache holds block 1;
        // For each core, when reading the first block of data, the data cached in the L2 Cache is the zeroth block;
        // Each core needs to read data from GM four times
        DataCopy(xLocal, xGm[process * TILE_LENGTH], TILE_LENGTH );
        inQueueX.EnQue(xLocal);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> yLocal = inQueueY.AllocTensor<half>();
        LocalTensor<half> xLocal = inQueueX.DeQue<half>();
        Adds(yLocal, xLocal, 1, TILE_LENGTH);   
        inQueueY.EnQue<half>(yLocal);
        inQueueX.FreeTensor(xLocal);
    }
    __aicore__ inline void CopyOut(int32_t process)
    {
        LocalTensor<half> yLocal = inQueueY.DeQue<half>();
        DataCopy(yGm[process * TILE_LENGTH], yLocal, TILE_LENGTH);
        inQueueY.FreeTensor(yLocal);
    }
}
...
```

[Positive Example]

Assume that the input data size is InputTotalSize, the L2 Cache size is L2CacheSize, InputTotalSize = L2CacheSize \* 2, and the total number of available cores is 20. The input data is evenly tiled into two parts, so the overall computation is performed in two passes, with each pass processing L2CacheSize data. In the first pass, the 20 cores compute the first L2CacheSize data; in the second pass, the 20 cores compute the remaining L2CacheSize data. The data read before each pass can hit the L2 Cache, improving operator performance.

**Figure 2**  L2 Cache Tiling enabled<a name="fig1623673819553"></a>  
![](../../../figures/l2_cache_tiling_enabled.png "L2 Cache Tiling enabled")

```
constexpr int32_t TOTAL_LENGTH = InputTotalSize / sizeof(half);
constexpr int32_t TILE_NUM = 2;
constexpr int32_t USE_CORE_NUM = 20;
constexpr int32_t TILE_LENGTH = TOTAL_LENGTH / TILE_NUM;
constexpr int32_t BLOCK_LENGTH = TILE_LENGTH / USE_CORE_NUM;

class KernelSample {
public:
    __aicore__ inline KernelSample() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, int32_t index)
    {
        xGm.SetGlobalBuffer((__gm__ half*)x + BLOCK_LENGTH * GetBlockIdx() + index * TILE_LENGTH, BLOCK_LENGTH);
        yGm.SetGlobalBuffer((__gm__ half*)y + BLOCK_LENGTH * GetBlockIdx() + index * TILE_LENGTH, BLOCK_LENGTH);
        pipe.InitBuffer(inQueueX, 1, BLOCK_LENGTH * sizeof(half));
        pipe.InitBuffer(inQueueY, 1, BLOCK_LENGTH * sizeof(half));
    }
    __aicore__ inline void Process()
    {
        // The example demonstrates the operation of adding 2 to the input data
        constexpr int32_t loopCount = 2;
        for (int32_t i = 0; i < loopCount; i++) {
            // Add 1 to the input data in each loop
            CopyIn();
            Compute();
            CopyOut();
        }
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> xLocal = inQueueX.AllocTensor<half>();
        // For each core, except for the first read, the second read can hit the L2 Cache;
        // Each core reads data from GM twice and accesses the L2 Cache twice to read data
        DataCopy(xLocal, xGm, BLOCK_LENGTH );
        inQueueX.EnQue(xLocal);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> yLocal = inQueueY.AllocTensor<half>();
        LocalTensor<half> xLocal = inQueueX.DeQue<half>();
        Adds(yLocal, xLocal, 1, BLOCK_LENGTH);   
        inQueueY.EnQue<half>(yLocal);
        inQueueX.FreeTensor(xLocal);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<half> yLocal = inQueueY.DeQue<half>();
        DataCopy(yGm, yLocal, BLOCK_LENGTH);
        inQueueY.FreeTensor(yLocal);
    }
}
...

extern "C" __global__ __aicore__ void simple_kernel(__gm__ uint8_t* srcGm, __gm__ uint8_t* dstGm)
{
    AscendC::KernelSample op;
    // Split the input data evenly into two parts for computation
    for (int32_t i = 0; i < TILE_NUM; i++) {
        op.Init(srcGm, dstGm, i);
        op.Process();
    }
}
...
```

For more complete samples, see [L2 Cache Tiling operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache).
