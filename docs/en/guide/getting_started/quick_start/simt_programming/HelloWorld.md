# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-08-26T12:55:35.324Z -->

This getting-started example implements a Hello World operator based on Ascend C SIMT to help you quickly get started with hands-on practice. It covers the complete process of device-side kernel function implementation, host-side call, and compilation and execution, helping developers build an overall understanding.

Before you begin, see the [environment setup](../../environment_setup.md) to install the required CANN software package. For the complete sample, see [hello\_world](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt/README.md).

- **Hello World feature overview**:

    Print `Hello World!` on the NPU.

- **Device-side kernel implementation**:

    Code files with the `*.asc` extension contain both Host-side and Device-side code. The Device-side part is shown as follows:
    ```cpp
    __global__ void hello_world()
    {
        if (threadIdx.x < 3) {
            printf("[blockIdx (%lu/%lu)][threadIdx (%lu/%lu)]: Hello World!\n", blockIdx.x, gridDim.x, threadIdx.x, blockDim.x);
        }
    }
    ```

- **Host-side code implementation**:

    The host side calls the device-side code through the <<<>>> syntax sugar.
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

- **Operator compilation and execution**:
    ```bash
    bisheng hello_world.asc --npu-arch=dav-3510 -o demo --enable-simt 
    ./demo
    ```

Running this sample prints the core ID and the `Hello World!` message.

> [!NOTE] Note
> - This sample supports the following models:
>     - Ascend 950PR / Ascend 950DT
> - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the mapping between AI processor models and architecture version numbers, see the [mapping between AI processor models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).
> - The compilation option `--enable-simt` enables the SIMT programming scenario.

To learn more about the SIMD and SIMT programming models of Ascend C, see the [Ascend C programming model overview](../../../programming_guide/programming_model/programming_model_overview.md).
