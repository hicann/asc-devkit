# Add算子快速入门<a name="ZH-CN_TOPIC_0000002500781060"></a>

本示例是一个入门实践，基于Ascend C SIMD实现Add算子，帮助您快速上手。它完整呈现了Device端核函数实现、Host端调用及编译运行的全流程，助您建立整体认知。开始前，请先参考[环境准备](../../环境准备.md)安装所需的CANN软件包。

下面分别介绍基于C API与C++ API的Add算子实现，完整示例代码请参考[基于C API实现的Add算子示例](../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add/README.md)和[基于C++ API实现的Add算子示例](../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md)。

- **Add算子功能介绍**：

  Add算子的数学表达式为：

  ![](../../../figures/zh-cn_formulaimage_0000002501241896.png)

  计算逻辑为逐元素完成`z = x + y`。

- **算子设计**

    - **Device端核函数编程接口**
        - 核函数定义：通过 [\_\_global\_\_](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)修饰符声明。
        - 数据分块（Tiling）：使用内置关键字 [block_idx](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)确定每个Block负责处理的数据。
        - 数据搬入：通过[C API接口](../../../编程指南/语言扩展层/SIMD语言扩展层C-API.md) `asc_copy_gm2ub`或[C++接口](../../../编程指南/类库API/编程接口概述.md) `AscendC::DataCopy`完成。
        - 数据计算：通过C API接口`asc_add`或C++接口`AscendC::Add`完成。
        - 数据搬出：通过C API接口`asc_copy_ub2gm`或C++接口`AscendC::DataCopy`完成。
    - **Host端运行时接口**
        - 内存分配：使用`aclrtMallocHost`分配Host Memory，`aclrtMalloc`分配Device Memory。
        - 数据搬入：使用`aclrtMemcpy`将输入数据从Host Memory拷贝到Device Memory。
        - 启动NPU计算任务：通过`<<<...>>>`语法糖启动核函数。
        - 同步等待：调用`aclrtSynchronizeStream`或`aclrtSynchronizeDevice`等待任务完成。
        - 数据搬出：使用`aclrtMemcpy`将计算结果从Device Memory拷贝回Host Memory。
      
      > [!NOTE]说明
      > - 请参见[Ascend-C概述与学习路径](../../Ascend-C概述与学习路径.md)技术附录章节，获取`Ascend C API参考`和`CANN运行时接口`链接，以查阅更多接口信息。

- **算子代码实现**：

  后缀名为`*.asc`的代码文件包含Host端与Device端代码。

  - **Device端Kernel实现**：
  Device端部分示例如下：
    - **基于C语言API实现Memory矢量计算示例**
      ```cpp
      __vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
      {
          asc_init();

          constexpr uint32_t block_length = TOTAL_LENGTH / NUM_BLOCKS;
          
          // Determine the data that each block needs to process
          __gm__ float* x_gm = x + block_idx * block_length;
          __gm__ float* y_gm = y + block_idx * block_length;
          __gm__ float* z_gm = z + block_idx * block_length;

          // Allocate on-chip UB memory
          __ubuf__ float x_local[block_length];
          __ubuf__ float y_local[block_length];
          __ubuf__ float z_local[block_length];

          // Copy input data from Global Memory (i.e., Device Memory) to on-chip UB memory
          asc_copy_gm2ub(x_local, x_gm, block_length * sizeof(float));
          asc_copy_gm2ub(y_local, y_gm, block_length * sizeof(float));
          asc_sync();

          // Call SIMD API to complete the Add operation
          asc_add(z_local, x_local, y_local, block_length);
          asc_sync();

          // Write the result from on-chip UB memory back to Global Memory
          asc_copy_ub2gm(z_gm, z_local, block_length * sizeof(float));
          asc_sync();
      }
      ```
      > [!NOTE]说明
      > - 本Memory矢量计算示例支持以下型号：
      >     <!-- npu="A3" id1 -->
      >     - Atlas A3训练系列产品/Atlas A3推理系列产品
      >     <!-- end id1 -->
      >     <!-- npu="910b" id2 -->
      >     - Atlas A2训练系列产品/Atlas A2推理系列产品
      >     <!-- end id2 -->
      > - SIMD算子的Kernel函数需要额外修饰符，[`__vector__`](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)修饰符表明该算子仅在向量计算单元上执行。
      > - **性能提示**：示例中为简化同步操作，统一使用了 `asc_sync`。在实际算子开发中，建议根据流水线执行情况使用具体的同步控制指令，以获得更好的性能。详见[同步机制](../../../编程指南/编程模型/AI-Core-SIMD编程/基于指针的C语言编程/C语言编程概述.md#同步机制)章节。

    - **基于C++ Tensor实现Memory矢量计算示例**

      ```cpp
      template <uint32_t blockLength>
      __vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
      {
          AscendC::InitSocState();
          
          // Determine the data that each block needs to process
          AscendC::GlobalTensor<float> xGm, yGm, zGm;
          xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);
          yGm.SetGlobalBuffer(y + block_idx * blockLength, blockLength);
          zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);
          
          // Allocate on-chip UB memory
          AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
          AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
          AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
          AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();
          
          // Copy input data from Global Memory (i.e., Device Memory) to on-chip UB memory
          AscendC::DataCopy(xLocal, xGm, blockLength);
          AscendC::DataCopy(yLocal, yGm, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();
          
          // Call SIMD API to complete the Add operation
          AscendC::Add(zLocal, xLocal, yLocal, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();
          
          // Write the result from on-chip UB memory back to Global Memory
          AscendC::DataCopy(zGm, zLocal, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();
      }
      ```
      > [!NOTE]说明
      > - 该样例支持以下型号：
      >     <!-- npu="950" id3 -->
      >     - Ascend 950PR/Ascend 950DT
      >     <!-- end id3 -->
      >     <!-- npu="A3" id4 -->
      >     - Atlas A3训练系列产品/Atlas A3推理系列产品
      >     <!-- end id4 -->
      >     <!-- npu="910b" id5 -->
      >     - Atlas A2训练系列产品/Atlas A2推理系列产品
      >     <!-- end id5 -->
      > - SIMD算子的Kernel函数需要额外修饰符，[`__vector__`](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)修饰符表明该算子仅在向量计算单元上执行。

  - **Host端代码实现**：

    Host端通过`<<<>>>`语法糖调用Device端核函数，示例代码片段如下：
    ```cpp
      int32_t main(int argc, char const *argv[])
      {
          ...
          constexpr uint32_t numBlocks = 8;
          ...

          // Create runtime stream by aclrtCreateStream API
          aclrtStream stream = nullptr;
          aclrtCreateStream(&stream);
          
          // Allocate host and device memory, and copy input data from host to device
          aclrtMalloc((void**)&xDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
          aclrtMalloc((void**)&yDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
          aclrtMalloc((void**)&zDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
          aclrtMallocHost((void**)&zHost, totalByteSize);

          aclrtMemcpy(xDevice, totalByteSize, x.data(), totalByteSize, ACL_MEMCPY_HOST_TO_DEVICE);
          aclrtMemcpy(yDevice, totalByteSize, y.data(), totalByteSize, ACL_MEMCPY_HOST_TO_DEVICE);

          // Launch kernel <<<numBlocks, dynUBufSize, stream>>>
          // numBlocks : Number of blocks. Default to 8 in this example.
          // dynUBufSize : Dynamic unified buffer size. Default to 0 in this example.
          // stream : Runtime stream.

          // Example：Call add kernel which is implemented by SIMD C++ Basic API
          add_custom<blockLength><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);

          // Wait for the add_custom kernel to complete
          aclrtSynchronizeStream(stream);

          // Copy the result from device memory to host memory
          aclrtMemcpy(zHost, totalByteSize, zDevice, totalByteSize, ACL_MEMCPY_DEVICE_TO_HOST);
          ...
      }
    ```

- **算子编译与运行**：

  CMake配置文件示例：
  ```cmake
  cmake_minimum_required(VERSION 3.16)

  find_package(ASC REQUIRED)

  project(kernel_samples LANGUAGES ASC CXX)

  add_executable(c_api_add_example
      c_api_add.asc
  )

  # ======================================================================================
  # NPU编译选项配置
  #
  # 说明：
  #   - 需根据实际部署的NPU硬件架构选择对应的`npu-arch`参数。
  # ======================================================================================
  target_compile_options(c_api_add_example PRIVATE
      $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
  )
  ```
  编译与执行示例：
  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake ..;make -j;             # 编译工程
  ./c_api_add_example           # 运行样例
  ```
  > [!NOTE]说明
  > - 编译选项`--npu-arch`用于指定NPU架构版本，`dav-`后面的数字为架构版本号，请替换为您实际使用的版本。各AI处理器型号与架构版本的对应关系请查阅[AI处理器型号和 \_\_NPU_ARCH\_\_ 的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)。

此外，基于C/C++不同层级的编程接口和不同的矢量计算类型，Add算子有多种实现方式，具体可参考下表：

| API层级 | 矢量计算类型 | Add算子实例 | 说明 |
|--------------|-----------|--------------|--------------|
| SIMD C API | 基于指针的Memory矢量计算 | [Memory矢量计算Add算子示例（同上述C API实现样例）](../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add/README.md) | 贴合C语言开发习惯，易于上手 |
| SIMD C API | 基于指针和Reg计算的Reg矢量计算 | [Reg矢量计算Add算子示例](../../../../../../examples/02_simd_c_api/00_introduction/04_reg_base_add_compute/c_api_simd_add/README.md) | 贴合C语言开发习惯，性能上限更高 |
| 基础API | 基于Tensor的Memory矢量计算 | [Memory矢量计算Add算子示例（同上述C++ Tensor实现样例）](../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md) | 匹配Tensor编程习惯，易于上手 |
| 基础API | 基于Tensor的Reg矢量计算 | [Reg矢量计算Add算子示例](../../../../../../examples/01_simd_cpp_api/00_introduction/04_reg_compute/add/README.md) | 匹配Tensor编程习惯，性能上限更高 |

<!-- npu="950" id6 -->
> [!NOTE]说明
> Ascend 950PR/Ascend 950DT新一代架构在传统[UB](../../../技术附录/概念原理和术语/术语表.md)缓存体系的基础上，开放了寄存器（Register）可编程能力，单个寄存器大小为256B。基于寄存器的矢量计算称为Reg矢量计算，而基于传统UB的矢量计算称为Memory矢量计算。
<!-- end id6 -->

若要深入理解Ascend C的SIMD与SIMT编程模型，请参阅[Ascend C编程模型概述](../../../编程指南/编程模型/编程模型概述.md)。
