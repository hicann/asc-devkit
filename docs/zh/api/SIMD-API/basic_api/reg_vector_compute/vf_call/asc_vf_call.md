# asc\_vf\_call<a name="ZH-CN_TOPIC_0000002563057755"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

在SIMD编程场景下使用。用于启动SIMD VF（Vector Function）子任务。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(Args &&...args)
```

## 参数说明<a name="section0866173114710"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :-- | :------------ | 
| funcPtr | 用于指定SIMD入口核函数（Kernel）。 |
| Args | 定义可变参数，用于传递实参到SIMD入口核函数（Kernel）。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :-- | :------------ | :-- |
| args | 输入 | 可变参数，用于传递实参到SIMD入口核函数（Kernel）。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   asc\_vf\_call启动SIMD VF子任务时，子任务函数不能是类的成员函数，推荐使用普通函数或类静态函数，且入口函数必须使用\_\_simd\_vf\_\_修饰宏。
-   asc\_vf\_call启动SIMD VF子任务时，入参仅支持Ascend C的内置数据类型（half、int32_t、uint32_t、float等）及其组成的指针、数组、结构体类型，且指针类型必须指向Unified Buffer（UB）内存。

## 调用示例<a name="section1316724610428"></a>

使用SIMD VF函数对UB数据做加法计算。

```cpp
// SIMD函数
template <typename T>
__simd_vf__ inline void AddVF(
    __ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(src0Reg, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(src1Reg, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}

template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = outQueueZ.AllocTensor<T>();
    AscendC::LocalTensor<T> src0 = inQueueX.DeQue<T>();
    AscendC::LocalTensor<T> src1 = inQueueY.DeQue<T>();
    constexpr uint16_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint32_t count = 512;
    // 向上取整，计算循环次数
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* src0Addr = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* src1Addr = (__ubuf__ T*)src1.GetPhyAddr();
    asc_vf_call<AddVF<T>>(dstAddr, src0Addr, src1Addr, count, oneRepeatSize, repeatTimes);
    outQueueZ.EnQue(dst);
    inQueueX.FreeTensor(src0);
    inQueueY.FreeTensor(src1);
}
```
