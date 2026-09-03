# AddrReg<a name="ZH-CN_TOPIC_0000002081579973"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/AddrReg_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件引用路径为 "basic_api/reg_compute/kernel_reg_compute_addrreg_intf.h"。

AddrReg是用于Reg矢量计算接口管理地址偏移量的专用寄存器。在循环外定义后，在循环内调用模板函数`CreateAddrReg<T>`接口赋值并绑定每层循环轴索引（index）与步长（stride），能够在多维循环中逐层累加地址偏移。CreateAddrReg接口支持绑定1-4层for循环，返回的地址偏移由index*stride的决定，在循环中，index每次递增1，AddrReg的偏移量就自动增加对应的stride，具体的偏移量满足如下伪代码所示：

```cpp
// 1层循环: 
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0);  // Offset = index0 * stride0 
} 
    
// 2层循环 
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1);  // Offset = index0 * stride0 + index1 * stride1 
    } 
} 
 
// 3层循环： 
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        for (uint16_t index2 = 0; index2 < loopNum2; ++index2){ 
            aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1, index2, stride2);  // Offset = index0 * stride0 + index1 * stride1 + index2 * stride2 
        } 
    } 
} 
 
// 4层循环： 
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        for (uint16_t index2 = 0; index2 < loopNum2; ++index2){ 
            for (uint16_t index3 = 0; index3 < loopNum3; ++index3){ 
                aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1, index2, stride2, index3, stride3);  // Offset = index0 * stride0 + index1 * stride1 + index2 * stride2 + index3 * stride3 
            } 
        } 
    } 
}
```

AddrReg应该通过CreateAddrReg API初始化，然后在循环中使用AddrReg存储地址偏移量。AddrReg在每层循环中根据所设置的stride进行自增。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
// offset = index0 * stride0 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0) 
  
// offset = index0 * stride0 + index1 * stride1 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1) 
  
// offset = index0 * stride0 + index1 * stride1 + index2 * stride2 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2)
 
// offset = index0 * stride0 + index1 * stride1 + index2 * stride2 + index3 * stride3 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2, uint16_t index3, uint32_t stride3)
```

## 参数说明<a name="section132601254123919"></a>

| 参数 | 含义 |
| :-- | :------------ | 
| T | 模板参数，支持的数据类型为b8、b16、b32、b64。 |
| index0 | 计算偏移量时作为最外层循环轴。 |
| index1 | 计算偏移量时作为第二层循环轴。 |
| index2 | 计算偏移量时作为第三层循环轴。 |
| index3 | 计算偏移量时作为第四层循环轴。 |
| stride0 | 每次循环，最外层循环轴对应的地址偏移量，单位为element。 |
| stride1 | 每次循环，第二层循环轴对应的地址偏移量，单位为element。 |
| stride2 | 每次循环，第三层循环轴对应的地址偏移量，单位为element。 |
| stride3 | 每次循环，第四层循环轴对应的地址偏移量，单位为element。 |

## 约束说明<a name="section11585101304320"></a>

- AddrReg 为地址偏移量寄存器，仅支持部分搬运指令使用，请根据Reg数据搬运接口函数原型选择，除了通过AddrReg设置地址偏移，支持用户自行累加地址或通过PostUpdate模式进行地址自增，完整特性见连续对齐搬入关键特性。通过AddrReg地址偏移进行搬运时，需要满足对应搬运指令的地址对齐约束。
- AddrReg 数量上限为8。
- 由于硬件循环(HardwareLoop)限制，AddrReg最多支持4层循环轴。

## 调用示例<a name="section633mcpsimp"></a>

```cpp
__simd_vf__ inline void CreateAddrRegVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::AddrReg aReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
        AscendC::Reg::LoadAlign(mask, srcAddr, aReg);
        AscendC::Reg::StoreAlign(dstAddr, mask, aReg);
    }
}
```
