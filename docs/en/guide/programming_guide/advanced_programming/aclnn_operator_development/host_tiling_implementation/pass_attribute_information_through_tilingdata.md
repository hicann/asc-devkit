# Passing Attribute Information Through TilingData<a name="ZH-CN_TOPIC_0000002399353401"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T12:03:46.896Z pushedAt=2026-09-02T11:54:47.467Z -->

If an operator contains attribute information, the attribute information can be passed to the kernel side through TilingData to participate in the computation of the operator kernel function on the kernel side. Take the ReduceMaxCustom operator as an example. This operator returns the maximum value of the input data along the dimension dim and also returns the index. The ReduceMaxCustom operator has two attributes, reduceDim and isKeepDim. reduceDim indicates the dimension along which the reduce operation is performed, and isKeepDim indicates whether the output dimension needs to be kept the same as the input. This sample supports only the reduce operation on the last dimension, and the input data type is half.

1.  The TilingData of the ReduceMaxCustom operator is defined as follows. Here we focus on reduceAxisLen. The parameter reduceAxisLen indicates the length of the reduceDim axis, which is the length of the last dimension here. This parameter is subsequently passed to the kernel side through TilingData to participate in the computation.

    ```
    #ifndef REDUCE_MAX_CUSTOM_TILING_H
    #define REDUCE_MAX_CUSTOM_TILING_H
    #include "register/tilingdata_base.h"
    namespace optiling {
    BEGIN_TILING_DATA_DEF(ReduceMaxTilingData)
      TILING_DATA_FIELD_DEF(uint32_t, reduceAxisLen); // Add the tiling field, the length of the reduceDim axis
      //Definitions of other TilingData parameters
      ...
    END_TILING_DATA_DEF;
    // Register the operator tilingdata class to the corresponding ReduceMaxCustom operator
    REGISTER_TILING_DATA_CLASS(ReduceMaxCustom, ReduceMaxTilingData)
    }
    #endif // REDUCE_MAX_CUSTOM_TILING_H
    ```

2.  The tiling implementation of the ReduceMaxCustom operator is as follows. Here we focus on the process of passing attribute information through TilingData: first, obtain the reduceDim attribute value from attr through the TilingContext context; then, obtain the length of the reduceDim axis based on the reduceDim attribute value and set it to TilingData.

    ```
    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        ReduceMaxTilingData tiling;
        // Obtain the reduceDim attribute value from attr. Because reduceDim is the first attribute, the index value passed to GetAttrPointer is 0
        const gert::RuntimeAttrs* attrs = context->GetAttrs();
        const uint32_t* reduceDim = attrs->GetAttrPointer<uint32_t>(0);
        // Obtain the length of the reduceDim axis
        const gert::StorageShape* xShapePtr = context->GetInputShape(0);
        const gert::Shape& xShape = xShapePtr->GetStorageShape();
        const uint32_t reduceAxisLen = xShape.GetDim(*reduceDim);
        // Calculate the values of the other member variables in TilingData except reduceAxisLen
        ...
        // Set reduceAxisLen in the tiling structure and pass it to the kernel function for use
        tiling.set_reduceAxisLen(reduceAxisLen);
        // Set the values of the other member variables in TilingData except reduceAxisLen
        ...
        // Serialize and save TilingData
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        ...
        return ge::GRAPH_SUCCESS;
    }} // namespace optiling
    ```

