# SK\_BIND<a name="ZH-CN_TOPIC_0000002565043461"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas 训练系列产品：不支持
<!-- end id9 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明<a name="section259105813316"></a>

本接口为算子SuperKernel场景提供绑定原核函数（Kernel）和SK子函数的能力。核函数（Kernel）直调算子的完整适配方法请参见[核函数（Kernel）直调算子额外适配说明](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md)。

## 函数原型<a name="section2067518173415"></a>

```cpp
#define SK_BIND(GF, cap, SK0,...)
```

## 参数说明<a name="section158061867342"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| GF | 输入 | 核函数（Kernel）签名。 |
| cap | 输入 | `uint64_t`类型，用于标记当前算子的SuperKernel特性。SuperKernel框架根据这些信息调整相应的同步和融合策略。各bit位可按位或组合：<br>&bull; 1：early start wait flag，表示算子内使用了[WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md)接口。<br>&bull; 2：early start set flag，表示算子内使用了[SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md)接口。<br>&bull; 4：disable_dcci，表示当前算子需要SuperKernel框架关闭调用前后的DCCI ALL指令。详细说明请参见[算子适配说明](../../../guide/programming_guide/advanced_programming/super_kernel/operator_adaptation.md)。<br>&bull; 8：disable_batchmode_check，表示跳过当前算子的`__schedmode__`检查，使该算子可继续参与SuperKernel融合。<br>例如，同时配置early start wait flag和early start set flag时，cap取值为3。 <br>&bull; 16：blockdim_scale_up, 表示算子已自行支持核数向上扩展。融合后，SuperKernel会按自身的核数启动该算子；算子需要自行保证在实际启动核数大于其原始核数时仍能正确执行。|
| SK0 | 输入 | SK子函数签名。 |
| ... | 输入 | SK1~SK3。可选，可提供额外的SK子函数签名。SuperKernel可以为不同核指定不同的SK子函数缓解同地址访问开销。|

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section43265506459"></a>

-   一个核函数（Kernel）最多绑定四个SK子函数。
-   核函数（Kernel）直调算子目前仅支持在npugraph_ex后端进入SuperKernel，不支持GE图模式。

## 需要包含的头文件<a name="section10354115115916"></a>

使用该接口需要包含"kernel\_operator.h"头文件。

```cpp
#include "kernel_operator.h"
```

## 调用示例<a name="section990974612242"></a>

以下示例展示SK_BIND接口的基本用法，SK子函数签名、参数结构体定义等完整规则请参见[核函数（Kernel）直调算子额外适配说明](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md)。

```cpp
#include "kernel_operator.h"

// 原普通kernel保留（用于非SuperKernel场景）
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength) 
{ 
    KernelAdd op; 
    op.Init(x, y, z, totalLength); 
    op.Process(); 
} 

// 规则3：定义参数结构体（根据原global函数的实际参数定义）
struct AddCustomArgs {
   GM_ADDR x;                      // 对应add_custom的第一个参数 
   GM_ADDR y;                      // 对应add_custom的第二个参数 
   GM_ADDR z;                      // 对应add_custom的第三个参数 
   uint32_t totalLength;           // 对应add_custom的第四个参数
};  
// 定义一个带模板参数的SK子函数
// 模板参数仅用于实例化出不同的符号，不影响函数逻辑
template<uint32_t splitNum>
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs/* 可选添加sysArgs参数 */)
{
    // 从结构体中获取参数
    GM_ADDR x = args->x;
    GM_ADDR y = args->y; 
    GM_ADDR z = args->z; 
    uint32_t totalLength = args->totalLength; 
    // 规则5：逻辑与global函数一致
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}
// 规则6：使用SK_BIND绑定
// 通过指定模板参数实例化出4个不同的符号
// cap取值4表示配置disable_dcci
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```
