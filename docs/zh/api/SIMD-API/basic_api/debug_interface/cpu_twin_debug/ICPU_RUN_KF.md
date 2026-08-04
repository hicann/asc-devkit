# ICPU_RUN_KF<a name="ZH-CN_TOPIC_0000002080882157"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section259105813316"></a>

头文件路径为：`"tools/cpudebug/include/kern_fwk.h"`。

进行核函数的CPU侧运行验证时，CPU调测总入口，完成CPU侧的算子程序调用。

## 函数原型<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section2067518173415"></a>

```cpp
#define ICPU_RUN_KF(func, numBlocks, ...)
```

## 参数说明<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section158061867342"></a>

**表1**  模板参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| func | 输入 | 算子的kernel函数指针。|
| numBlocks | 输入 | 算子的核心数，corenum。|
| ... | 输入 | 所有的入参和出参，依次填入，当前参数个数限制为32个，超出32时会出现编译错误。 |

## 返回值说明<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section794123819592"></a>

- 为了保留接口兼容，推荐[<<<>>>](../../../../../guide/编程指南/调试调优/功能调试/CPU域孪生调试.md)编译使用。
- 除了func、blkdim以外，其他的变量都必须是通过[GmAlloc](GmAlloc.md)分配的共享内存的指针；传入的参数的数量和顺序都必须和kernel保持一致。

## 调用示例<a name="zh-cn_topic_0000001963799134_zh-cn_topic_0000001541924164_section82241477610"></a>

下面代码以add_custom算子为例，介绍算子核函数在CPU侧验证时，算子调用的应用程序如何编写。您在实现自己的应用程序时，需要关注由于算子核函数不同带来的修改，包括算子核函数名，入参出参的不同等，合理安排相应的内存分配、内存拷贝和文件读写等，相关API的调用方式直接复用即可。

1.  按需包含头文件，通过ASCENDC_CPU_DEBUG宏区分CPU和NPU侧需要包含的头文件。

    ```cpp
    #include "data_utils.h"
    #ifndef ASCENDC_CPU_DEBUG
    #include "acl/acl.h"
    #else
    #include "tikicpulib.h"
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z); // 核函数声明。
    #endif
    ```

2.  CPU侧运行验证。完成算子核函数CPU侧运行验证的步骤如下：

    **图1**  CPU侧运行验证步骤<a name="fig13576112114442"></a>  
    ![](../../../../figures/CPU侧运行验证步骤.png "CPU侧运行验证步骤")

    ```cpp
    int32_t main(int32_t argc, char* argv[])
    {
        uint32_t numBlocks = 8;
        size_t inputByteSize = 8 * 2048 * sizeof(uint16_t);
        size_t outputByteSize = 8 * 2048 * sizeof(uint16_t);
    
       // 使用GmAlloc分配共享内存，并进行数据初始化。
        uint8_t* x = (uint8_t*)AscendC::GmAlloc(inputByteSize);
        uint8_t* y = (uint8_t*)AscendC::GmAlloc(inputByteSize);
        uint8_t* z = (uint8_t*)AscendC::GmAlloc(outputByteSize);
    
        ReadFile("./input/input_x.bin", inputByteSize, x, inputByteSize);
        ReadFile("./input/input_y.bin", inputByteSize, y, inputByteSize);
        // 矢量算子需要设置内核模式为AIV模式。
        AscendC::SetKernelMode(KernelMode::AIV_MODE);
        // 调用ICPU_RUN_KF调测宏，完成核函数CPU侧的调用。
        ICPU_RUN_KF(add_custom, numBlocks, x, y, z);
        // 输出数据写出。
        WriteFile("./output/output_z.bin", z, outputByteSize);
        // 调用GmFree释放申请的资源。
        AscendC::GmFree((void *)x);
        AscendC::GmFree((void *)y);
        AscendC::GmFree((void *)z);
        return 0;
    }
    ```
