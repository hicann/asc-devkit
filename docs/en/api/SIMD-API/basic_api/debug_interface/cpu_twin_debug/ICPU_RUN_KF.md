# ICPU_RUN_KF<a name="ZH-CN_TOPIC_0000002080882157"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:09:42.588Z pushedAt=2026-10-26T07:41:44.375Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/cpu_twin_debug/ICPU_RUN_KF_res.md#id1 -->

## Description<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section259105813316"></a>

The header file path is `"tools/cpudebug/include/kern_fwk.h"`.

When performing CPU-side run verification of a kernel function, this serves as the main CPU debugging entry point and completes the invocation of the operator program on the CPU side.

## Prototype<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section2067518173415"></a>

```cpp
#define ICPU_RUN_KF(func, numBlocks, ...)
```

## Parameters<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section158061867342"></a>

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| **func** | Input | Kernel function pointer of the operator. |
| **numBlocks** | Input | Number of cores of the operator, corenum. |
| ... | Input | All input and output parameters, filled in sequence. The current number of parameters is limited to 32. A compilation error occurs when it exceeds 32. |

## Return Value<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile with [<<<>>>](../../../../../guide/programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md).
- Except for **func** and **blkdim**, all other variables must be pointers to shared memory allocated by [GmAlloc](GmAlloc.md); the number and order of the passed parameters must be consistent with the kernel.

## Example<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section82241477610"></a>

The following code uses the **add_custom** operator as an example to describe how to write the application that invokes the operator when the operator kernel function is verified on the CPU side. When implementing your own application, pay attention to the modifications caused by different operator kernel functions, including the operator kernel function name and the differences in input and output parameters, and properly arrange the corresponding memory allocation, memory copy, and file read/write operations. The calling methods of the related APIs can be directly reused.

1.  Include header files as needed, and use the **ASCENDC_CPU_DEBUG** macro to distinguish the header files to be included on the CPU and NPU sides.

    ```cpp
    #include "data_utils.h"
    #ifndef ASCENDC_CPU_DEBUG
    #include "acl/acl.h"
    #else
    #include "tikicpulib.h"
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z); // Kernel Function declaration.
    #endif
    ```

2.  CPU-side run verification. The steps to complete the CPU-side run verification of the operator kernel function are as follows:

    **Figure 1**  CPU-side run test steps<a name="fig13576112114442"></a>  
    ![](../../../../figures/cpu_verification_steps.png "CPU-side run test steps")

    ```cpp
    int32_t main(int32_t argc, char* argv[])
    {
        uint32_t numBlocks = 8;
        size_t inputByteSize = 8 * 2048 * sizeof(uint16_t);
        size_t outputByteSize = 8 * 2048 * sizeof(uint16_t);
    
       // Use GmAlloc to allocate shared memory and initialize the data.
        uint8_t* x = (uint8_t*)AscendC::GmAlloc(inputByteSize);
        uint8_t* y = (uint8_t*)AscendC::GmAlloc(inputByteSize);
        uint8_t* z = (uint8_t*)AscendC::GmAlloc(outputByteSize);
    
        ReadFile("./input/input_x.bin", inputByteSize, x, inputByteSize);
        ReadFile("./input/input_y.bin", inputByteSize, y, inputByteSize);
        // For vector operators, set the kernel mode to AIV mode.
        AscendC::SetKernelMode(KernelMode::AIV_MODE);
        // Call the ICPU_RUN_KF debug macro to complete the Kernel Function call on the CPU side.
        ICPU_RUN_KF(add_custom, numBlocks, x, y, z);
        // Write out the output data.
        WriteFile("./output/output_z.bin", z, outputByteSize);
        // Call GmFree to release the requested resources.
        AscendC::GmFree((void *)x);
        AscendC::GmFree((void *)y);
        AscendC::GmFree((void *)z);
        return 0;
    }
    ```
