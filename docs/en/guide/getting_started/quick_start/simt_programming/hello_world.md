# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=276a605f3e5e5e4b0c5f588952ce2976d7be72c5 translatedAt=2026-11-09T10:24:26.300Z pushedAt=2026-11-19T06:32:19.699Z nl=2 -->

This getting-started example implements a Hello World operator based on Ascend C SIMT to help you get started quickly. It covers the complete process of implementing the device-side kernel, calling it from the host side, and compiling and running it, helping developers build an overall understanding.

Before you start, install the required CANN software package by referring to [Environment Setup](../../environment_setup.md). For the complete sample, see [hello\_world](../../../../../../examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt/README.md).

- **Hello World feature overview**:
 
    Prints `Hello World!` on the NPU.

- **Device-side kernel function implementation**:

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
        // Launch kernel <<<grid_dim, block_dim, dynamic_memory_size, stream>>>
        // grid_dim : Number of thread blocks (Grid size)
        // block_dim : Number of threads per block (Block size)
        // dynamic_memory_size : Dynamic unified buffer size. Default to 0 in this example
        // stream : Runtime stream, pass nullptr to use default stream
        hello_world<<<2, 32, 0, nullptr>>>();
        // Wait for kernel execution to complete
        ...
    }
    ```

- **Operator compilation and execution**:

    The complete code of this sample (**hello_world.asc**) is as follows:

    ```cpp
    #include "acl/acl.h"
    #include "simt_api/asc_simt.h"
    #include "utils/debug/asc_printf.h"

    __global__ void hello_world()
    {
        if (threadIdx.x < 3) {
            printf("[blockIdx (%u/%u)][threadIdx (%u/%u)]: Hello World!\n", blockIdx.x, gridDim.x, threadIdx.x, blockDim.x);
        }
    }

    int32_t main(int argc, char const* argv[])
    {
        aclInit(nullptr);
        aclrtSetDevice(0);
        hello_world<<<2, 32, 0, nullptr>>>();
        aclrtSynchronizeDevice();
        aclrtResetDevice(0);
        aclFinalize();
        return 0;
    }
    ```

    Run the following command to compile and run the sample:

    ```bash
    bisheng hello_world.asc --npu-arch=dav-3510 -o demo --enable-simt
    ./demo
    ```

    Running this sample prints the core ID and the `Hello World!` message.

    > [!NOTE] Note
    > - This sample supports the following models:
    >     <!-- npu="950" id1 -->
    >     - Ascend 950PR&950DT products
    >     <!-- end id1 -->
    > - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the architecture version corresponding to each AI processor model, see [the mapping between AI processor models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).
    > - The compilation option `--enable-simt` enables the SIMT programming scenario.

For more information about the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
