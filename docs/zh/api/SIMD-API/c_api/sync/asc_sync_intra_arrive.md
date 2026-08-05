# asc_sync_intra_arrive

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

在分离模式下，AIC（Cube Core）与AIV（Vector Core）按group划分，一个group由1个block（主核）和N个subblock（从核）组成，比例1:N。block内同步指同一block内block（AIC）与单个subblock（AIV）之间的同步。

block内同步指令，与[asc_sync_intra_wait](asc_sync_intra_wait.md)配合使用，是block内不同Core的流水线同步指令，用于向每个Core的同步寄存器发送同步信号。每个sync_id对应Vector Core与Cube Core中的一个初始值为0的计数器。asc_sync_intra_arrive指令后其他Core对应的计数器增加1；执行asc_sync_intra_wait时如果对应的计数器数值为0则阻塞不执行；如果对应的计数器大于0，则计数器减一，同时后续指令开始执行。

block内同步场景可细分为两类：

- AIC执行asc_sync_intra_arrive，对应的AIV执行asc_sync_intra_wait。当AIC执行完asc_sync_intra_arrive时，AIV对应sync_id的计数器增加1。若AIV该sync_id的计数器为非0，则解除AIV的阻塞，asc_sync_intra_wait后的指令继续执行，该sync_id的计数器减1。总计AIC 1次调用asc_sync_intra_arrive，单个AIV完成1次asc_sync_intra_wait才算配对使用。

- 1个AIV执行asc_sync_intra_arrive，对应的AIC执行asc_sync_intra_wait。当AIV执行完asc_sync_intra_arrive时，AIC对应sync_id的计数器增加1。若AIC该sync_id的计数器为非0，则解除AIC的阻塞，asc_sync_intra_wait后的指令继续执行，该sync_id的计数器减1。总计单个AIV 1次asc_sync_intra_arrive，对应AIC 1次调用asc_sync_intra_wait才算配对使用。

以图1为例，演示1个AI Core中AIV1与AIC进行同步（AIV1-2发起asc_sync_intra_arrive）。

AIC 1中在执行asc_sync_intra_wait后，此时AIC 1 sync_id=0的计数器为0，后续所有指令全部被阻塞，需要等到1个AIV执行完asc_sync_intra_arrive。

- AIV 1-1不需要执行asc_sync_intra_arrive。
- AIV 1-2的PIPE_MTE3指令全部执行完毕后，asc_sync_intra_arrive生效。此时调度模块感知1个AIV已执行完asc_sync_intra_arrive，因此将AIC 1 sync_id=0的计数器值增加为1。AIC 1检测到对应的sync_id=0的计数器变为1，则AIC 1核解除阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图1**  block内同步时序图（AIV进行asc_sync_intra_arrive）
![](../../../figures/single_ai_core_aic_single_aiv_sync.png "block内同步时序图（AIV进行asc_sync_intra_arrive）")

## 函数原型

```cpp
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint8_t sync_id)
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint64_t sync_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 该指令的执行流水线。 |
| sync_id | 输入 | 事件标号。AIC的取值范围为[0, 31]，AIV的取值范围为[0, 15]。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 需要保证每一个ID的计数器不会溢出，否则会出现异常。
- 需要保证相同的ID只会被一个流水线控制。
- 每个ID对应一个计数器，特定CORE的同步ID计数器可通过其他CORE发起的asc_sync_intra_arrive递增；也可通过本身发起的asc_sync_intra_wait递减。 
- 三个CORE的ID映射规则如下：
    - ID为0~15的Vector Core 0的asc_sync_intra_arrive指令会被映射到Cube Core的0~15的ID。 
    - ID为0~15的Vector Core 1的asc_sync_intra_arrive指令会被映射到Cube Core的16~31的ID。 
    - ID为0~15的Cube Core的asc_sync_intra_arrive指令会被映射到Vector Core 0的0~15的ID。 
    - ID为16~31的Cube Core的asc_sync_intra_arrive指令会被映射到Vector Core 1的0~15的ID。 

- 必须保证配套使用asc_sync_intra_arrive和[asc_sync_intra_wait](asc_sync_intra_wait.md)，否则会出现未定义行为。
- pipe支持的流水类型为PIPE_V、PIPE_M、PIPE_MTE1、PIPE_MTE2、PIPE_MTE3、PIPE_FIX、PIPE_S，不支持PIPE_ALL。
- 一个核连续发出的asc_sync_intra_arrive，硬件不保证执行顺序。

## 调用示例

```cpp
// 场景：1个AIV执行asc_sync_intra_arrive通知AIC，对应AIC执行asc_sync_intra_wait等待。
// 注意：asc_sync_intra_arrive与asc_sync_intra_wait必须分别在不同Core上调用，同一Core连续调用无法实现跨核同步。

// ---- AIV核代码 ----
uint64_t sync_id = 1;
asc_sync_intra_arrive(PIPE_V, sync_id);   // AIV执行arrive，AIC对应sync_id计数器+1

// ---- AIC核代码 ----
asc_sync_intra_wait(PIPE_FIX, sync_id);   // AIC执行wait，计数器>0则解除阻塞并-1
```
