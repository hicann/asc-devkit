# Add Operator Quick Start<a name="ZH-CN_TOPIC_0000002500781060"></a>

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T11:14:17.546Z pushedAt=2026-11-03T07:05:13.261Z nl=2 -->

This example is an introductory practice that implements the Add operator based on Ascend C SIMD to help you get started quickly. It presents the complete process of Device-side kernel function implementation, Host-side invocation, and compilation and running, helping you build an overall understanding. Before you begin, install the required CANN software package by referring to [Environment Setup](../../environment_setup.md).

The following describes the Add operator implementation based on the C API and the C++ API respectively. For the complete sample code, see [Add Operator Example Based on the C API](../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add/README.md) and [Add Operator Example Based on the C++ API](../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md).

- **Add operator feature overview**:

  The mathematical expression of the Add operator is:

  ![](../../../figures/zh-cn_formulaimage_0000002501241896.png)

  The computation logic is to complete `z = x + y` element by element.

- **Operator design**

    - **Device-side kernel gunction programming APIs**
        - Kernel function definition: declared through the [\_\_global\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md) modifier.
        - Data tiling: Use the built-in keyword [block_idx](../../../programming_guide/language_extension/simd_builtin_keywords.md) to determine the data processed by each block.
        - Data copy-in: Use the [C API](../../../programming_guide/language_extension/simd_language_extension_c_api.md) `asc_copy_gm2ub` or the [C++ API](../../../programming_guide/library_api/programming_interface_overview.md) `AscendC::DataCopy`.
        - Data computation: Use the C API `asc_add` or the C++ API `AscendC::Add`.
        - Data copy-out: Use the C API `asc_copy_ub2gm` or the C++ API `AscendC::DataCopy`.
    - **Host-side runtime APIs**
        - Memory allocation: Use `aclrtMallocHost` to allocate host memory and `aclrtMalloc` to allocate device memory.
        - Data copy-in: Use `aclrtMemcpy` to copy the input data from host memory to device memory.
        - Start the NPU computation task: Use the `<<<...>>>` syntactic sugar to launch the kernel function.
        - Synchronization and waiting: Call `aclrtSynchronizeStream` or `aclrtSynchronizeDevice` to wait for the task to complete.
        - Data copy-out: Use `aclrtMemcpy` to copy the computation result from device memory back to host memory.
      
      > [!NOTE] Note
      > - See the technical appendix in [Ascend C Overview and Learning Path](../../ascend_c_overview_and_learning_path.md) for the links to the *Ascend C API Reference* and *CANN Runtime APIs* to view more API information.

- **Operator code implementation**:

  Code files with the `*.asc` extension contain both host-side and device-side code.

  - **Device-side kernel function implementation**:
  The device-side example is as follows:
    - **Example of implementing memory vector calculation based on the C API**
      ```cpp
      __vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
      {
          asc_init();

          constexpr uint32_t block_length = TOTAL_LENGTH / NUM_BLOCKS;
          
          // Determine the data that each block needs to process
          __gm__ float* x_gm = x + block_idx * block_length;
          __gm__ float* y_gm = y + block_idx * block_length;
          __gm__ float* z_gm = z + block_idx * block_length;

          // Allocate on-chip Unified Buffer (UB) memory
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
      > [!NOTE] Note
      > - This Memory vector calculation example supports the following models:
      >     <!-- npu="A3" id1 -->
      >     - Atlas A3 products
      >     <!-- end id1 -->
      >     <!-- npu="910b" id2 -->
      >     - Atlas A2 products
      >     <!-- end id2 -->
      > - The kernel functionof a SIMD operator requires an additional modifier. The [`__vector__`](../../../programming_guide/language_extension/simd_builtin_keywords.md) modifier indicates that the operator is executed only on the vector calculation unit.
      > - **Performance tip**: To simplify synchronization operations, the example uses `asc_sync` uniformly. In actual operator development, you are advised to use specific synchronization control instructions based on the pipeline execution to achieve better performance. For details, see the [Synchronization Mechanism](../../../programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md#synchronization-mechanism) section.

    - **Example of implementing memory vector calculation based on C++ tensor**

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
      > [!NOTE] Note
      > - This example supports the following models:
      >     <!-- npu="950" id3 -->
      >     - Ascend 950PR&950DT products
      >     <!-- end id3 -->
      >     <!-- npu="A3" id4 -->
      >     - Atlas A3 products
      >     <!-- end id4 -->
      >     <!-- npu="910b" id5 -->
      >     - Atlas A2 products
      >     <!-- end id5 -->
      > - The kernel function of a SIMD operator requires an additional modifier. The [`__vector__`](../../../programming_guide/language_extension/simd_builtin_keywords.md) modifier indicates that the operator is executed only on the vector calculation unit.

  - **Host-side code implementation**:

    The host side calls the device-side kernel function through the `<<<>>>` syntax sugar. The sample code snippet is as follows:
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

          // Example: Call add kernel which is implemented by SIMD C++ basic API.
          add_custom<blockLength><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);

          // Wait for the add_custom kernel to complete
          aclrtSynchronizeStream(stream);

          // Copy the result from device memory to host memory
          aclrtMemcpy(zHost, totalByteSize, zDevice, totalByteSize, ACL_MEMCPY_DEVICE_TO_HOST);
          ...
      }
    ```

- **Operator compilation and execution**:

  Example of the CMake configuration file:
  ```cmake
  cmake_minimum_required(VERSION 3.16)

  find_package(ASC REQUIRED)

  project(kernel_samples LANGUAGES ASC CXX)

  add_executable(c_api_add_example
      c_api_add.asc
  )

  # ======================================================================================
  # NPU compilation option configuration
  #
  # Note:
  #   - Select the corresponding `npu-arch` parameter based on the NPU hardware architecture actually deployed.
  # ======================================================================================
  target_compile_options(c_api_add_example PRIVATE
      $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
  )
  ```
  Compilation and execution example:
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake ..;make -j;             # Compile the project.
  ./c_api_add_example           # Run the sample.
  ```
  > [!NOTE] Note
  > - The compilation option `--npu-arch` specifies the NPU architecture version. The number after `dav-` is the architecture version number. Replace it with the version you actually use. For the mapping between AI processor models and **\_\_NPU_ARCH\_\_**, see [Mapping Between AI Processor Models and \_\_NPU_ARCH\_\_](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

In addition, based on C/C++ programming APIs at different levels and different vector calculation types, the Add operator can be implemented in multiple ways. For details, see the following table:

| API Level | Vector Computation Type | Add Operator Example | Description |
|--------------|-----------|--------------|--------------|
| SIMD C API | Pointer-based memory vector computation | [Memory Vector Computation Add Operator Example (Same as the C API Implementation Sample Above)](../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add/README.md) | Follows C language development habits and is easy to get started with |
| SIMD C API | Reg vector computation based on pointer and Reg computation | [Reg Vector Computation Add Operator Example](../../../../../../examples/02_simd_c_api/00_introduction/04_reg_base_add_compute/c_api_simd_add/README.md) | Follows C language development habits and offers a higher performance ceiling |
| Basic API | Tensor-based memory vector computation | [Memory Vector Computation Add Operator Example (Same as the C++ Tensor Implementation Sample Above)](../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add/README.md) | Matches Tensor programming habits and is easy to get started with |
| Basic API | Tensor-based Reg vector computation | [Reg Vector Computation Add Operator Example](../../../../../../examples/01_simd_cpp_api/00_introduction/04_reg_compute/add/README.md) | Matches Tensor programming habits and offers a higher performance ceiling |

<!-- npu="950" id6 -->
> [!NOTE] Note
> The new-generation architecture of the Ascend 950PR&950DT products open up register programmability on top of the traditional [UB](../../../technical_appendix/concepts_and_terms/glossary.md) cache system. A single register is 256B in size. Register-based vector computation is called Reg vector computation, while vector computation based on the traditional UB is called memory vector computation.
<!-- end id6 -->

To gain a deeper understanding of the SIMD and SIMT programming models of Ascend C, see [Ascend C Programming Model Overview](../../../programming_guide/programming_model/programming_model_overview.md).
