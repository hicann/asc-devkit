# DumpAccChkPoint<a name="ZH-CN_TOPIC_0000001877958569"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section259105813316"></a>

头文件路径为：`"basic_api/kernel_operator_dump_tensor_intf.h"`。

该接口Dump指定Tensor的内容。同时支持打印自定义的标签（仅支持uint32_t数据类型的信息），比如打印当前行号等。区别于[DumpTensor](DumpTensor.md)，使用该接口可以支持指定偏移位置的Tensor打印。

在算子kernel侧实现代码中需要打印偏移后Tensor数据的地方，调用DumpAccChkPoint接口打印相关内容。样例如下：

```cpp
AscendC::DumpAccChkPoint(srcLocal, 5, 32, dataLen);
```
> [!CAUTION]注意
> 该接口主要用于调试分析，开启后会对算子性能产生一定影响，通常在调试阶段使用，生产环境建议关闭。<br>
> 默认情况下，调用该接口就会打印相关内容，开发者可以参考[关闭ASCENDC_DUMP说明](../关闭ASCENDC_DUMP说明.md)，按需关闭该接口功能。

## 函数原型<a name="section2067518173415"></a>

```cpp
template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t dumpSize)
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t dumpSize)
```

## 参数说明<a name="section158061867342"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 需要dump的Tensor的数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| tensor | 输入 | 需要dump的Tensor。<br>•待dump的tensor位于Unified Buffer/L1 Buffer/L0C Buffer时使用LocalTensor类型的tensor参数输入。<br>•待dump的tensor位于Global Memory时使用GlobalTensor类型的tensor参数输入。|
| index | 输入 | 用户自定义附加信息（行号或其他自定义数字）。|
| countOff | 输入 | 偏移元素个数。偏移后的Tensor地址需要满足所在物理位置的对齐约束。具体参考[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。|
| dumpSize | 输入 | 需要dump的元素个数。|

## 数据类型

<!-- npu="950" id101 -->
- Ascend 950PR/Ascend 950DT，T支持的数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id101 -->
<!-- npu="A3" id102 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id102 -->
<!-- npu="910b" id103 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id103 -->
<!-- npu="310p" id105 -->
- Atlas 推理系列产品AI Core，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id105 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section794123819592"></a>

- 当前仅支持打印存储位置为Unified Buffer/L1 Buffer/L0C Buffer/Global Memory的Tensor信息。
<!-- npu="950" id100 -->
- 针对Ascend 950PR/Ascend 950DT，使用该接口打印L1 Tensor数据时，HDK版本需要至少升级到25.7.0以上。
<!-- end id100 -->
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 单次调用DumpAccChkPoint打印的数据总量不可超过30KB（还包括少量框架需要的头尾信息，通常可忽略）。使用时应注意，如果超出这个限制，则数据不会被打印。

## 调用示例<a name="section82241477610"></a>

```cpp
constexpr uint32_t totalLength = 256;    // 参与搬运的元素个数
AscendC::LocalTensor<half> srcLocal;
AscendC::GlobalTensor<half> srcGlobal;
AscendC::DataCopy(srcLocal, srcGlobal, totalLength * sizeof(half));
uint32_t index = 56;    // 用户自定义附加信息，此处传入DumpAccChkPoint指令的行号
uint32_t countOff = 32;    // 偏移元素个数，从srcLocal[32]开始打印
uint32_t dumpSize = 128;    // dump的元素个数，从srcLocal[32]开始打印128个元素个数
AscendC::DumpAccChkPoint(srcLocal, index, countOff, dumpSize);
```

打印结果如下：
```plain
DumpTensor: desc=56, addr=0x40, data_type=float16, position=UB, dump_size=15
[4.710938, 4.707031, 4.773438, 2.271484, 4.347656, 2.359375, 1.284180, 1.073242, 1.242188, 2.298828, 0.521973, 1.099609, 1.880859, 1.226562, 3.916016]
```
