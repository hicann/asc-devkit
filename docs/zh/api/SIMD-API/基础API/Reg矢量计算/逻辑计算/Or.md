# Or<a name="ZH-CN_TOPIC_0000001929668268"></a>

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`（对`RegTensor`操作）、`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`（对`MaskReg`操作）。

本节介绍两种接口，分别用于对[RegTensor](../寄存器数据类型/RegTensor.md)和[MaskReg](../寄存器数据类型/MaskReg.md)进行操作：

- 对`RegTensor`操作：

  根据mask对输入数据srcReg0、srcReg1按位求或（|）操作，将结果写入dstReg。

- 对`MaskReg`操作：

  根据mask对两个输入`MaskReg`数据src0、src1按位求或（|）操作，将结果写入dst。

## 函数原型<a name="section620mcpsimp"></a>

- 对`RegTensor`操作

  ```cpp
  template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
  __simd_callee__ inline void Or(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
  ```

- 对`MaskReg`操作

  ```cpp
  __simd_callee__ inline void Or(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
  ```

## 参数说明<a name="section622mcpsimp"></a>

- 对`RegTensor`操作

  **表1**  模板参数说明

  | 参数名 | 描述 |
  | :----- | :--- |
  | T | 操作数数据类型。 |
  | mode | [MaskMergeMode](../辅助数据类型/MaskMergeMode.md)枚举类型。选择MERGING模式或ZEROING模式。<br>• ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>• MERGING模式当前不支持。 |
  | U | 目的操作数和源操作数的`RegTensor`类型，例如`RegTensor<uint32_t>`，由编译器自动推导，用户不需要填写。 |

  **表2**  参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :-------- | :--- |
  | dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
  | srcReg0 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>数据类型需要与目的操作数保持一致。 |
  | srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>数据类型需要与目的操作数保持一致。 |
  | mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

- 对`MaskReg`操作

  **表3**  参数说明

  | 参数名 | 描述 |
  | :----- | :--- |
  | dst    | 目的操作数。 |
  | src0   | 源操作数。 |
  | src1   | 源操作数。 |
  | mask   | 指示在计算过程中哪些bits有效。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

- 对`RegTensor`操作

  ```cpp
  template <typename T>
  __simd_vf__ inline void OrVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg0;
      AscendC::Reg::RegTensor<T> srcReg1;
      AscendC::Reg::RegTensor<T> dstReg;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; i++) {
          AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
          AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::Or(dstReg, srcReg0, srcReg1, mask);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- 对`MaskReg`操作

  ```cpp
  template <typename T>
  __simd_vf__ inline void OrVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
      AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
      AscendC::Reg::MaskReg dst;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; ++i) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Or(dst, src0, src1, mask);
          AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
      }
  }
  ```
