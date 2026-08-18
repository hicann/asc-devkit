# Transpose Tiling

## 功能说明

用于获取Transpose Tiling参数。

## 函数原型

> [!NOTE]说明
>GetConfusionTransposeMaxMinTmpSize、GetConfusionTransposeTilingInfo、GetConfusionTransposeOnlyTilingInfo接口废弃，并将在后续版本移除，请不要使用该接口。请使用GetTransposeMaxMinTmpSize、GetTransposeTilingInfo接口。

-   获取最小临时空间大小

    ```
    void GetTransposeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const uint32_t transposeTypeIn, uint32_t& maxValue, uint32_t& minValue)
    ```

    ```
    void GetConfusionTransposeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const uint32_t transposeTypeIn, uint32_t& maxValue, uint32_t& minValue)
    ```

-   获取Transpose Tiling

    ```
    void GetTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeOnlyTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeOnlyTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

## 参数说明

**表1** **GetTransposeMaxMinTmpSize接口参数说明**

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| srcShape | 输入 | 输入Tensor的shape信息，参数类型为[AscendC::TensorShape](../data_structures/TensorShape.md)，具体srcShape传入格式为：<br><br>场景1：[B, N, S, H/N]<br><br>场景2：[B, N, S, H/N]<br><br>场景3：[B, N, S, H/N]<br><br>场景4：[B, N, S, H/N]<br><br>场景5：[B, N, S, H/N]<br><br>场景6：[B, N, S, H/N]<br><br>场景7：[H, W]<!-- npu="950" id1 --><br><br>场景13：[H, W]或者[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景14：[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景15：[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景16：[H, W]，仅支持Ascend 950PR/Ascend 950DT。<!-- end id1 --> |
| typeSize | 输入 | 输入的数据类型大小，单位为字节。比如输入的数据类型为half，此处应传入2。 |
| transposeTypeIn | 输入 | 选择数据排布及reshape的类型，根据输入数字选择对应的场景，参数范围为[1,7]。<!-- npu="950" id2 -->针对Ascend 950PR/Ascend 950DT，该参数取值范围为[1, 7]和[13, 16]。<!-- end id2 --><br><br>场景1（NZ2ND，1、2轴互换）：1<br><br>场景2（NZ2NZ，1、2轴互换）：2<br><br>场景3（NZ2NZ，尾轴切分）：3<br><br>场景4（NZ2ND，尾轴切分）：4<br><br>场景5（NZ2ND，尾轴合并）：5<br><br>场景6（NZ2NZ，尾轴合并）：6<br><br>场景7（二维转置）：7<!-- npu="950" id5 --><br><br>场景13 （二维转置或者三维转置中后两维转置）：13，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景14 （三维转置中第一维和第二维互换）：14，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景15 （三维转置中第一维和第三维互换）：15，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景16 （使用交织指令进行两维ND2NZ转置）：16，仅支持Ascend 950PR/Ascend 950DT。<!-- end id5 --> |
| maxValue | 输出 | Transpose接口能完成计算所需的最大临时空间大小，超出该值的空间不会被该接口使用。在最小临时空间-最大临时空间范围内，随着临时空间增大，kernel侧接口计算性能会有一定程度的优化提升。为了达到更好的性能，开发者可以根据实际的内存使用情况进行空间预留/申请。<br>maxValue仅作为参考值，有可能大于Unified Buffer（UB）剩余空间的大小，该场景下，开发者需要根据UB剩余空间的大小来选取合适的临时空间大小。 |
| minValue | 输出 | Transpose接口能完成计算所需最小临时空间大小。为保证功能正确，接口计算时预留/申请的临时空间不能小于该数值。 |

**表2** **GetTransposeTilingInfo接口参数列表**

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| srcShape | 输入 | 输入的shape信息，参数类型为[AscendC::TensorShape](../data_structures/TensorShape.md)，具体srcShape传入格式为：<br><br>场景1：[B, N, S, H/N]<br><br>场景2：[B, N, S, H/N]<br><br>场景3：[B, N, S, H/N]<br><br>场景4：[B, N, S, H/N]<br><br>场景5：[B, N, S, H/N]<br><br>场景6：[B, N, S, H/N]<br><br>场景7：[H, W]<!-- npu="950" id3 --><br><br>场景13：[H, W]或者[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景14：[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景15：[N, H, W]，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景16 ：[H, W]，仅支持Ascend 950PR/Ascend 950DT。<!-- end id3 --> |
| stackBufferSize | 输入 | 可供Transpose接口计算的空间大小，单位Byte。 |
| typeSize | 输入 | 输入的数据类型大小，单位为字节。比如输入的数据类型为half，此处应传入2。 |
| transposeTypeIn | 输入 | 选择数据排布及reshape的类型，根据输入数字选择对应的场景，参数范围为[1,7]。<!-- npu="950" id4 --><br>Ascend 950PR/Ascend 950DT，该参数取值范围为[1, 7]和[13, 16]。<!-- end id4 --><br><br>场景1（NZ2ND，1、2轴互换）：1<br><br>场景2（NZ2NZ，1、2轴互换）：2<br><br>场景3（NZ2NZ，尾轴切分）：3<br><br>场景4（NZ2ND，尾轴切分）：4<br><br>场景5（NZ2ND，尾轴合并）：5<br><br>场景6（NZ2NZ，尾轴合并）：6<br><br>场景7（二维转置）：7<!-- npu="950" id6 --><br><br>场景13 （二维转置或者三维转置中后两维转置）：13，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景14 （三维转置中第一维和第二维互换）：14，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景15 （三维转置中第一维和第三维互换）：15，仅支持Ascend 950PR/Ascend 950DT。<br><br>场景16（使用交织指令进行两维ND2NZ转置）：16，仅支持Ascend 950PR/Ascend 950DT。<!-- end id6 --> |
| tiling | 输出 | 输入数据的切分信息。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

如下样例介绍了使用Transpose高阶API时host侧获取Tiling参数的流程以及该参数如何在kernel侧使用。样例中为场景1，输入Tensor的shape大小为\[1, 2, 64, 32\]，输入的数据类型为half。

1.  将ConfusionTransposeTiling结构体参数增加至TilingData结构体，作为TilingData结构体的一个字段。

    ```
    BEGIN_TILING_DATA_DEF(TilingData)         // 注册一个tiling的类，以tiling的名字作为入参
        TILING_DATA_FIELD_DEF(uint32_t, tileNum); // 添加tiling字段，每个核上总计算数据分块个数
        ...                                       // 添加其他tiling字段
        TILING_DATA_FIELD_DEF_STRUCT(
            ConfusionTransposeTiling,
            confusionTransposeTilingData); // 将ConfusionTransposeTiling结构体参数增加至TilingData结构体
    END_TILING_DATA_DEF;
    ```

2.  Tiling实现函数中，首先调用**GetTransposeMaxMinTmpSize**接口获取Transpose接口能完成计算所需最大和最小临时空间大小，根据该范围结合实际的内存使用情况设置合适的空间大小；然后根据输入shape、剩余的可供计算的空间大小等信息获取Transpose kernel侧接口所需Tiling参数。

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetSimdNumBlocks(NUM_BLOCKS);
        tiling.set_tileNum(TILE_NUM);
        // 设置其他Tiling参数
        ...
        std::vector<int64_t> shapeVec = {1, 2, 64, 32};
        AscendC::TensorShape srcShape(shapeVec);

        uint32_t transposeTypeIn = 1;
        uint32_t maxValue = 0;
        uint32_t minValue = 0;
        AscendC::GetTransposeMaxMinTmpSize(srcShape, sizeof(half), transposeTypeIn, maxValue, minValue);
        // 本样例为保证功能正确，传入最小临时空间大小，开发者可以根据需要传入合适的空间大小
        const uint32_t stackBufferSize = minValue;
        // 获取Transpose接口所需的Tiling参数
        AscendC::GetTransposeTilingInfo(
            srcShape, stackBufferSize, sizeof(half), transposeTypeIn, tiling.confusionTransposeTilingData);
        ...
        // 其他逻辑
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  对应的kernel侧通过在核函数（Kernel）中调用GET\_TILING\_DATA获取TilingData，继而将TilingData中的ConfusionTransposeTiling信息传入Transpose接口参与计算。完整的kernel侧样例请参考[Transpose](Transpose.md)。

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR src_gm, GM_ADDR dst_gm, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(TilingData, tiling);
        KernelTranspose<half> op;
        op.Init(src_gm, dst_gm, TilingData.confusionTransposeTilingData);
        op.Process();
    }
    ```
