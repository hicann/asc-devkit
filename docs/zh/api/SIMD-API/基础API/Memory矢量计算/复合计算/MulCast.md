# MulCast<a name="ZH-CN_TOPIC_0000001786581974"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_mulcast_intf.h"`。

按元素求积，并根据源操作数和目的操作数Tensor的数据类型进行精度转换。计算公式如下：

$$
dst_i = Cast(src0_i * src1_i)
$$

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint32_t count)
  ```

- tensor高维切分计算
  - mask逐bit模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams &repeatParams)
    ```

  - mask连续模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams &repeatParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 目的操作数数据类型。 |
| U | 源操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src0、src1 | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 参与计算的元素个数。<br>**注：参数取值范围和操作数的数据类型有关，数据类型不同，能够处理的元素个数最大值不同，最大处理的数据量不能超过UB大小限制。** |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。<br>设置详见[掩码操作](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。<br>矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[BinaryRepeatParams](../../辅助数据结构/BinaryRepeatParams.md)类型，包含操作数相邻迭代间相同datablock的地址步长，操作数同一迭代内不同datablock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

<!-- npu="950" id8 -->

**表**  Ascend 950PR/Ascend 950DT数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |
| int64_t            | float       |
| int64_t            | int32_t     |

针对Ascend 950PR/Ascend 950DT，src0、src1数据类型为int64_t的数据类型组合仅支持tensor前n个数据计算接口。

<!-- end id8 -->

<!-- npu="A3" id9 -->

**表**  Atlas A3 训练系列产品/Atlas A3 推理系列产品数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |

<!-- end id9 -->

<!-- npu="910b" id10 -->

**表**  Atlas A2 训练系列产品/Atlas A2 推理系列产品数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |

<!-- end id10 -->

<!-- npu="310b" id11 -->

**表**  Atlas 200I/500 A2 推理产品数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |

<!-- end id11 -->

<!-- npu="310p" id12 -->

**表**  Atlas 推理系列产品AI Core数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |

<!-- end id12 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#section668772811100)。
- 使用tensor高维切分计算接口时，一个迭代处理的源操作数元素个数需要和目的操作数保持一致，而一次计算最多输入128个half类型数据，因此目的操作数最多输出128个b8类型数据，所以每次迭代输出到前4个datablock，srcRepStride需要设置为8，dstRepStride则设置为4。
<!-- npu="A3,910b,950" id13 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id14 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id15 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id17 -->
<!-- end id13 -->
- 本指令涉及精度转换，转换规则参考[精度转换规则](../../数据结构/precision_conversion.md)。
<!-- npu="950" id18 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算接口占用8KB Unified Buffer临时空间。
  - tensor前n个数据连续计算接口不涉及8KB Unified Buffer临时空间的占用。
<!-- end id18 -->

## 调用示例<a name="section642mcpsimp"></a>

- tensor高维切分计算样例-mask连续模式。

  ```cpp
  uint64_t mask = 128;
  // repeatTime = 4，一次迭代计算128个数，共计算512个数
  // dstBlkStride, src0BlkStride, src1BlkStride = 1，单次迭代内数据连续读取和写入
  // dstRepStride = 4，相邻迭代间数据连续写入
  // src0RepStride, src1RepStride = 8，相邻迭代间数据连续读取
  AscendC::MulCast(dstLocal, src0, src1Local, mask, repeatTime, repeatParams);
  ```

- tensor高维切分计算样例-mask逐bit模式。

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX }; 
  // repeatTime = 4，一次迭代计算128个数，共计算512个数
  // dstBlkStride, src0BlkStride, src1BlkStride = 1，单次迭代内数据连续读取和写入
  // dstRepStride = 4，相邻迭代间数据连续写入
  // src0RepStride, src1RepStride = 8，相邻迭代间数据连续读取
  AscendC::MulCast(dstLocal, src0, src1Local, mask, repeatTime, repeatParams);
  ```

- tensor前n个数据计算样例。

  ```cpp
  AscendC::MulCast(dstLocal, src0, src1Local, 512);
  ```

结果示例如下：

```plain
输入数据src0: [1 -2 3 ... -6]
输入数据src1Local: [1 3 -4 ... 5]
输出数据dstLocal: [1 -6 -12 ... -30]
```
