# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=2b484c5881d1dac6bdf3605d81a9eed1e5b244d9 translatedAt=2026-11-09T10:24:11.094Z pushedAt=2026-11-19T06:32:19.696Z nl=2 -->

This getting-started example implements a Hello World operator based on Ascend C SIMD to help you get started quickly. It covers the complete process of implementing the device-side kernel, calling it from the host side, and compiling and running it, helping developers build an overall understanding.

Before you start, install the required CANN software package by referring to [Environment Setup](../../environment_setup.md). For the complete sample, see [hello\_world](../../../../../../examples/01_simd_cpp_api/00_introduction/00_quickstart/hello_world/README.md).

- **Hello World feature overview**:

    Prints `Hello World!!!` on the NPU.

- **Device-side kernel function implementation**:

    A code file with the `*.asc` extension contains both host-side and device-side code. The device-side part is as follows:

    ```cpp
    __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
    ```

    > [!NOTE] Note
    > The kernel of a SIMD operator requires additional modifiers. For example, the [`__vector__`](../../../programming_guide/language_extension/simd_builtin_keywords.md) modifier indicates that the operator is executed only on the vector compute unit.
    
- **Host-side code implementation**:
 
    The host side calls the device-side code snippet through the `<<<>>>` syntax sugar.

    ```cpp
    int main(int argc, char const* argv[])
    {
        ...
        // Launch kernel <<<numBlocks, dynUBufSize, stream>>>
        // numBlocks : Number of blocks. Default to 8 in this example.
        // dynUBufSize : Dynamic unified buffer size. Default to 0 in this example.
        // stream : Runtime stream. Uses stream created by aclrtCreateStream API in this example.
        hello_world<<<numBlocks, 0, stream>>>();
        ...
    }
    ```

- **Operator compilation and execution**:  

    The complete code of this sample (**hello_world.asc**) is as follows:
    ```cpp
    #include "utils/debug/asc_printf.h"
    #include "acl/acl.h"

    __global__ __vector__ void hello_world() { printf("Hello World!!!\n"); }

    int main(int argc, char const* argv[])
    {
        aclInit(nullptr);
        aclrtSetDevice(0); // Acquire runtime management resources.
        hello_world<<<8, 0, nullptr>>>();
        aclrtSynchronizeDevice();
        aclrtResetDevice(0); // Release runtime resources.
        aclFinalize();
        return 0;
    }
    ```
    Run the following command to compile and run the sample:
    ```bash
    bisheng hello_world.asc --npu-arch=dav-2201 -o demo
    ./demo
    ```

    Running this sample prints the core ID and the `Hello World!!!` message.

    > [!NOTE] Note
    >- This sample supports the following models:
    >    <!-- npu="950" id1 -->
    >    - Ascend 950PR&950DT products
    >    <!-- end id1 -->
    >    <!-- npu="A3" id2 -->
    >    - Atlas A3 products
    >    <!-- end id2 -->
    >    <!-- npu="910b" id3 -->
    >    - Atlas A2 products
    >    <!-- end id3 -->
    > - The `--npu-arch` compilation option specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the architecture version corresponding to each AI processor model, see [the mapping between AI processor models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

For more information about the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
