# Store

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

Reg矢量计算数据搬运接口，支持从RegTensor搬出至非32字节对齐的UB地址dstAddr。连续搬运时，用户需手动更新dstAddr地址。

该接口封装了[StoreUnAlign和StoreUnAlignPost](StoreUnAlign_continuous.md)。

## 函数原型

-   搬运量为VL

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg)
    ```

-   搬运count个数据

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg, uint32_t count)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 源操作数的RegTensor类型，例如RegTensor\<half>，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstAddr | 输出 | 目的操作数，UB起始地址，不需要32字节对齐。 |
| srcReg | 输入 | 源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| count | 输入 | 搬运数据量。连续搬运时需手动更新地址：dstAddr = dstAddr + count。|

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。
- 当RegTensor模板参数regTrait为RegTraitNumOne时支持的数据类型为：b8、b16、b32、b64。
- 当RegTensor模板参数regTrait为RegTraitNumTwo时支持的数据类型为：complex32、b64。

## 返回值说明

无

## 约束说明

-   count不能大于一个RegTensor能存储的数据个数，即count <= 256B / sizeof(T)。
-   接口内部定义了一个[UnalignRegForStore](../寄存器数据类型/UnalignRegForLoad-UnalignRegForStore.md)，该寄存器数量上限为4。

## 调用示例

- 搬出一个VL数据量
    ```cpp
    template<typename T>
    __simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Load(srcReg, srcAddr + i * count);
            AscendC::Reg::Store(dstAddr + i * count, srcReg);
        }
    }
    ```

- 搬出count个数据
    ```cpp
    template<typename T>
    __simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Load(srcReg, srcAddr + i * count);
            AscendC::Reg::Store(dstAddr + i * count, srcReg, count);
        }
    }
    ```
