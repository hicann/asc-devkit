# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

本入门示例基于Ascend C SIMD实现Hello World算子，带你快速上手实践，涵盖Device端核函数实现、Host端调用以及编译运行的完整流程，帮助开发者建立整体认知。

开始前请参考[环境准备](../../environment_setup.md)安装所需的CANN软件包，完整样例请参考[hello\_world](../../../../../../examples/01_simd_cpp_api/00_introduction/00_quickstart/hello_world/README.md)。

- **Hello World功能介绍**：

    在NPU上打印`Hello World!!!`。

- **Device端Kernel实现**：

    后缀名为`*.asc`的代码文件包含Host端与Device端代码，其Device端部分示例如下：
    ```cpp
    __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
    ```
    > [!NOTE]说明
    > - SIMD算子的Kernel函数需要额外的修饰符，如[`__vector__`](../../../programming_guide/language_extension/simd_builtin_keywords.md)修饰符说明该算子仅在向量计算单元上执行。
    
- **Host端代码实现**：
 
    Host端通过<<<>>>语法糖调用Device端代码片段。
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

- **算子编译与运行**：
 
    ```bash
    bisheng hello_world.asc --npu-arch=dav-2201 -o demo
    ./demo
    ```
    执行本样例，将打印了核号和`Hello World!!!`信息。

    > [!NOTE]说明
    >- 该样例支持如下型号：
    >    <!-- npu="950" id1 -->
    >    - Ascend 950PR/Ascend 950DT
    >    <!-- end id1 -->
    >    <!-- npu="A3" id2 -->
    >    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    >    <!-- end id2 -->
    >    <!-- npu="910b" id3 -->
    >    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    >    <!-- end id3 -->
    > - 编译选项`--npu-arch`用于指定NPU架构版本，`dav-`后为架构版本号，请替换为您实际使用的版本。各AI处理器型号对应的架构版本号请通过[AI处理器型号和 \_\_NPU\_ARCH\_\_ 的对应关系](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)查询。

如需进一步了解Ascend C的SIMD与SIMT编程模型，请参阅[Ascend C编程模型概述](../../../programming_guide/programming_model/programming_model_overview.md)。
