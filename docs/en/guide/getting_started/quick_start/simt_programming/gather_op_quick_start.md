# Gather Operator Quick Start

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:17:40.212Z pushedAt=2026-11-03T07:05:13.264Z nl=2 -->

This quick start example implements a one-dimensional Gather operator based on Ascend C SIMT. It walks you through the complete process of implementing the device-side kernel function, calling it from the host side, and compiling and running the program, helping you build an overall understanding.

Before you begin, see [Environment Setup](../../environment_setup.md) to install the required CANN software package. For the complete sample, see [Gather](../../../../../../examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_1d).

- **Gather operator feature overview**:

    The mathematical expression of the Gather operator is:

    ```
    output[i] = input[index[i]]
    ```

    Computation logic: Based on each element in the index tensor `index`, collect the data at the corresponding position from the one-dimensional input vector `input`, and write it to the output tensor `output`.

- **Operator design**:

    - **Device-side kernel function programming APIs**
        - Kernel function definition: Declared with the [\_\_global\_\_](../../../programming_guide/language_extension/simt_builtin_keywords.md#section204112391232) modifier.
        - Data partitioning: Use the built-in variables [threadIdx](../../../programming_guide/language_extension/simt_builtin_keywords.md#li7760123814919), [blockIdx](../../../programming_guide/language_extension/simt_builtin_keywords.md#li1676053814914), and [blockDim](../../../programming_guide/language_extension/simt_builtin_keywords.md#li076017381191) to compute the thread index, and assign the data elements to be processed to each thread.
        - Data loading: No additional API is required. Data can be accessed directly through pointers.
        - Data computation: Read the input data based on the index values in `index` and the `[]` operator.
        - Data unloading: No additional API is required. Data can be accessed directly through pointers.
    - **Host-side runtime APIs**
        - Memory allocation: Use `aclrtMallocHost` to allocate host memory and `aclrtMalloc` to allocate device memory.
        - Data loading: Use `aclrtMemcpy` to copy the input data from host memory to device memory.
        - Starting the NPU computation task: Use the `<<<...>>>` syntax sugar to launch the kernel function.
        - Synchronization and waiting: Call `aclrtSynchronizeStream` to wait for the tasks on the current stream to complete, or call `aclrtSynchronizeDevice` to wait for all tasks on the device to complete.
        - Data unloading: Use `aclrtMemcpy` to copy the computation results from device memory back to host memory.

- **Operator code implementation**:

    Code files with the `*.asc` suffix contain both host-side and device-side code.

    - **Device-side kernel function implementation**:

        An example of the device-side part is as follows:

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

    - **Host-side code implementation**:

        The host side calls the device-side code through the `<<<>>>` syntax sugar.

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

- **Operator compilation and execution**:

    Example of the CMake configuration file:

    ```cmake
    cmake_minimum_required(VERSION 3.16)
    # find_package(ASC) is a CMake command used to find and configure the Ascend C compilation toolchain.
    find_package(ASC REQUIRED)
    # Specify that the project supports the ASC and CXX languages. ASC indicates that the BiSheng compiler is used to compile the Ascend C programming language.
    project(kernel_samples LANGUAGES ASC CXX)

    add_executable(demo
        gather_1d.asc
    )

    # Set the NPU architecture through compilation options.
    set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU ARCH, e.g. dav-3510")
    target_compile_options(demo PRIVATE
       $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
    )
    ```

    Compile and run:

    ```bash
    mkdir -p build && cd build;
    cmake ..;make -j;
    ./demo
    ```

    > [!NOTE] Note
    > - This sample supports the following models:
    >     <!-- npu="950" id1 -->
    >     - Ascend 950PR & 950DT products
    >     <!-- end id1 -->
    > - The compilation option `--npu-arch` specifies the NPU architecture version. The value after `dav-` is the architecture version number. Replace it with the version you actually use. For the architecture version corresponding to each AI processor model, see [the mapping between AI processor models and \_\_NPU\_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).
    > - The compilation option `--enable-simt` enables the SIMT programming scenario.

For more information about the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
