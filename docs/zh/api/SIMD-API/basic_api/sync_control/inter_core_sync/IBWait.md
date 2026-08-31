# IBWait<a name="ZH-CN_TOPIC_0000001487056954"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/IBWait_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

当不同核之间操作同一块全局内存且可能存在读后写、写后读以及写后写等数据依赖问题时，通过调用该函数来插入同步语句来避免上述数据依赖时可能出现的数据读写错误问题。

调用IBSet设置某一个核的标志位，与IBWait成对使用，表示核之间的同步等待指令：被等待核中调用IBSet，通过将值1写入全局内存中某块地址来通知等待核；而等待核中调用IBWait，持续读取全局内存中相应地址块的值，直到其值变为1，从而实现核间同步。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <bool isAIVOnly = true>
__aicore__ inline void IBWait(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx, int32_t eventID)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| isAIVOnly | 控制是否为AIVOnly模式，默认为true。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gmWorkspace | 输入 | 外部存储核状态的公共缓存，类型为GlobalTensor。GlobalTensor数据结构的定义请参考[GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md)。 |
| ubWorkspace | 输入 | 存储当前核状态的公共缓存。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| blockIdx | 输入 | 表示等待核的idx号，取值范围：[0,核数-1]。 |
| eventID | 输入 | 用来控制当前核的set、wait事件。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- gmWorkspace申请的空间最少要求为：核数*32Bytes*eventID_max+blockIdx_max*32Bytes+32Bytes（eventID_max和blockIdx_max分别指eventID、blockIdx的最大值）。需要注意的是，如果是AIVOnly模式，核数=GetBlockNum()；如果是MIX模式，核数=GetBlockNum()*2。
- ubWorkspace申请的空间最少要求为：32Bytes。
- 使用该接口进行多核控制时，算子调用时指定的逻辑AI Core核数numBlocks必须保证不大于实际运行该算子的AI处理器核数，否则框架进行多轮调度时会插入异常同步，导致核函数（Kernel）“卡死”现象。
- IBSet和IBWait配对使用时，除了ubWorkspace其余所有参数都必须相同，否则程序会在IBWait处卡死。
- 分离模式下，使用该接口进行两个核间的同步时，仅对AIV核生效。
- IBSet接口的gmWorkspace缓存的值需要初始化为0。

## 调用示例<a name="section177231425115410"></a>

完整样例请参考[调用示例](IBSet.md#section177231425115410)。
