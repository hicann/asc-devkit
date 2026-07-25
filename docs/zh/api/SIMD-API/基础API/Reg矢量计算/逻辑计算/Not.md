# Not<a name="ZH-CN_TOPIC_0000001929668260"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`（对`RegTensor`操作）、`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`（对`MaskReg`操作）。

本节介绍两种接口，分别用于对[RegTensor](../寄存器数据类型/RegTensor.md)和[MaskReg](../寄存器数据类型/MaskReg.md)进行操作：

- 对`RegTensor`操作：

  根据mask对输入数据srcReg执行按位取反操作，将结果写入dstReg。

- 对`MaskReg`操作：

    根据mask对输入数据src执行按位取反操作，将结果写入dst。

## 函数原型<a name="section620mcpsimp"></a>

- 对`RegTensor`进行操作

  ```cpp
  template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
  __simd_callee__ inline void Not(U& dstReg, U& srcReg, MaskReg& mask)
  ```

- 对`MaskReg`进行操作

  ```cpp
  __simd_callee__ inline void Not(MaskReg& dst, MaskReg& src, MaskReg& mask)
  ```

## 参数说明<a name="section622mcpsimp"></a>

- 对`RegTensor`进行操作

  **表1**  模板参数说明

  | 参数名 | 描述 |
  | :----- | :--- |
  | T | 操作数数据类型。 |
  | mode | [MaskMergeMode](../辅助数据类型/MaskMergeMode.md)枚举类型。选择MERGING模式或ZEROING模式。<br>• ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>• MERGING模式当前不支持。 |
  | U | 目的操作数和源操作数的`RegTensor`类型，例如`RegTensor<half>`，由编译器自动推导，用户不需要填写。 |

  **表2**  参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :-------- | :--- |
  | dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
  | srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>源操作数的数据类型需要与目的操作数保持一致。 |
  | mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

- 对`MaskReg`进行操作

  **表3**  参数说明

  | 参数名 | 描述 |
  | :----- | :--- |
  | dst | 目的操作数。 |
  | src | 源操作数。 |
  | mask | 指示在计算过程中哪些bit有效。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

- 对`RegTensor`进行操作

  ```cpp
  template <typename T>
  __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
   uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::RegTensor<T> dstReg;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; i++) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Not(dstReg, srcReg, mask);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- 对`MaskReg`进行操作

  ```cpp
  template <typename T>
  __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
      AscendC::Reg::MaskReg dst;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; ++i) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Not(dst, src, mask);
          AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
      }
  }
  ```
