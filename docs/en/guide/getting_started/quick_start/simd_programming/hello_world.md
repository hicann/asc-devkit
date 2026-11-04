# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:14:48.023Z pushedAt=2026-11-03T07:05:13.260Z nl=2 -->

This getting-started example implements a Hello World operator based on Ascend C SIMD to help you get started quickly. It covers the complete process of implementing the device-side kernel, calling it from the host side, and compiling and running the operator, helping developers build an overall understanding.

Before you start, install the required CANN software package by referring to [Environment Setup](../../environment_setup.md). For the complete sample, see [hello\_world](../../../../../../examples/01_simd_cpp_api/00_introduction/00_quickstart/hello_world/README.md).

- **Hello World feature overview**:

    Prints `Hello World!!!` on the NPU.

- **Device-side kernel implementation**:

    A code file with the `*.asc` extension contains both host-side and device-side code. The device-side part is as follows:
    ```cpp
    __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
    ```
    > [!NOTE] Note
    > - The kernel of a SIMD operator requires additional modifiers. For example, the [`__vector__`](../../../programming_guide/language_extension/simd_builtin_keywords.md) modifier indicates that the operator is executed only on the vector compute unit.
    
- **Host-side code implementation**:
 
    The host side calls the device-side code snippet through the <<<>>> syntactic sugar.
    ```cpp
    int main(int argc, char const* argv[])
    {
        ...
        constexpr uint32_t numBlocks = 8;
        ...
        
        aclrtStream stream = nullptr;
        aclrtCreateStream(&stream);
        // Launch kernel <<<numBlocks, dynUBufSize, stream>>>
        // numBlocks : Number of blocks. Default to 8 in this example.
        // dynUBufSize : Dynamic unified buffer size. Default to 0 in this example.
        // stream : Runtime stream. Uses stream created by aclrtCreateStream API in this example.
        hello_world<<<numBlocks, 0, stream>>>();
        ...
    }
    ```

- **Operator compilation and execution**:
 
    ```bash
    bisheng hello_world.asc --npu-arch=dav-2201 -o demo
    ./demo
    ```
    Running this sample prints the core number and the `Hello World!!!` message.

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
    > - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the architecture version number corresponding to each AI processor model, see [Correspondence Between AI Processor Models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

To learn more about the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
