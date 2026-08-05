# SetSubBlockIdx

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明

设置当前AIV核的ID。分离架构下，一个AI Core由Cube Core（AIC）和Vector Core（AIV）按照一定比例1：N进行组合，其中N个AIV核的ID分别为0, 1, ..., N-1。

## 函数原型

```
__aicore__ inline void SetSubBlockIdx(uint8_t subBlockIdx)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| subBlockIdx | 输入 | 当前AIV核的ID。 |

## 返回值说明

无

## 约束说明

-   该接口仅支持在分离架构下使用。
-   在分离架构中，AIV核的ID会在[REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md)接口内部自动初始化和赋值。如果在算子程序中使用了REGIST\_MATMUL\_OBJ\(\)接口，则不建议调用此接口；若未使用REGIST\_MATMUL\_OBJ\(\)接口，则请调用此接口并将子核ID设置为0。

## 调用示例

```
#define ASCENDC_CUBE_ONLY
#include "lib/matmul_intf.h"

typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
mm.SetSubBlockIdx(0); // 子核ID设置为0
```
