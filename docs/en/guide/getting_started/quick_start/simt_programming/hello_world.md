# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:18:27.147Z pushedAt=2026-11-03T07:05:13.262Z nl=2 -->

This getting-started example implements a Hello World operator based on Ascend C SIMT to help you get started quickly. It covers the complete process of implementing the device-side kernel, calling it from the host side, and compiling and running it, helping developers build an overall understanding.

Before you start, install the required CANN software package by referring to [Environment Setup](../../environment_setup.md). For the complete sample, see [hello\_world](../../../../../../examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt/README.md).

- **Hello World feature overview**:
 
    Prints `Hello World!` on the NPU.

- **Device-side kernel implementation**:

    A code file with the `*.asc` extension contains both host-side and device-side code. The device-side part is as follows:
    ```cpp
    __global__ void hello_world()
    {
        if (threadIdx.x < 3) {
            printf("[blockIdx (%u/%u)][threadIdx (%u/%u)]: Hello World!\n", blockIdx.x, gridDim.x, threadIdx.x, blockDim.x);
        }
    }
    ```

- **Host-side code implementation**:

    The host side calls the device-side code through the `<<<>>>` syntax sugar.
    ```cpp
    int32_t main(int argc, char const *argv[])
    {
        ...
        // 4. Configure kernel launch parameters
        constexpr uint32_t blocks_per_grid     = 2;   // Number of thread blocks (Grid size)
        constexpr uint32_t threads_per_block    = 32;  // Number of threads per block (Block size)
        constexpr uint32_t dyn_ubuf_size = 0;   // No dynamic memory required in this sample

        // 5. Launch kernel <<<grid_dim, block_dim, dynamic_memory_size, stream>>>
        hello_world<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>();
        // Wait for kernel execution to complete
        ...
    }
    ```

- **Operator compilation and running**:
    ```bash
    bisheng hello_world.asc --npu-arch=dav-3510 -o demo --enable-simt 
    ./demo
    ```

    Running this sample prints the core ID and the `Hello World!` message.

    > [!NOTE] Note
    > - This sample supports the following models:
    >     <!-- npu="950" id1 -->
    >     - Ascend 950PR & 950DT products
    >     <!-- end id1 -->
    > - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the architecture version number corresponding to each AI processor model, see [the mapping between the AI processor models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).
    > - The compilation option `--enable-simt` enables the SIMT programming scenario.

To learn more about the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
