# SetPadValue\(ISASI\)<a name="ZH-CN_TOPIC_0000001786582454"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

[DataCopyPad（GMToUB非对齐数据搬运）](../数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)从Global Memory将数据非对齐搬运至Unified Buffer时，可根据开发者的需要自行填充数据。SetPadValue用于设置DataCopyPad需要填充的数值。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, TPosition pos = TPosition::MAX>
__aicore__ inline void SetPadValue(T paddingValue)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| T | 输入 | 填充值的数据类型，与DataCopyPad接口搬运的数据类型一致。 |
| pos | 输入 | 用于指定DataCopyPad接口搬运过程中从Global Memory搬运数据到哪一个目的地址，目的地址通过逻辑位置来表达。默认值为TPosition::MAX，等效于TPosition::VECIN或TPosition::VECOUT。<br>支持的取值为：<br>&bull; TPosition::VECIN、TPosition::VECOUT、TPosition::MAX |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| paddingValue | 输入 | DataCopyPad接口填充的数值，数据类型与DataCopyPad接口搬运的数据类型一致。 |

## 数据类型

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id3 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section1227835243314"></a>

```cpp
// 场景1：使用SetPadValue进行自定义填充。
// DataCopyExtParams参数：blockCount=1, blockLen=20*sizeof(half), srcStride=0, dstStride=0, rsv=0。
// DataCopyPadExtParams参数：isPad=false, leftPadding=0, rightPadding=12。
// SetPadValue设置填充值为1，配合isPad=false使用。
AscendC::DataCopyExtParams copyParams{1, srcCols * sizeof(T), 0, 0, 0};
AscendC::DataCopyPadExtParams<T> padParams;
padParams.isPad = false;
padParams.leftPadding = 0;
padParams.rightPadding = dstCols - srcCols;

AscendC::SetPadValue((T)1);
AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
```

完整样例请参考[DataCopyPad样例场景1](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm)。
