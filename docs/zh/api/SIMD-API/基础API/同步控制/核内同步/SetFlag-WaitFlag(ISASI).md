# SetFlag/WaitFlag\(ISASI\)<a name="ZH-CN_TOPIC_0000001837467889"></a>

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
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

如图1所示，SetFlag/WaitFlag接口用于核内多流水间的同步：

> [!NOTE]说明
>该接口标注为ISASI类别，不能保证跨硬件版本兼容。如需保证跨硬件版本兼容的同步控制接口，请参见[TQueSync SetFlag/WaitFlag](SetFlag-WaitFlag.md)。二者的区别在于，TQueSync类接口可以保证跨硬件版本兼容。

- SetFlag：当源流水的前序指令的所有读写操作都完成之后，当前指令开始执行，并将硬件中的对应标志位设置为1。SetFlag只是设置硬件中的对应标志位，并不会阻塞源流水中的下一个指令。
- WaitFlag：当目的流水执行到该指令时，如果发现硬件中对应标志位为0，目的流水的后续指令将一直被阻塞；如果发现硬件中对应标志位为1，则将硬件中对应标志位设置为0，同时目的流水的后续指令开始执行。

**图1**  SetFlag/WaitFlag接口功能示意图<a name="zh-cn_topic_0000002511125384_fig58242463299"></a>  
![](../../../../figures/setflag_waitflag_multi_pipeline_sync.png "SetFlag_WaitFlag_多流水同步示意图")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <HardEvent event>
__aicore__ inline void SetFlag(int32_t eventID)
template <HardEvent event>
__aicore__ inline void WaitFlag(int32_t eventID)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| event | 输入 | 模板参数。<br>同步事件，数据类型为HardEvent。同一核内的不同流水之间，在存在数据访问依赖时，需要根据数据访问的先后顺序，插入对应的同步事件。HardEvent用来表示对应的同步事件。HardEvent命名规则为<源流水\_目标流水\>，其中源流水的指令先执行、目标流水中的指令后执行。例如MTE2\_V，代表PIPE\_MTE2为源流水，PIPE\_V为目标流水，标识从PIPE\_MTE2到PIPE\_V的同步，PIPE\_V等待PIPE\_MTE2。由于硬件架构版本代际间的差异，不同硬件架构上的事件存在差异。 |
| eventID | 输入 | 事件ID。数据类型为int32_t类型。eventID的取值范围与产品型号有关，具体请参考[约束说明](#section633mcpsimp)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- SetFlag只是设置硬件标志位，不会阻塞源流水中的下一个指令。

- SetFlag和WaitFlag必须成对使用，且SetFlag和WaitFlag的参数必须完全一致（包括模板参数event和输入参数eventID）。如果不匹配，会引发timeout问题。例如，`SetFlag<HardEvent::S_MTE3>(1)`和`WaitFlag<HardEvent::MTE3_MTE1>(1)`并不匹配，因为其模板参数event不同。

- 在采用[TPipe-TQue框架编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程范式.md)时，eventID需要通过[AllocEventID](../../资源管理/TPipe/AllocEventID.md)或者[FetchEventID](../../资源管理/TPipe/FetchEventID.md)来获取。

- 在采用[静态Tensor编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)时，事件的类型和事件ID由开发者自行管理，建议使用事件ID0-5，事件ID6用于系统内部规划（当前未使用），事件ID7用于TPipe编程中的**自动同步**功能，目前暂不建议直接使用事件ID6-7。

- eventID的取值范围如下：
<!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT，数据范围为：0-7。
<!-- end id10 -->
<!-- npu="A3" id11 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，数据范围为：0-7。
<!-- end id11 -->
<!-- npu="910b" id12 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，数据范围为：0-7。
<!-- end id12 -->
<!-- npu="310p" id13 -->
    - Atlas 推理系列产品AI Core，数据范围为：0-7。
<!-- end id13 -->
<!-- npu="910" id14 -->
    - Atlas 训练系列产品，数据范围为：0-3。
<!-- end id14 -->

- 相同流水、相同eventID下，连续使用SetFlag会引发未定义行为，此时再执行PipeBarrier<PIPE\_ALL\>会出现卡死现象：

    ```cpp
    SetFlag<M_MTE1>(0);  // 第一次SetFlag
    SetFlag<M_MTE1>(0);  // 第二次SetFlag（相同流水、相同eventID连续使用，引发未定义行为）
    ...
    PipeBarrier<PIPE_ALL>();  // 触发卡死
    ...
    WaitFlag<M_MTE1>(0);
    WaitFlag<M_MTE1>(0);
    ```

## 调用示例<a name="section837496171220"></a>

```cpp
    AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
    AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);
    
    // 循环内依赖：先“DataCopy(PIPE_MTE2)写src0Local”，后“Maxs和Mins（PIPE_V）读src0Local”。
    // 由于PIPE_V需要等待PIPE_MTE2，所以需要插入以下同步。
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
    AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```
