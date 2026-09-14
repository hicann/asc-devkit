# Basic Development Process<a name="ZH-CN_TOPIC_0000001985333472"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T11:32:57.868Z pushedAt=2026-09-01T07:03:11.083Z -->

This development process is based on [engineering-based operator development](../aclnn_operator_development/overview.md). In addition to the operator implementation files required in engineering-based operator development, you also need to deliver the code files for operator graph integration. This section provides only the development guide for the operator graph integration code files.

Assume that the following figure shows the network model to be used. You may think that you can run the network by simply calling operators one by one and obtaining the output tensor from the input tensor. However, in graph mode, tensor shape and datatype inference is performed first during actual network model generation. This allows you to know the data type and shape of each tensor before graph execution and verify their correctness in advance. At the same time, the output tensor descriptors of operators, including the tensor shape, data type, and data layout format, are inferred in advance, so that memory can be statically allocated for all tensors during the operator graph construction preparation phase, avoiding the overhead of dynamic memory allocation.

After shape and datatype inference, the network model below yields the inference information shown in the gray-shaded boxes:

**Figure 1**  Shape and datatype inference diagram<a name="fig1729122183718"></a>  
![](../../../figures/shape_and_datatype_inference_diagram.png "Shape and datatype inference diagram")

In addition to the tiling implementation, the following implementation code must be additionally provided for operator graph integration:

-   Datatype inference: infers the output tensor datatype of an operator based on the input datatype, operator logic, and operator attributes.
-   Shape inference: infers the output tensor shape of an operator based on the input shape, operator logic, and operator attributes.
-   ShapeRange inference: when the output shape cannot be inferred at compile time, only the output shape range can be inferred, and the output shape can be obtained only after execution. During delivery, the maximum output memory must be allocated based on the output shape range. Operators of this type must provide a ShapeRange inference function.
-   Data dependency declaration: some operators require the specific value of an input to perform InferShape. Such operators are called "data-dependent operators", and the corresponding input is called a "data-dependent input". When registering such operators, you need to declare their data-dependent inputs.

The following table lists the requirements of different types of operators on the preceding implementation code.

**Table 1** Requirements of different types of operators on graph-building implementation code

<a name="table772183710452"></a>
<table><thead align="left"><tr id="row1372110374457"><th class="cellrowborder" valign="top" width="35.8%" id="mcps1.2.3.1.1"><p id="p572263714456"><a name="p572263714456"></a><a name="p572263714456"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="64.2%" id="mcps1.2.3.1.2"><p id="p117222376454"><a name="p117222376454"></a><a name="p117222376454"></a>Requirement for Graph-Building Implementation Code</p></th>
</tr>
</thead>
<tbody><tr id="row1672217377453"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p197221937114512"><a name="p197221937114512"></a><a name="p197221937114512"></a>The output shape can be inferred from the input shape.</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><a name="ul126301833144813"></a><a name="ul126301833144813"></a><ul id="ul126301833144813"><li>Shape inference</li><li>Datatype inference</li></ul></td>
</tr>
<tr id="row1472216375456"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p87221437114518"><a name="p87221437114518"></a><a name="p87221437114518"></a>The output shape can be inferred only by depending on the value of an input, that is, a data-dependent operator. For example, the Reshape operator depends on the value of the shape input to infer the output shape.</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><a name="ul51966562363"></a><a name="ul51966562363"></a><ul id="ul51966562363"><li>Shape inference</li><li>Datatype inference</li><li>Declare data dependency</li></ul></td>
</tr>
<tr id="row16722103711454"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p1272233734515"><a name="p1272233734515"></a><a name="p1272233734515"></a>The output shape cannot be inferred at compile time. Only the output shape range can be inferred, and the output shape can be obtained only after execution.</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><a name="ul71712171414"></a><a name="ul71712171414"></a><ul id="ul71712171414"><li>Shape inference (required)</li><li>DataType inference (required)</li><li>ShapeRange inference (required)</li><li>Declare data dependency (as required)</li></ul></td>
</tr>
</tbody>
</table>

During actual development, implement the inference functions using the fixed datatype and shape inference prototypes, and then associate the corresponding shape inference functions through the SetInferShape and SetInferDataType APIs. The following is an example.

```
namespace ge {
static graphStatus InferShape(gert::InferShapeContext *context)
{
    ...
    return GRAPH_SUCCESS;
}

static graphStatus InferDataType(gert::InferDataTypeContext *context)
{
    ...
    return ge::GRAPH_SUCCESS;
}
} // namespace ge


namespace ops {
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        // Determine whether registration is required based on how the user calls the operator. Registration is required in graph mode.
        this->SetInferShape(ge::InferShape);
        this->SetInferShapeRange(ge::InferShapeRange);
        this->SetInferDataType(ge::InferDataType);  
        this->AICore()
            .SetTiling(optiling::TilingFunc);
        // Replace it with the actual Ascend AI Processor model.
        this->AICore().AddConfig("ascendxxx");
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

## Datatype Inference<a name="section1145010781013"></a>

Take the AddCustom operator as an example. The implementation of InferDataType is as follows. In this example, the data type of the output tensor is the same as that of the input tensor, so the data type of any input tensor can be directly assigned to the output tensor.

```
namespace ge {
static graphStatus InferDataType(gert::InferDataTypeContext* context)
{
    const auto inputDataType = context->GetInputDataType(0);
    context->SetOutputDataType(0, inputDataType);
    return ge::GRAPH_SUCCESS;
}
} // namespace ge
```

The following example provides a more flexible datatype inference example. When the input data type is DT\_INT4, the output data type is DT\_INT32.

```
ge::graphStatus InferDataTypeForFoo(gert::InferDataTypeContext* context) {

    if (context->GetInputDataType(0) == DT_INT4) {
        context->SetOutputDataType(0, DT_INT32);
    }
}
```

## Shape Inference<a name="section358205411221"></a>

Simple shape inference logic can be expressed using the Follow API, for example, when the output shape is the same as the input shape. The following example shows a scenario where output "y1" follows input "x1" with the Follow mode set to SHAPE, in which case the shape of "y1" remains consistent with that of "x1".

```
this->Input("x1")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND});
this->Input("x2")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND});
this->Output("y1")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND})
    .Follow("x1", FollowType::SHAPE);
```

For cases that cannot be expressed through Follow in the prototype definition, developers need to write an InferShape function. The prototype of the InferShape function is fixed, as shown in the following example. It accepts an InferShapeContext as input, from which you can obtain the shape pointers of the inputs and outputs. The input shape is of the const type, so during InferShape, the input shape is read-only and cannot be modified. After InferShape succeeds, ge::GRAPH\_SUCCESS is returned; any other return value is considered an inference failure. After an inference failure, the execution process ends and exits.

Taking the Reshape operator as an example, the implementation of InferShape is as follows. Based on the value of the first input (shape input), the Reshape operator transforms the shape of the zeroth input (x input) and outputs it to its zeroth output (y output). The InferShape implementation of Reshape is as follows:

```
ge::graphStatus InferShapeForReshape(InferShapeContext *context) {
const gert::Shape *x_shape = context->GetInputShape(0);        // Obtain the shape of the zeroth input.
const gert::Tensor *shape_tensor = context->GetInputTensor(1); // Obtain the tensor of the first input.
gert::Shape *output_shape = context->GetOutputShape(0);
if (x_shape == nullptr || shape_tensor == nullptr || output_shape == nullptr) {
    // Defensive programming: this scenario should not occur. Print an error and return failure.
    return ge::GRAPH_FAILED;
}

auto reshape_size = static_cast<int32_t>(shape_tensor->GetShapeSize());
if (reshape_size < 1) {
    // Defensive programming: this scenario should not occur. Print an error and return failure.
    return ge::GRAPH_FAILED;
}

// According to the prototype information, the shape input of Reshape supports both INT32 and INT64. Based on the different types, enter the corresponding template function to perform the actual shape transformation.
if (shape_tensor->GetDataType() == ge::DT_INT32) {
    int32_t *reshape_data = shape_tensor->GetData<int32_t>();
    return ReshapeInferShapeImpl<int32_t>(reshape_data, *x_shape, *output_shape, reshape_size);
} else {
    int64_t *reshape_data = shape_tensor->GetData<int64_t>();
    return ReshapeInferShapeImpl<int64_t>(reshape_data, *x_shape, *output_shape, reshape_size);
}
}
```

InferShapeContext publicly inherits from ExtendedKernelContext. Therefore, the methods provided in ExtendedKernelContext, such as obtaining the operator type, name, and attributes, can all be called on an InferShapeContext instance.

> [!CAUTION] Caution
> - The InferShape inference function and the Follow API cannot be used together. That is, the case where some outputs are inferred through InferShape and others through Follow is not supported. If a user uses both the InferShape function and the Follow API, the user's InferShape function takes precedence, and it must be able to infer the shapes of all outputs.
> - For efficiency, when the InferShape function is called, the framework does not initialize the output shapes. Therefore, within the InferShape function, the outputs can be considered **uninitialized**. If you want to operate on the output shapes in Append mode during InferShape, clear the DimNum of the output shapes first to prevent undefined behavior.

## InferShapeRange Implementation<a name="section1168614153115"></a>

For some operators, the output shape can be determined only after computation is complete. For example, the shape inference logic of the unique operator is as follows:

Given a one-dimensional tensor x, find the unique elements in it, return the deduplicated tensor y, and output idx, which has the same size as the input x and stores the index of each element of x in y.

```
# tensor 'x' is [1, 1, 2, 4, 4, 4, 7, 8, 8]               x shape[9]
y, idx = unique(x)
y ==> [1, 2, 4, 7, 8]                                      y shape[5] 
idx ==> [0, 0, 1, 2, 2, 2, 3, 4, 4]                        idx shape[9]
```

Therefore, the shape of y is \[-1\] at compile time and is determined only after the unique operation is executed.

In the graph construction scenario, output memory must be allocated before execution, and the memory size depends on the output shape and data type. For such operators, because the output shape can be determined only after execution, output memory must be allocated based on the maximum range of the output shape to ensure that there is sufficient space for the computation function to write the output tensor.

In this scenario, developers need to implement the InferShapeRange function themselves to infer the range of the output shape. The following uses the unique operator as an example to describe how to implement the InferShapeRange function.

```
ge::graphStatus UniqueInferShapeRangeFunc(gert::InferShapeRangeContext *context) {
  // Obtain the input shape range
  auto x_shape_range = context->GetInputShapeRange(0U);
  OPS_CHECK_NULL_WITH_CONTEXT(context, x_shape_range);
  OPS_CHECK_NULL_WITH_CONTEXT(context, x_shape_range->GetMax());
  OPS_CHECK_NULL_WITH_CONTEXT(context, x_shape_range->GetMin());

  // Start computing the shape range of output y
  auto y_shape_range = context->GetOutputShapeRange(0U);
  OPS_CHECK_NULL_WITH_CONTEXT(context, y_shape_range);
  y_shape_range->GetMax()->SetDimNum(1); // One-dimensional vector with rank 1
  y_shape_range->GetMin()->SetDimNum(1);

  auto x_max_shape = x_shape_range->GetMax();
  auto x_shape_dimnum = x_max_shape->GetDim(0); // x is a one-dimensional tensor with shape [n], and x_shape_dimnum indicates the number of elements n in input x
  if (x_shape_dimnum == 1) {
    // If the x input has only one element, no deduplication occurs, and both the minimum and maximum of the y shape axis are 1. Therefore, the range is [1~1].
    y_shape_range->GetMax()->SetDim(0, 1);
    y_shape_range->GetMin()->SetDim(0, 1);
  } else {
    // If the x input has zero elements or more than one element, after deduplication, the minimum number of elements in y is the min of x, and the maximum is the max of x.
    y_shape_range->GetMax()->SetDim(0, x_shape_dimnum);
    y_shape_range->GetMin()->SetDim(0, x_shape_range->GetMin());
  }

  // Start calculating the shape range of the output idx.
  // The output idx represents the index of each x element in y. Its number of elements equals that of x, so its shape range is the same as that of x.
  auto idx_shape_range = context->GetOutputShapeRange(1U);
  OPS_CHECK_NULL_WITH_CONTEXT(context, idx_shape_range);
  *(idx_shape_range->GetMax()) = *(x_shape_range->GetMax());
  *(idx_shape_range->GetMin()) = *(x_shape_range->GetMin());

  return ge::GRAPH_SUCCESS;
}
```

## Obtaining Attributes and Inputs During InferShape<a name="section108351240182418"></a>

During InferShape and tiling, you can obtain the operator IR attribute values through the context instance. IR attributes refer to the attributes defined during IR registration. Take the TransData operator as an example:

```
namespace ops {
class TransData : public OpDef {
public:
    explicit TransData(const char *name) : OpDef(name)
    {
        this->Input("src")
             ...
        this->Output("dst")
             ...
        this->Attr("src_format")
            .AttrType(REQUIRED)
            .String();
        this->Attr("dst_format")
            .AttrType(REQUIRED)
            .String();
        this->Attr("group")
            .AttrType(OPTIONAL)           
           .Int(1);
        ...
    }
};
OP_ADD(TransData);
} // namespace ops
```

Its prototype definition declares three attributes: src\_format, dst\_format, and group. You can obtain the operator attributes as follows:

```
ge::graphStatus ExampleGetTransDataAttr(TilingContext *context) {
  // Obtain all attributes
  const RuntimeAttrs *attrs = context->GetAttrs();
  ASSERT_NOT_NULL(attrs);
  
  // Obtain attributes by index in the order defined in the prototype. The index starts from 0.
  const char *src_format = attrs->GetAttrPointer<char>(0);  // Obtain src_format. src_format is the first attribute, so its index is 0.
  const char *dst_format = attrs->GetAttrPointer<char>(1);  // Obtain dst_format. dst_format is the second attribute, so its index is 1.
  const int64_t group = attrs->GetAttrPointer<int64_t>(2);  // Obtain group. group is the third attribute, so its index is 2.
  
  return ge::GRAPH_SUCCESS;
}
```

Inputs and outputs are indexed by index rather than by string name. For operators with OPTIONAL or DYNAMIC inputs, after instantiation, a specific input may not be indexed solely by index. Take the DynamicRNNV3 operator as an example:

```
namespace ops {
class DynamicRNNV3 : public OpDef {
public:
    explicit DynamicRNNV3(const char *name) : OpDef(name)
    {
        this->Input("x")
             .ParamType(REQUIRED)
             ...
        this->Input("w")
             .ParamType(REQUIRED)
             ...
        this->Input("b")
             .ParamType(REQUIRED)
             ...
        this->Input("seq_length")
             .ParamType(OPTIONAL)
             ...
        this->Input("init_h")
             .ParamType(OPTIONAL)
             ...
        this->Input("init_c")
             .ParamType(OPTIONAL)
             ...
        this->Input("wci")
             .ParamType(OPTIONAL)
             ...
        this->Input("wcf")
             .ParamType(OPTIONAL)
             ...
        this->Input("mask")
             .ParamType(OPTIONAL)
             ...
        this->Input("mask")
             .ParamType(OPTIONAL)
             ...
        this->Input("project")
             .ParamType(OPTIONAL)
             ...
       
        ...
    }
};
OP_ADD(DynamicRNNV3);
} // namespace ops
```

Because the DynamicRNNV3 operator has multiple consecutive optional inputs, the post-instantiation indexes of init\_h and the inputs after it are uncertain. For operators of this type, you can call GetOptionalInputShape and pass the corresponding index in the prototype to obtain the input shape and other data. Take InferShape as an example:

```
ge::graphStatus InferShapeForDynamicRNNV3(InferShapeContext *context) {
  // For the first two inputs, which are not affected by optional or dynamic inputs, obtain the input shape in the conventional way.
  auto x_shape = context->GetInputShape(0);
  auto w_shape = context->GetInputShape(1);
  if (x_shape == nullptr || w_shape == nullptr) {
    return ge::GRAPH_FAILED;
  }

  int64_t state_size = 0;
  // In the prototype definition, project is the 11th input (counting from 0).
  constexpr int64_t kProjectInputIndex = 11;

  // Affected by the preceding optional inputs, the index of the instantiated project input is uncertain. Use GetOptionalInputShape to obtain the corresponding input shape.
  // The input parameter of GetOptionalInputShape is the corresponding index in the prototype.
  auto project_shape = context->GetOptionalInputShape(kProjectInputIndex);
  if (project_shape != nullptr) {
    if (project_shape->GetDimNum() < 2) {
      return ge::GRAPH_FAILED;
    }
    state_size = project_shape->GetDim(1);
  }
  // More infershape logic...
  return ge::GRAPH_SUCCESS;
}
```

For dynamic inputs, one or more inputs may be instantiated. To obtain such inputs, use the following method:

```
// ir_index: index of this input in the prototype definition, counting from 0.
// relative_index: relative index of the instantiated input, counting from 0. For example, if a DYNAMIC_INPUT is instantiated three times and you want to obtain the second one, set relative_index to 1.
auto shape = context->GetDynamicInputShape(ir_index, relative_index);
```

The methods for obtaining optional and dynamic inputs described in this section can be called in both InferShape and Tiling functions.

## Data Dependency<a name="section0610144611487"></a>

Generally, once the input shape is available, an operator can infer the output shape through InferShape. However, some operators require the specific value of an input to perform InferShape. Such operators are called "data-dependent operators", and the corresponding inputs are called "data-dependent inputs". Take the Reshape operator as an example. It adjusts the input shape based on the description of the shape input, so the Reshape operator depends on the value of the shape input. For such operators, the corresponding input must be declared as a data-dependent input through the ValueDepend API during prototype definition.

```
namespace ops {
class Reshape : public OpDef {
public:
    explicit Reshape(const char *name) : OpDef(name)
    {
        ...
        this->Input("shape")
             .ParamType(REQUIRED)
             ...
             .ValueDepend(REQUIRED) // Declare the shape input of the Reshape operator as a data-dependent input
        ...
    }
};
OP_ADD(Reshape);
} // namespace ops
```

Based on the value of the first input (shape input), the Reshape operator transforms the shape of the zeroth input (x input) and outputs it to its zeroth output (y output). The InferShape implementation of Reshape is as follows:

```
// Specific implementation of shape transformation
template<typename T>
ge::graphStatus ReshapeInferShapeImpl(const T *reshape_dims, const gert::Shape &x_shape, gert::Shape &output_shape, int32_t reshape_rank) {
    constexpr T UNKNOWN_DIM = -1;
    // Set the number of dimensions of the operator output to reshape_rank, the number of dimensions after reshape
    output_shape.SetDimNum(reshape_rank);
    auto x_shape_size = x_shape.GetShapeSize();
    int64_t output_shapesize = 1;
    size_t unknown_dim_idx = std::numeric_limits<size_t>::max();
    for (int32_t i = 0; i < reshape_rank; i++) {
        if (reshape_dims[i] != UNKNOWN_DIM) {  // The dimension value of an axis after reshape is not -1
            output_shape.SetDim(i, reshape_dims[i]);  // Set the output dimension value to the dimension value after reshape
            output_shapesize *= reshape_dims[i];  // Calculate the number of elements in the current output
        } else {
            output_shape.SetDim(i, 1);  // The dimension value of an axis after reshape is -1. Temporarily set the output dimension value to 1, check whether a definite value can be derived after subsequent calculation, and record the index of the unknown dimension
            unknown_dim_idx = i;
        }
    }
    if (unknown_dim_idx == std::numeric_limits<size_t>::max() && output_shapesize == x_shape_size) {
        return ge::GRAPH_SUCCESS;  // No unknown dimension exists, and the output shape size is the same as the input x shape size, so success is returned directly.
    } else if (unknown_dim_idx != std::numeric_limits<size_t>::max() && x_shape_size % output_shapesize == 0) {
        output_shape.SetDim(unknown_dim_idx, x_shape_size / output_shapesize); // An unknown dimension exists, so the unknown dimension value is dynamically adjusted based on the input shape to keep the total number of elements unchanged.
        return ge::GRAPH_SUCCESS;
    }
    return ge::GRAPH_FAILED;
}

ge::graphStatus InferShapeForReshape(InferShapeContext *context) {
    const gert::Shape *x_shape = context->GetInputShape(0);        // Obtain the shape of input 0.
    const gert::Tensor *shape_tensor = context->GetInputTensor(1); // Obtain the tensor of input 1.
    gert::Shape *output_shape = context->GetOutputShape(0);
    if (x_shape == nullptr || shape_tensor == nullptr || output_shape == nullptr) {
        // Defensive programming: this scenario should not occur. Print an error and return failure.
        return ge::GRAPH_FAILED;
    }

    auto reshape_size = static_cast<int32_t>(shape_tensor->GetShapeSize());
    if (reshape_size < 1) {
        // Defensive programming: this scenario should not occur. Print an error and return failure.
        return ge::GRAPH_FAILED;
    }

    // According to the prototype information, the shape input of Reshape supports both INT32 and INT64. Based on the type, enter the corresponding template function to perform the actual shape transformation.
    if (shape_tensor->GetDataType() == ge::DT_INT32) {
        int32_t *reshape_data = shape_tensor->GetData<int32_t>();
        return ReshapeInferShapeImpl<int32_t>(reshape_data, *x_shape, *output_shape, reshape_size);
    } else {
        int64_t *reshape_data = shape_tensor->GetData<int64_t>();
        return ReshapeInferShapeImpl<int64_t>(reshape_data, *x_shape, *output_shape, reshape_size);
    }
}
```

>[!CAUTION] Caution
>- Only for inputs whose data dependency has been declared can you call APIs such as GetInputTensor to obtain the tensor data during InferShape. If you call APIs such as GetInputTensor to obtain the tensor for an input whose data dependency has not been declared, you can only obtain the correct shape, format, and data type information from the tensor, but cannot obtain the actual tensor data address (the obtained address is nullptr).
>- When obtaining tensor\_data from a tensor \(GetData<int32\_t\> or GetData<int64\_t\>\), you must ensure that the obtained data type is correct. Otherwise, the behavior is undefined.

