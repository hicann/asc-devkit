# Gather算子快速入门

本入门示例基于Ascend C SIMT实现一维Gather算子，带你快速上手实践，涵盖Device端核函数实现、Host端调用以及编译运行的完整流程，帮助开发者建立整体认知。

开始前请参考[环境准备](../../environment_setup.md)安装所需的CANN软件包，完整样例请见[Gather](../../../../../../examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_1d)。

- **Gather算子功能介绍**：

    Gather算子的数学表达式为：

    ```
    output[i] = input[index[i]]
    ```

    计算逻辑：根据索引张量`index`中的每个元素，从一维输入向量`input`中采集对应位置的数据，并写入输出张量`output`。

- **算子设计**：

    - **Device端核函数编程接口**
        - 核函数定义：通过[\_\_global\_\_](../../../编程指南/语言扩展层/SIMT-BuiltIn关键字.md#section204112391232)修饰符声明。
        - 数据划分：使用内置变量[threadIdx](../../../编程指南/语言扩展层/SIMT-BuiltIn关键字.md#li7760123814919)、[blockIdx](../../../编程指南/语言扩展层/SIMT-BuiltIn关键字.md#li1676053814914)、[blockDim](../../../编程指南/语言扩展层/SIMT-BuiltIn关键字.md#li076017381191)计算线程索引，并为每个线程分配需要处理的数据元素。
        - 数据搬入：无需额外接口，直接通过指针访问即可。
        - 数据计算：根据`index`中的索引值及操作符`[]`读取输入数据。
        - 数据搬出：无需额外接口，直接通过指针访问即可。
    - **Host端运行时接口**
        - 内存分配：使用`aclrtMallocHost`分配Host Memory，`aclrtMalloc`分配Device Memory。
        - 数据搬入：使用`aclrtMemcpy`将输入数据从Host Memory拷贝到Device Memory。
        - 启动NPU计算任务：通过`<<<...>>>`语法糖启动核函数。
        - 同步等待：调用`aclrtSynchronizeStream`等待当前Stream上的任务完成，或调用`aclrtSynchronizeDevice`等待Device上所有任务完成。
        - 数据搬出：使用`aclrtMemcpy`将计算结果从Device Memory拷贝回Host Memory。

- **算子代码实现**：

    后缀名为`*.asc`的代码文件包含Host端与Device端代码。

    - **Device端Kernel实现**：

        Device端部分示例如下：

        ```cpp
        __global__ void gather_1d_custom(float* input, int32_t* index, float* output, uint64_t index_total_length)
        {
            // Calculate global thread ID
            int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;

            // Maps to the index of output tensor
            if (idx >= index_total_length) {
                return;
            }
            output[idx] = input[index[idx]];
        }
        ```

    - **Host端代码实现**：

        Host端通过<<<>>>语法糖调用Device端代码。

        ```cpp
        std::vector<float> gather_1d(std::vector<float>& input, std::vector<int32_t>& index)
        {
            ...
            aclrtCreateStream(&stream);
            // Allocate host and device memory, and copy input data from host to device
            aclrtMallocHost((void **)(&output_host), output_total_byte_size);
            aclrtMalloc((void **)&input_device, input_total_byte_size, ACL_MEM_MALLOC_HUGE_FIRST);
            aclrtMalloc((void **)&index_device, index_total_byte_size, ACL_MEM_MALLOC_HUGE_FIRST);
            aclrtMalloc((void **)&output_device, output_total_byte_size, ACL_MEM_MALLOC_HUGE_FIRST);
            aclrtMemcpy(input_device, input_total_byte_size, input_host, input_total_byte_size, ACL_MEMCPY_HOST_TO_DEVICE);
            aclrtMemcpy(index_device, index_total_byte_size, index_host, index_total_byte_size, ACL_MEMCPY_HOST_TO_DEVICE);

            // Configure kernel launch parameters.
            // In this sample, index.size() is 48 * 256, so 48 blocks and 256 threads per block
            // cover one output element per thread. For general input lengths, use
            // blocks_per_grid = (index.size() + threads_per_block - 1) / threads_per_block.
            uint32_t blocks_per_grid = 48; // Number of thread blocks (Grid size)
            uint32_t threads_per_block = 256; // Number of threads per block (Block size)
            uint32_t dyn_ubuf_size = 0;  // No dynamic memory required in this sample

            // Launch kernel <<<grid_dim, block_dim, dynamic_memory_size, stream>>>
            gather_1d_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(
                input_device, index_device, output_device, index.size());

            // Wait for the gather_1d_custom kernel to complete
            aclrtSynchronizeStream(stream);

            // Copy the result from device memory to host memory
            aclrtMemcpy(output_host, output_total_byte_size, output_device, output_total_byte_size, ACL_MEMCPY_DEVICE_TO_HOST);
            std::vector<float> output((float *)output_host, (float *)(output_host + output_total_byte_size));
            ...
        }
        ```

- **算子编译与运行**：

    CMake配置文件示例：

    ```cmake
    cmake_minimum_required(VERSION 3.16)
    # find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
    find_package(ASC REQUIRED)
    # 指定项目支持的语言包括ASC和CXX，ASC表示支持使用毕昇编译器对Ascend C编程语言进行编译
    project(kernel_samples LANGUAGES ASC CXX)

    add_executable(demo
        gather_1d.asc
    )

    # 通过编译选项设置NPU架构
    set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU ARCH, e.g. dav-3510")
    target_compile_options(demo PRIVATE
       $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
    )
    ```

    编译并执行：

    ```bash
    mkdir -p build && cd build;
    cmake ..;make -j;
    ./demo
    ```

    > [!NOTE]说明
    > - 该样例支持以下型号：
    >     <!-- npu="950" id1 -->
    >     - Ascend 950PR/Ascend 950DT
    >     <!-- end id1 -->
    > - 编译选项`--npu-arch`用于指定NPU架构版本，`dav-`后为架构版本号，请替换为您实际使用的版本。各AI处理器型号对应的架构版本号请通过[AI处理器型号和 \_\_NPU\_ARCH\_\_ 的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)查询。
    > - 编译选项`--enable-simt`用于启用SIMT编程场景。

如需进一步了解Ascend C的SIMD与SIMT编程模型，请参阅[Ascend C编程模型概述](../../../编程指南/编程模型/编程模型概述.md)。
