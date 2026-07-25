# Compare<a name="ZH-CN_TOPIC_0000001504145204"></a>

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
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_cmpsel_intf.h"`。

逐元素比较两个tensor大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。

支持多种比较模式：

- LT：小于（less than）

- GT：大于（greater than）

- GE：大于或等于（greater than or equal to）

- EQ：等于（equal to）

- NE：不等于（not equal to）

- LE：小于或等于（less than or equal to）

## 函数原型<a name="section620mcpsimp"></a>

- 整个Tensor参与计算

    ```cpp
    dst = src0 < src1;
    dst = src0 > src1;
    dst = src0 <= src1;
    dst = src0 >= src1;
    dst = src0 == src1;
    dst = src0 != src1;
    ```

    <!-- npu="310b" id15 -->
    Atlas 200I/500 A2 推理产品暂不支持整个Tensor参与计算的运算符重载。
    <!-- end id15 -->

- Tensor前n个数据计算

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, uint32_t count)
    ```

- Tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

### 模板参数及接口参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 源操作数数据类型。 |
| U | 目的操作数数据类型。 |
| isSetMask | 保留参数，保持默认值即可。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>dst用于存储比较结果，将dst中uint8_t类型的数据按照bit位展开，由左至右依次表征对应位置的src0和src1的比较结果，如果比较后的结果为真，则对应比特位为1，否则为0。 |
| src0、src1 | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| cmpMode | 输入 | CMPMODE类型，表示比较模式，包括EQ，NE，GE，LE，GT，LT。<br>&bull; LT：src0小于（less than）src1 <br>&bull; GT：src0大于（greater than）src1 <br>&bull; GE：src0大于或等于（greater than or equal to）src1 <br>&bull; EQ：src0等于（equal to）src1 <br>&bull; NE：src0不等于（not equal to）src1 <br>&bull; LE：src0小于或等于（less than or equal to）src1<br> |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[BinaryRepeatParams](../../辅助数据结构/BinaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |
| count | 输入 | 参与计算的元素个数。设置count时，需要保证count个元素所占空间256字节对齐。未对齐部分元素不参与计算，仅完整对齐块有效。 |

### mask/mask[]参数说明

<!-- npu="950" id16 -->
- 针对Ascend 950PR/Ascend 950DT，设置有效。
<!-- end id16 -->

<!-- npu="A3" id17 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，保留参数，设置无效。
<!-- end id17 -->

<!-- npu="910b" id18 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，保留参数，设置无效。
<!-- end id18 -->

<!-- npu="310b" id19 -->
- 针对Atlas 200I/500 A2 推理产品，设置有效。
<!-- end id19 -->

<!-- npu="310p" id20 -->
- 针对Atlas 推理系列产品AI Core，保留参数，设置无效。
<!-- end id20 -->

<!-- npu="910" id21 -->
- 针对Atlas 训练系列产品，保留参数，设置无效。
<!-- end id21 -->

<!-- npu="x90" id22 -->
- 针对Kirin X90，保留参数，设置无效。
<!-- end id22 -->

<!-- npu="9030" id23 -->
- 针对Kirin 9030，保留参数，设置无效。
<!-- end id23 -->

## 数据类型

<!-- npu="950" id24 -->
- 针对Ascend 950PR/Ascend 950DT
    - T支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。其中，int8\_t/uint8\_t/int64\_t/uint64\_t/double数据类型仅支持tensor前n个数据计算接口和整个tensor参与计算的运算符重载。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id24 -->

<!-- npu="A3" id25 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品
    - T支持的数据类型为：half（所有CMPMODE都支持）、float（所有CMPMODE都支持）、int32_t（只支持CMPMODE::EQ）。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id25 -->

<!-- npu="910b" id26 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品
    - T支持的数据类型为：half（所有CMPMODE都支持）、float（所有CMPMODE都支持）、int32_t（只支持CMPMODE::EQ）。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id26 -->

<!-- npu="310b" id27 -->
- 针对Atlas 200I/500 A2 推理产品
    - T支持的数据类型为：half、float。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id27 -->

<!-- npu="310p" id28 -->
- 针对Atlas 推理系列产品AI Core
    - T支持的数据类型为：half、float。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id28 -->

<!-- npu="910" id29 -->
- 针对Atlas 训练系列产品
    - T支持的数据类型为：half、float。
    - U支持的数据类型为：int8_t、uint8_t。
<!-- end id29 -->

<!-- npu="x90" id30 -->
- 针对Kirin X90
    - T支持的数据类型为：half（所有CMPMODE都支持）、float（所有CMPMODE都支持）、int32_t（只支持CMPMODE::EQ）。
    - U支持的数据类型为：uint8_t。
<!-- end id30 -->

<!-- npu="9030" id31 -->
- 针对Kirin 9030
    - T支持的数据类型为：half（所有CMPMODE都支持）、float（所有CMPMODE都支持）、int32_t（只支持CMPMODE::EQ）。
    - U支持的数据类型为：uint8_t。
<!-- end id31 -->

## 返回值说明<a name="section128671456102513"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。

- dst按照小端顺序排序成二进制结果，对应src中相应位置的数据比较结果。

<!-- npu="A3,910b,950" id10 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id11 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id12 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id12 -->
    <!-- npu="910b" id13 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id13 -->
  <!-- end id11 -->
  <!-- npu="950" id14 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id14 -->
<!-- end id10 -->

- **使用整个tensor参与计算的运算符重载功能，src0和src1需满足256字节对齐；使用tensor前n个数据参与计算的接口，设置count时，需要保证count个元素所占空间256字节对齐。**

## 调用示例<a name="section642mcpsimp"></a>

本样例中，源操作数src0和src1各存储了256个float类型的数据。样例实现的功能为，逐元素对src0和src1中的数据进行比较，如果src0中的元素小于src1中的元素，dst结果中对应的比特位置1；反之，则置0。dst结果使用uint8\_t类型数据存储。

完整的调用样例可参考[Compare类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare)场景一。

- 整个tensor参与计算

    ```cpp
    dstLocal = src0Local < src1Local;  // 小于LT
    dstLocal = src0Local > src1Local;  // 大于GT
    dstLocal = src0Local <= src1Local; // 小于等于LE
    dstLocal = src0Local >= src1Local; // 大于等于GE
    dstLocal = src0Local == src1Local; // 等于EQ
    dstLocal = src0Local != src1Local; // 不等于NE
    ```

- tensor前n个数据计算

    ```cpp
    // srcDataSize：参与计算的元素个数
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, srcDataSize);
    ```

    结果示例如下：

    ```plain
    LT：小于
    输入数据src0Local：[ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    输入数据src1Local：[ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    输出数据dstLocal： [ 0 127 127 127 0 0 0 0 ]

    GT：大于
    输入数据src0Local：[ 2 3 5 7 11 13 17 19 ... ]
    输入数据src1Local：[ 2 4 6 8 10 12 14 16 ... ]
    逐元素比较结果：   [ 0 0 0 0  1  1  1  1 ... ]
    输出数据dstLocal： [ 240(0b11110000) ... ]

    GE：大于或等于
    输入数据src0Local：[ 2 3 5 7 11 13 17 19 ... ]
    输入数据src1Local：[ 2 4 6 8 10 12 14 16 ... ]
    输出数据dstLocal： [ 241(0b11110001) ... ]

    LE：小于或等于
    输入数据src0Local：[ 2 3 5 7 11 13 17 19 ... ]
    输入数据src1Local：[ 2 4 6 8 10 12 14 16 ... ]
    输出数据dstLocal： [ 15(0b00001111) ... ]

    EQ：等于
    输入数据src0Local：[ 2 3 5 7 11 13 17 19 ... ]
    输入数据src1Local：[ 2 4 6 8 10 12 14 16 ... ]
    输出数据dstLocal： [ 1(0b00000001) ... ]

    NE：不等于
    输入数据src0Local：[ 2 3 5 7 11 13 17 19 ... ]
    输入数据src1Local：[ 2 4 6 8 10 12 14 16 ... ]
    输出数据dstLocal： [ 126(0b11111110) ... ]
    ```

- Tensor高维切分计算，mask逐bit模式

    ```cpp
    // masks数组控制每次迭代参与计算的元素，两个uint64_t的值一共128bit，每个bit可以控制一个元素，为1则参与计算，为0则不参与计算
    // masks[0]可以控制前64个元素，低bit位控制索引小的元素；masks[1]同理，可以控制后64个元素
    // 例如，对float类型数据，每次迭代处理256B / sizeof(float) = 64个元素，因此只需要通过masks[0]即可进行控制
    uint64_t masks[2] = {858993459, 0}; // 858993459(0x33333333)
    // repeat: 1, dstBlkStride: 1, src0BlkStride: 1, src1BlkStride: 1, dstRepStride: 1, src0RepStride: 8, src1RepStride: 8
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    ```

    结果示例如下：

    ```plain
    LE：小于等于
    输入数据src0Local：[ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    输入数据src1Local：[ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    输入数据masks：{ 858993459, 0 }
    输出数据dstLocal： [ 51 51 51 51 0 0 0 0 ]
    ```

- Tensor高维切分计算，mask连续模式

    ```cpp
    // mask控制每次迭代参与计算的连续元素个数
    // 例如，对float类型数据，每次迭代处理256B / sizeof(float) = 64个元素，因此mask可取值1至64
    uint64_t mask = 28;
    // repeat: 1, dstBlkStride: 1, src0BlkStride: 1, src1BlkStride: 1, dstRepStride: 1, src0RepStride: 8, src1RepStride: 8
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    ```

    结果示例如下：

    ```plain
    LE：小于等于
    输入数据src0Local：[ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    输入数据src1Local：[ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    输入数据mask：28
    输出数据dstLocal： [ 127 127 127 16 0 0 0 0 ]
    ```
