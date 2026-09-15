# ICPU_RUN_KF<a name="ZH-CN_TOPIC_0000002080882157"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T11:34:30.379Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section259105813316"></a>

The header file path is `"tools/cpudebug/include/kern_fwk.h"`.

Serves as the overall CPU debugging entry point for invoking the operator program on the CPU side during CPU-side run verification of the kernel function.

## Prototype<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section2067518173415"></a>

```cpp
#define ICPU_RUN_KF(func, numBlocks, ...)
```

## Parameters<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section158061867342"></a>

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| func | Input | Kernel function pointer of the operator. |
| numBlocks | Input | Number of cores of the operator, that is, corenum. |
| ... | Input | All input and output parameters, filled in sequentially. The current number of parameters is limited to 32; exceeding 32 causes a compilation error. |

## Return Value<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile and use [<<<>>>](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/Debugging and Tuning/Function Debugging/CPU Domain Twin Debugging.md).
- Except for **func** and **blkdim**, all other variables must be pointers to shared memory allocated through [GmAlloc](GmAlloc.md). The number and order of the passed parameters must be consistent with those of **kernel**.

## Example<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section82241477610"></a>

The following code uses the **add_custom** operator as an example to show how to write the application that invokes the operator when the operator kernel function is verified on the CPU side. When implementing your own application, you need to pay attention to the modifications caused by differences in the operator kernel function, including the operator kernel function name and the differences in input and output parameters, and properly arrange the corresponding memory allocation, memory copy, and file read/write operations. The calling methods of the related APIs can be reused directly.

1.  Include the required header files as needed, and use the **ASCENDC_CPU_DEBUG** macro to distinguish the header files to be included on the CPU side and the NPU side.

    ```cpp
    #include "data_utils.h"
    #ifndef ASCENDC_CPU_DEBUG
    #include "acl/acl.h"
    #else
    #include "tikicpulib.h"
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z); // Declare the kernel function.
    #endif
    ```

2.  Run verification on the CPU side. The steps to complete the CPU-side run verification of the operator kernel function are as follows:

    **Figure 1**  CPU Side Run Verification Steps<a name="fig13576112114442"></a>  
    ![](../../../../figures/cpu_run_validation_steps.png "CPU Side Run Verification Steps")

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
        // Set the kernel mode to AIV mode for vector operators.
        AscendC::SetKernelMode(KernelMode::AIV_MODE);
        // Call the ICPU_RUN_KF debugging macro to invoke the kernel function on the CPU side.
        ICPU_RUN_KF(add_custom, numBlocks, x, y, z);
        // Write the output data.
        WriteFile("./output/output_z.bin", z, outputByteSize);
        // Call GmFree to release the allocated resources.
        AscendC::GmFree((void *)x);
        AscendC::GmFree((void *)y);
        AscendC::GmFree((void *)z);
        return 0;
    }
    ```
