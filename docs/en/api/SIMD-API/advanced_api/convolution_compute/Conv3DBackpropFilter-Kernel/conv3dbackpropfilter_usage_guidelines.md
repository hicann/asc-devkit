# Conv3DBackpropFilter Usage Guidelines

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:18:34.572Z -->

Ascend C provides a set of **Conv3DBackpropFilter** high-level APIs to help users quickly implement the backward operation of convolution and solve the backpropagation error.

The weight propagation of convolution backpropagation is shown in [Figure 1](#fig1710410547569), and the weight computation of convolution backpropagation is shown in [Figure 2](#fig25291917533).

The calculation formula of **Conv3dBackpropFilter** is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002067618970.png)

-   X is the feature matrix input of the convolution.
-   ∂L/∂Y is the gradient **GradOutput** of the convolution forward loss function with respect to the output Y, which serves as the input for solving the backpropagation error ∂L/∂W, that is, the backward **GradOutput** of the convolution output.
-   ∂L/∂W is the backpropagation error **GradWeight** of the **Weight**.

**Figure 1**  Convolution backpropagation weight propagation<a name="fig1710410547569"></a>  
![](../../../../figures/convolution_backpropagation_weight_propagation.png "Convolution backpropagation weight propagation")

**Figure 2**  Convolution backpropagation weight computation process<a name="fig25291917533"></a>  
![](../../../../figures/convolution_backpropagation_weight_computation_process.png "Convolution backpropagation weight computation process")

The steps for implementing Conv3DBackpropFilter on the Kernel side to solve the backpropagation error computation are summarized as follows:

1.  Create a Conv3DBackpropFilter object.
2.  Perform initialization.
3.  Set the convolution feature matrix input and the convolution output gradient GradOutput.
4.  Complete the convolution backpropagation operation.
5.  End the convolution backpropagation operation.

The specific steps for using the Conv3DBackpropFilter high-level API to solve the backpropagation error computation are as follows:

1.  Create a Conv3DBackpropFilter object.

    ```
    #include "lib/conv_backprop/conv3d_bp_filter_api.h"

    using inputType = ConvBackpropApi::ConvType <ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, inputType>;
    using weightSizeType = ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::ND, int32_t>;
    using gradOutputType = ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, gradOutputType>;
    using gradWeightType = ConvBackpropApi::ConvType <ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, gradWeightType>;
    ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType> gradWeight_;
    ```

    When creating the object, you need to pass the shape information WeightSize of the feature matrix input and the weight matrix Weight, as well as the parameter type information of GradOutput and GradWeight. The type information is defined by [ConvType](#table19081115275), including the memory logical location, data format, and data type.

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename T>
    struct ConvType {
        constexpr static TPosition pos = POSITION;    // Logical position of the convolution input or output.
        constexpr static ConvFormat format = FORMAT;  // Data format of the convolution input or output.
        using Type = T;                               // Data type of the convolution input or output.
    };
    ```

    The following briefly describes the related data structures used when creating an object. You can optionally review this information. The data structures used to create a **Conv3DBackpropFilter** object are defined as follows:

    ```
    using Conv3DBackpropFilter = Conv3DBpFilterIntf<Conv3DBpFilterCfg<INPUT_TYPE, WEIGHT_TYPE, GRAD_OUTPUT_TYPE, GRAD_WEIGHT_TYPE>, Conv3DBpFilterImpl>;
    ```

    The **Conv3DBpFilterIntf** and **Conv3DBpFilterCfg** data structures are defined as follows:

    ```
    template <class Config_, template <typename, class> class Impl>
    struct Conv3DBpFilterIntf {
    }
    ```

    ```
    template <class A, class B, class C, class D>
    struct Conv3DBpFilterCfg : public ConvBpContext<A, B, C, D>{
    }
    ```

    **Table 1**  ConvType description

    <a name="table19081115275"></a>
    | Parameter | Description |
    | --- | --- |
    | POSITION | Memory logical location.<br>    The input X matrix can be set to **TPosition::GM**.<br>The WeightSize can be set to **TPosition::GM**.<br>The GradOutput matrix can be set to **TPosition::GM**.<br>The GradWeight matrix can be set to **TPosition::GM**. |
    | ConvFormat | Data format.<br>    The input matrix can be set to **ConvFormat::NDC1HWC0**.<br>The WeightSize matrix can be set to **ConvFormat::ND**.<br>The GradOutput matrix can be set to **ConvFormat::NDC1HWC0**.<br>The GradWeight matrix can be set to **ConvFormat::FRACTAL_Z_3D**. |
    | TYPE | Data type.<br>The input matrix can be set to **half** or **bfloat16_t**.<br>**WeightSize** can be set to **int32_t**.<br>The **GradOutput** matrix can be set to **half** or **bfloat16_tGrad**.<br>The Weight matrix can be set to **float**.<br><br>Note: The data types of **Input** and **GradOutput** must be consistent. For the specific data type combination relationships, see Table 2. |

    **Table 2**  Combination description of the input and output data types of Conv3DBackpropFilter

    | Input | WeightSize | GradOutput | GradWeight | Supported Platforms |
    | --- | --- | --- | --- | --- |
    | half | int32_t | half | float | Atlas A3 training products/Atlas A3 inference products<br>Atlas A2 training products/Atlas A2 inference products |
    | bfloat16_t | int32_t | bfloat16_t | float | Atlas A3 training products/Atlas A3 inference products<br>Atlas A2 training products/Atlas A2 inference products |

2.  Perform initialization.

    ```
    gradWeight_.Init(&(tilingData->dwTiling)); // Initialize parameters related to gradWeight_.
    ```

3.  Set the feature matrix input of the convolution and the backward output GradOutput of the convolution.

    ```
    gradWeight_.SetGradOutput(gradOutputGm_[offsetA_]);    // Set the gradOutput matrix.
    gradWeight_.SetInput(inputGm_[offsetB_]);    // Set the input matrix.
    gradWeight_.SetSingleShape(singleShapeM, singleShapeN, singleShapeK); // Set the shape to be computed.
    gradWeight_.SetStartPosition(hoStartIdx_); // Set the start position.
    ```

4.  Complete the convolution backpropagation operation.

    Call [Iterate](Iterate-125.md) to complete a single iteration, and superimpose a while loop to complete the computation of the full data volume on a single core. With the Iterate method, you can control the number of iterations to compute the required data volume.

    ```
    while (gradWeight_.Iterate()) {
        gradWeight_.GetTensorC(gradWeightGm_[offsetC_]);
    }
    ```

5.  End the convolution backpropagation operation.

    ```
    gradWeight_.End();
    ```

## Required Header Files

```
#include "lib/conv_backprop/conv3d_bp_filter_api.h"
```
