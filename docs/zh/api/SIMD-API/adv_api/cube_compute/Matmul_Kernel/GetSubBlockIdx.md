# GetSubBlockIdx

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetSubBlockIdx_res.md#id1 -->

## 功能说明

获取当前AIV核的ID。分离架构下，一个AI Core由Cube Core（AIC）和Vector Core（AIV）按照一定比例1：N进行组合，其中N个AIV的子核ID分别为0, 1, ..., N-1。

Matmul::GetSubBlockIdx\(\)与基础API接口[AscendC::GetSubBlockIdx\(\)](../../../basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md)的区别在于，Matmul::GetSubBlockIdx\(\)用于获取当前AIV核在当前AI Core分组中的ID，而AscendC::GetSubBlockIdx\(\)获取的是AIV核在所有AI Core分组中的逻辑ID。例如，有10组AI Core，AIC与AIV的比例为1:2，共20个AIV核。调用Matmul::GetSubBlockIdx\(\)时，20个AIV的获取结果依次为0, 1, 0, 1, 0, 1, ..., 0, 1。调用AscendC::GetSubBlockIdx\(\)时，20个AIV的获取结果依次为0, 1, 2, 3, 4, 5, ..., 18, 19。

## 函数原型

```
__aicore__  inline uint8_t GetSubBlockIdx()
```

## 参数说明

无

## 返回值说明

当前AIV核的ID。

## 约束说明

-   该接口仅支持在分离架构下使用，否则返回随机值。
-   在分离架构中，AIV核的ID会在[REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md)接口内部自动初始化和赋值。因此，需要在调用REGIST\_MATMUL\_OBJ\(\)接口之后，再调用本接口，以获取正确的ID。
-   若在算子程序中调用[SetSubBlockIdx\(\)](SetSubBlockIdx.md)后， GetSubBlockIdx\(\)接口将返回由SetSubBlockIdx接口设置的ID值。

## 调用示例

```
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.GetSubBlockIdx(); // 获取子核ID
```
