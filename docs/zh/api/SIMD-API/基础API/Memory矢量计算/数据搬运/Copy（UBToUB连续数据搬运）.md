# Copy（UBToUB连续数据搬运）<a name="ZH-CN_TOPIC_0000002575088175"></a>

## 产品支持情况<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas 训练系列产品：不支持
<!-- end id17 -->
<!-- npu="x90" id18 -->
- Kirin X90：不支持
<!-- end id18 -->
<!-- npu="9030" id19 -->
- Kirin 9030：不支持
<!-- end id19 -->

## 功能说明<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

支持Unified Buffer和Unified Buffer之间的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

## 函数原型<a name="zh-cn_topic_0000002567699435_section82039854412"></a>

```cpp
template <typename T, bool isSetMask = true>
__aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## 参数说明<a name="zh-cn_topic_0000002567699435_section16128134420472"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002567699435_section42191353048181)。 |
| isSetMask | 参数不生效，保持默认值true即可。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)，支持的TPosition为VECIN/VECCALC/VECOUT。起始地址需要保证32字节对齐。 |
| src | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。起始地址需要保证32字节对齐。<br>源操作数的数据类型需要与目的操作数保持一致。 |
| count | 输入 | 参与搬运的元素个数。 |

## 数据类型<a name="zh-cn_topic_0000002567699435_section42191353048181"></a>

源操作数和目的操作数支持的数据类型保持一致。
<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id1 -->

## 返回值说明

无

## 约束说明<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- 位于Unified Buffer的地址必须32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- isSetMask参数不生效，保持默认值true即可。

## 调用示例<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
AscendC::Copy(dstLocal, srcLocal, 512);
```
