# SIMT Built-In Keywords<a name="ZH-CN_TOPIC_0000002477663934"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T15:15:28.842Z pushedAt=2026-11-03T07:05:13.474Z nl=2 -->

## Predefined Macros

SIMT provides built-in macros to facilitate program development. This section on predefined macros focuses on the macros frequently used in heterogeneous programming and explains their meanings.

-   <a id="npu-arch"></a>**\_\_NPU\_ARCH\_\_**

    \_\_NPU\_ARCH\_\_ is a preprocessing macro in the Device-side AI Core code, used to identify the architecture version of the AI processor. With this macro, developers can adapt and optimize code differently for different AI processors. The mapping between product models and NPU architecture versions is as follows:

    <!-- npu="950" id1 -->
    - Ascend 950PR&950DT products: 3510
    <!-- end id1 -->
    <!-- npu="A3" id2 -->
    - Atlas A3 products: Not supported
    <!-- end id2 -->
    <!-- npu="910b" id3 -->
    - Atlas A2 products: Not supported
    <!-- end id3 -->
    <!-- npu="310b" id4 -->
    - Atlas 200I/500 A2 inference products: Not supported
    <!-- end id4 -->
    <!-- npu="310p" id5 -->
    - Atlas inference products: Not supported
    <!-- end id5 -->
    <!-- npu="910" id6 -->
    - Atlas training products: Not supported
    <!-- end id6 -->

## Function Execution Space Qualifier<a name="section204112391232"></a>

A function execution space qualifier indicates whether a function is executed on the Host side or the Device side, as well as the space scope from which it can be called.

**Table 1** Function execution space qualifier overview

<a name="table121121062614"></a>

| Function Execution Space Qualifier | Execution Space (Host) | Execution Space (Device) | Allowed Calling Function Space (Host) | Allowed Calling Function Space (Device) |
| --- | --- | --- | --- | --- |
| \_\_host\_\_, no qualifier | √ | x | √ | x |
| \_\_aicore\_\_ | x | √ | x | √ |
| \_\_global\_\_ | x | √ | √ | x |

A function modified by \_\_global\_\_ is the entry point of a kernel function and is subject to the following usage constraints:

-   The return type of the function must be void, and it cannot be a member function of a class, struct, or union.
-   Recursive calls are not supported.
-   A call to a \_\_global\_\_ function is asynchronous. After the call, the host thread on the Host side returns immediately.
-   It can only be called by a Host-side function and is executed on the Device.

Functions modified by \_\_aicore\_\_ can only be executed on the Device side and can only be called by \_\_global\_\_ functions or other \_\_aicore\_\_ functions.

Functions modified by \_\_host\_\_ can only be called and executed on the Host side.

## Address Space Qualifier<a name="section20787845172517"></a>

The address space qualifier \_\_ubuf\_\_ is used to indicate the dynamic and static memory allocated on the Unified Buffer (UB). The size of static memory is determined at compile time, while the size of dynamic memory is determined when the kernel function is executed.

-   Static memory is allocated through arrays:

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   Dynamic memory is requested and used in the following ways:

    ```cpp
    extern __ubuf__ half dynamicBuf[]; 
    ```

    The actual memory size of dynamic memory must be configured when the kernel function is launched. For details, see [Kernel Function (Kernel) Configuration](#section97005415463).

## Built-in Constants<a name="section784531219338"></a>

<a name="table878553753515"></a>

| Constant Name | Value | Function |
| --- | --- | --- |
| constexpr uint64_t ASC_UB_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the UB capacity under the current AI processor architecture. It can be used to obtain the UB resource size at compile time. |

## Built-in Structure<a name="section13165113520576"></a>

-   dim3<a name="li1136665405"></a>

    dim3 is a built-in structure type consisting of three unsigned integers. The structure is defined as follows:

    ```cpp
    struct dim3{
        unsigned int x, y, z;
        dim3(unsigned int x, unsigned int y = 1, unsigned int z = 1)
            :x(x), y(y), z(z) {}
    };
    ```

    It is used to specify the sizes of three different dimensions. The total of the three dimensions is x \* y \* z. Developers can create a dim3 structure in the following ways.

    ```cpp
    dim3(x); // Create a one-dimensional structure, with dimy and dimz defaulting to 1
    dim3(x, y); // Create a two-dimensional structure, with dimz defaulting to 1
    dim3(x, y, z); // Create a three-dimensional structure.
    ```

## Built-in Variables<a name="section13165113520577"></a>


The following built-in variables of the dim3 structure are available only on the Device:

-   gridDim<a name="li20760123812911"></a>

    A built-in global variable that can be used only in a kernel function. It indicates the number of thread blocks that constitute the entire computing task in each dimension.
-   blockDim<a name="li076017381191"></a>

    A built-in global variable that can be used directly in a kernel function to obtain the three-dimensional hierarchy of threads configured in a thread block, that is, the value of the dim3 structure instance configured when the kernel function is launched. blockDim.x, blockDim.y, and blockDim.z represent the number of threads in the three dimensions of the thread block, respectively.

-   blockIdx<a name="li1676053814914"></a>

    A built-in global variable that can be used only in a kernel function to obtain the block index. It indicates the position coordinates of the thread block where the current thread resides in the entire grid.

    -   The value range of blockIdx.x is [0, gridDim.x - 1].
    -   The value range of blockIdx.y is [0, gridDim.y - 1].
    -   The range of blockIdx.z is [0, gridDim.z - 1].

-   threadIdx<a name="li7760123814919"></a>

    A built-in global variable that can be used directly in a kernel function to obtain the index of the current thread within the thread block. threadIdx.x, threadIdx.y, and threadIdx.z represent the indices of the current thread in the three dimensions, respectively. The range of threadIdx.x is \[0, blockDim.x\), the range of threadIdx.y is \[0, blockDim.y\), and the range of threadIdx.z is \[0, blockDim.z\). The correspondence between the thread index within a thread block and the thread ID is as follows:

    -   For a one-dimensional thread block, the thread ID is blockIdx.x \* blockDim.x + threadIdx.x.

        ![](../../figures/thread_id_1.png)

    -   For a two-dimensional thread block, the thread ID is a two-dimensional structure, and its calculation formula is as follows:

        ```cpp
        thread_id_x = blockIdx.x * blockDim.x + threadIdx.x;
        thread_id_y = blockIdx.y * blockDim.y + threadIdx.y;
        ```

    -   For a three-dimensional thread block, the thread ID is a three-dimensional structure, and its calculation formula is as follows:

        ```cpp
        thread_id_x = blockIdx.x * blockDim.x + threadIdx.x;
        thread_id_y = blockIdx.y * blockDim.y + threadIdx.y;
        thread_id_z = blockIdx.z * blockDim.z + threadIdx.z;
        ```

The following built-in variables of the int type are available only on the Device:

-   warpSize

    A variable of the const int32_t type, indicating the number of threads in a warp. The current value is fixed at 32.

## Built-in Data Types<a name="section1835494915576"></a>

A set of data types applicable to the Device side is provided, including scalars and short vectors. A short vector is a simple vector composed of multiple elements. The following data types are supported by the compiler by default and can be used without including any header file.

**Table 2** Scalar data types

<a name="table114921244133211"></a>

| Type | Data Type | Description | Size (Bit) | Value Range |
| --- | --- | --- | --- | --- |
| Boolean | bool | All zeros represent false; otherwise, true. | 8 | true, false |
| Integer | uint8_t | unsigned char | 8 | [0, 255] |
| Integer | int8_t | signed char | 8 | [-128, 127] |
| Integer | uint16_t | unsigned short | 16 | [0, 65535] |
| Integer | int16_t | signed short | 16 | [-32768, 32767] |
| Integer | uint32_t | unsigned int | 32 | [0, 4294967295] |
| Integer | int32_t | signed int | 32 | [-2147483648, 2147483647] |
| Integer | uint64_t | unsigned long | 64 | [0,18446744073709551615] |
| Integer | int64_t | signed long | 64 | [-9223372036854775808, 9223372036854775807] |
| Floating-point | float8_e4m3_t | Sign bit width 1, exponent bit width 4, mantissa bit width 3 | 8 | [2<sup>6</sup> - 2<sup>9</sup>, 2<sup>9</sup> - 2<sup>6</sup>] |
| Floating-point | float8_e5m2_t | Sign bit width 1, exponent bit width 5, mantissa bit width 2 | 8 | [2<sup>13</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>13</sup>] |
| Floating-point | hifloat8_t | Sign bit width 1, point field width 2, exponent and mantissa bit widths determined by the point field encoding | 8 | The point field encoding determines the data precision and value range |
| Floating-point | half | Sign bit width 1, exponent bit width 5, mantissa bit width 10 | 16 | [2<sup>5</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>5</sup>] |
| Floating-point | bfloat16_t | Sign bit width 1, exponent bit width 8, mantissa bit width 7 | 16 | [2<sup>120</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>120</sup>] |
| Floating-point | float | Sign bit width 1, exponent bit width 8, mantissa bit width 23 | 32 | [2<sup>104</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>104</sup>] |

Short vector data types are classified into Vector X2, Vector X3, and Vector X4, indicating that a short vector variable has 2, 3, or 4 elements. The currently supported types are as follows:

<a name="table49986503175"></a>

| Element Data Type | Vector X2 | Vector X3 | Vector X4 |
| --- | --- | --- | --- |
| unsigned char | uchar2 | uchar3 | uchar4 |
| signed char | char2 | char3 | char4 |
| unsigned short (16bit) | ushort2 | ushort3 | ushort4 |
| signed short (16bit) | short2 | short3 | short4 |
| unsigned int | uint2 | uint3 | uint4 |
| signed int | int2 | int3 | int4 |
| Unsigned long integer (64bit) | ulonglong2 | ulonglong3 | ulonglong4 |
| Signed long integer (64bit) | longlong2 | longlong3 | longlong4 |
| Unsigned long integer (64bit) | ulong2 | ulong3 | ulong4 |
| Signed long integer (64bit) | long2 | long3 | long4 |
| Floating-point, 1 sign bit, 2 exponent bits, 1 mantissa bit | float4_e2m1x2_t | - | - |
| Floating-point, 1 sign bit, 1 exponent bit, 2 mantissa bits | float4_e1m2x2_t | - | - |
| Floating-point, 1 sign bit, 4 exponent bits, 3 mantissa bits | float8_e4m3x2_t | - | - |
| Floating-point, 1 sign bit, 5 exponent bits, 2 mantissa bits | float8_e5m2x2_t | - | - |
| Floating-point hif8 | hifloat8x2_t | - | - |
| Floating-point, 1 sign bit, 5 exponent bits, 10 mantissa bits | half2 | - | - |
| Floating-point, 1 sign bit, 8 exponent bits, 7 mantissa bits | bfloat16x2_t | - | - |
| Floating-point, 1 sign bit, 8 exponent bits, 23 mantissa bits | float2 | float3 | float4 |

**Table 3** Short vector data types

<a name="table13856144241"></a>

| Data Type | Memory Size (Bytes) | Address Alignment (Bytes) |
| --- | --- | --- |
| char2,  uchar2 | 2 | 2 |
| char3,  uchar3,  char4,  uchar4 | 4 | 4 |
| short2,  ushort2 | 4 | 4 |
| short3,  ushort3,  short4,  ushort4 | 8 | 8 |
| int2,  uint2 | 8 | 8 |
| int3,  uint3,  int4,  uint4 | 16 | 16 |
| long2,  ulong2 | 16 | 16 |
| long3,  ulong3,  long4,  ulong4 | 32 | 32 |
| longlong2,  ulonglong2 | 16 | 16 |
| longlong3,  ulonglong3,  longlong4,  ulonglong4 | 32 | 32 |
| float2 | 8 | 8 |
| float3,  float4 | 16 | 16 |
| float4_e2m1x2_t,  float4_e1m2x2_t | 1 | 1 |
| float8_e4m3x2_t,  float8_e5m2x2_t,  hifloat8x2_t | 2 | 2 |
| half2, bfloat16x2_t | 4 | 4 |

## Operators<a name="section186787252339"></a>

SIMT programming provides a set of operators for performing mathematical operations. The following lists the supported operators.

**Table 4**  Operators supported by SIMT programming

<a name="table11745172843710"></a>

| Category | Operator | bool | int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t | half/bfloat16_t/float | half2/bfloat16x2_t | hifloat8_t |
| --- | --- | --- | --- | --- | --- | --- |
| Arithmetic operator | + | x | √ | √ | √ | x |
| Arithmetic operator | - | x | √ | √ | √ | x |
| Arithmetic operator | * | x | √ | √ | √ | x |
| Arithmetic operator | / | x | √ | √ | √ | x |
| Arithmetic operator | % | x | √ | x | x | x |
| Arithmetic operator | ++ | x | √ | √ | √ | x |
| Arithmetic operator | -- | x | √ | √ | √ | x |
| Arithmetic operator | - (negation) | x | √ | √ | √ | x |
| Comparison operator | &lt; | x | √ | √ | x | x |
| Comparison operator | &lt;= | x | √ | √ | x | x |
| Comparison operator | &gt; | x | √ | √ | x | x |
| Comparison operator | &gt;= | x | √ | √ | x | x |
| Comparison operator | == | x | √ | √ | x | x |
| Comparison operator | != | x | √ | √ | x | x |
| Bitwise operator | & | x | √ | x | x | x |
| Bitwise operator | \| | x | √ | x | x | x |
| Bitwise operator | ^ | x | √ | x | x | x |
| Bitwise operator | ~ | x | √ | x | x | x |
| Bitwise operator | &lt;&lt; | x | √ | x | x | x |
| Bitwise operator | &gt;&gt; | x | √ | x | x | x |
| Logical operator | && | √ | √ | √ | x | x |
| Logical operator | \|\| | √ | √ | √ | x | x |
| Logical operator | ! | √ | √ | √ | x | x |
| Conditional operator | a ? b : c | √ | √ | √ | √ | x |

> **Note:** The division operator (/) currently does not support Subnormal inputs (where the exponent bits are all 0 and the mantissa bits are not all 0, representing a very small value close to zero). When the input is a Subnormal value, it is flushed to zero.

The following shows examples of using operators:

```cpp
// Addition operation.
res[idx] = x[idx] + y[idx]; 

// Negation operation.
x[idx] = (-x[idx]);

// Comparison operation.
if (x[idx] > y[idx]) {
    res[idx] = x[idx];
} else {
    res[idx] = y[idx];
}

// Bitwise AND operation
res[idx] = x[idx] & y[idx];

// Logical OR operation
if (x[idx] || y[idx]) {
    res[idx] = 1;
}

// Conditional operation
res[idx] = x[idx] > y[idx] ? x[idx] : y[idx];
```

## Kernel Function Configuration<a name="kernel-function-configuration"></a><a name="section97005415463"></a>

When calling a function modified by the __global__ qualifier, the execution configuration must be specified. The execution configuration is specified by inserting an expression in the following form between the function name and the parenthesized argument list:

```cpp
<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>
```

Where:

-   blocks_per_grid: dim3 type, used to specify the dimensions and size of the grid. blocks_per_grid.x * blocks_per_grid.y * blocks_per_grid.z is equal to the total number of launched thread blocks, which cannot exceed 65535.
-   threads_per_block: dim3 type, used to specify the dimensions and size of each thread block. threads_per_block.x * threads_per_block.y * threads_per_block.z is equal to the number of threads in each thread block, which must be less than or equal to the __launch_bounds__ configuration.
-   dyn\_ubuf\_size: size\_t type, used to specify the size of shared memory dynamically allocated by each thread block, in bytes. This memory is used for arrays. For details, see the "dynamic allocation" method in [Shared Memory](../programming_model/ai_core_simt_programming/memory_hierarchy.md#shared-memory).
-   stream: aclrtStream type, specifies the associated stream, used to maintain the execution order of asynchronous operations.

The following example shows how to declare and call a kernel function.

```cpp
// Declare
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length);
// Call
uint32_t blocks_per_grid = 48; // Number of thread blocks (Grid size)
uint32_t threads_per_block = 256; // Number of threads per block (Block size)
size_t dyn_ubuf_size = 1024; // need 1024 Byte dynamic memory
add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(x, y, z, 1024); // blocks_per_grid and threads_per_block are implicitly converted to the dim3 type
```

Before the function is executed, the preceding configuration parameters are validated. If blocks_per_grid or threads_per_block exceeds the maximum allowed scale of the device, or if dyn_ubuf_size exceeds the remaining shared memory available after static memory is allocated, the function will fail to execute.

The number of registers used by a kernel function determines the maximum number of threads that can be launched in a single thread block. The number of registers used by a kernel function is specified by the \_\_launch\_bounds\_\_\(\) qualifier or the \_\_maxnreg\_\_\(\) qualifier.

Pay attention to the following constraints when using the preceding optional qualifiers:
-   \_\_launch\_bounds\_\_ or\_\_maxnreg\_\_ can be used only in \_\_global\_\_ functions.
-   The same function cannot be configured with both \_\_launch\_bounds\_\_ and \_\_maxnreg\_\_.

During concurrent execution of multiple threads, using fewer registers per thread allows more threads to reside on the AI processor. Therefore, the compiler uses a heuristic algorithm to keep register spilling and the number of instructions at a minimum while reducing register usage as much as possible. An application can limit the launch bounds by using the \_\_launch\_bounds\_\_\(\) qualifier in the \_\_global\_\_ function definition, providing additional information to help the compiler optimize this process. This is an optional configuration.

-   \_\_launch\_bounds\_\_\(N\) <a name="li23861114618"></a>
  
    Function marking macro, optionally configured on a kernel function, used to specify the maximum number of threads launched by the kernel function. The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see the following table. Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as register spilling are likely to occur. It is recommended that the maximum number of threads be consistent with the dim3 thread count used when launching the kernel function.

    **Table 5**  \_\_launch\_bounds\_\_(N) Relationship with the number of registers available to each thread
    
    <a name="table1715318510594"></a>

    | N | Number of Registers Available to Each Thread |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

    Configure the maximum number of threads for the SIMT function to 512 and the number of registers available to each thread to 64, as shown in the following example:

    ```cpp
    __global__ __launch_bounds__(512) inline void add(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
    ```
    The parameter N of \_\_launch\_\_bounds\_\_\(N\) must satisfy:
    -   N >= dimx * dimy * dimz; dimx, dimy, and dimz are the dim3 structure representing threads.
    -   The value range of N is from 1 to 2048.
    -   If \_\_launch\_bounds\_\_ is not configured, the maximum number of threads defaults to 1024.

To support low-level performance tuning, applications can use the \_\_maxnreg\_\_\(\) qualifier in a \_\_global\_\_ function definition to convey performance tuning intent to the compiler. This qualifier directly limits the maximum number of registers that can be allocated to a single thread within a thread block.

-   \_\_maxnreg\_\_\(N\) <a name="section_maxnreg"></a>

    Function marking macro, optionally configured on a kernel function, used to specify at compile time the maximum number of registers that a single thread can be allocated within a thread block.

    The parameter N of \_\_maxnreg\_\_\(N\) must satisfy the following:
    -  The value of N is an integer in the range (0, 128].
    -   If the input value N is in the range (0, 16], each thread can use up to 16 registers; if the input value N is in the range (16, 32], each thread can use up to 32 registers; if the input value N is in the range (32, 64], each thread can use up to 64 registers; if the input value N is in the range (64, 128], each thread can use up to 127 registers;
    -   If \_\_maxnreg\_\_ is not configured, the maximum number of registers allocatable to a single thread defaults to 32.

    The maximum number of registers available to each thread imposes a limit on the number of threads actually launched per block. For the specific correspondence, see the following table.

    **Table 6**  Maximum number of registers allocatable per thread and number of threads actually launchable per block for \_\_maxnreg\_\_

    | Maximum Registers per Thread | Startable Threads per Thread |
    | --- | --- |
    | 16 | 1~2048 |
    | 32 | 1~1024 |
    | 64 | 1~512 |
    | 127 | 1~256 |

    The following example configures the maximum number of registers allocatable to a single thread of a SIMT function to 64:

    ```cpp
    __global__ __maxnreg__(64) void add(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
    ```
