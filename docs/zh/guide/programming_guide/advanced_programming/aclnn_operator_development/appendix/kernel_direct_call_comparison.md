# aclnn工程化算子开发与核函数（Kernel）直调对比

本文介绍aclnn工程化算子开发与核函数（Kernel）直调两种开发方式的差异，供开发者选择开发方式及迁移算子时参考。

| 特性 | 核函数（Kernel）直调 | aclnn工程化算子开发 |
|---|---|---|
| 调用入口 | 使用`<<<>>>`作为算子的调用入口，自行指定核函数及执行配置。 | 通过生成的两段式aclnn接口调用算子：第一段获取workspace大小和executor，第二段传入workspace、executor和stream执行。详见[单算子API调用](../invocation/single_operator_api_call.md)。 |
| 入参结构规范 | 核函数入口支持传入结构体参数，结构体需为POD类型。例如：<br>`struct AddParams;`<br>`__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z, AddParams params);` | 核函数入口不支持直接传入结构体对象（按值传参）。核函数入口按照算子原型中的输入、输出顺序传入地址，随后追加workspace和tiling地址。Tiling参数通过TilingData结构体组织，并通过tiling地址传递和解析。例如，AddCustom有两个输入x、y和一个输出z，其入口声明为：<br>`extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling);`<br>其中x、y为输入地址，z为输出地址，workspace为工作空间地址，tiling为Tiling数据地址。详见[核函数（Kernel）侧算子实现](../design_and_implementation/kernel_operator_implementation.md)。 |
| Kernel Type | 通过`__vector__`、`__cube__`或`__mix__`函数执行空间限定符设置核函数类型。 | 核函数入口使用`__global__ __aicore__`修饰，通过`KERNEL_TASK_TYPE_DEFAULT`设置默认类型，或通过`KERNEL_TASK_TYPE`按TilingKey设置类型。Tiling模板编程场景通过`ASCENDC_TPL_KERNEL_TYPE_SEL`设置模板组合的类型。详见[核函数（Kernel）侧算子实现](../design_and_implementation/kernel_operator_implementation.md)。 |
| 调度模式 | 通过`__schedmode__(mode)`限定符设置核函数调度模式。 | 在Host侧Tiling中调用`TilingContext::SetScheduleMode`设置调度模式。详见[SyncAll的调度模式约束](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll.md)。 |
| KFC workspace地址设置 | 使用KFC的场景需要预留系统workspace，并通过`__kfc_workspace__`标记核函数入口的workspace参数，供编译器初始化KFC workspace地址。 | 使用KFC的场景需要在Host侧Tiling中预留所需的系统workspace。Kernel侧的KFC workspace地址初始化由框架自动处理，用户按工程规范保留workspace入口参数即可，无需添加`__kfc_workspace__`标记。详见[如何使用workspace](../../../appendix/common_operations/how_to_use_workspace.md)。 |
| SuperKernel适配 | 需要用户提供使用`__sk__`修饰的SuperKernel入口函数，并通过`SK_BIND`宏与普通核函数入口绑定，完成SuperKernel适配。 | SuperKernel入口适配由框架内部自动完成，用户无需配置`__sk__`和`SK_BIND`。算子仍需满足[算子适配说明](../../super_kernel/operator_adaptation.md)中的通用约束。 |
