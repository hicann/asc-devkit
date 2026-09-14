# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-08-26T12:53:42.410Z -->

This getting-started example implements a Hello World operator based on Ascend C SIMD to help you quickly get started. It covers the complete process of implementing the kernel function on the Device side, invoking it on the Host side, and compiling and running the operator, helping developers build an overall understanding.

Before you start, install the required CANN software package by referring to the [environment setup](../../environment_setup.md). For the complete sample, see [hello\_world](../../../../../examples/01_simd_cpp_api/00_introduction/00_quickstart/hello_world/README.md).

- **Hello World feature overview**:

    Print `Hello World!!!` on the NPU.

- **Device-side kernel implementation**:

    A code file with the `*.asc` suffix contains both Host-side and Device-side code. The Device-side part is shown as follows:
    ```cpp
    __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
    ```
    > [!NOTE] Note
    > - The kernel function of a SIMD operator requires additional modifiers. For example, the [`__vector__`](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md) modifier indicates that the operator runs only on the vector computation unit.

- **Host-side code implementation**:

    The host side calls the device-side code snippet through the `<<<>>>` syntax sugar.
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
    When you run this sample, the core number and the `Hello World!!!` message are printed.

    > [!NOTE] Note
    >- This sample supports the following models:
    >    - Ascend 950PR/Ascend 950DT
    >    - Atlas A3 training products/Atlas A3 inference products
    >    - Atlas A2 training products/Atlas A2 inference products
    > - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the mapping between AI processor models and `__NPU_ARCH__`, see [Mapping Between AI Processor Models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).

To learn more about the SIMD and SIMT programming models of Ascend C, see the [Ascend C programming model overview](../../../programming_guide/programming_model/programming_model_overview.md).
