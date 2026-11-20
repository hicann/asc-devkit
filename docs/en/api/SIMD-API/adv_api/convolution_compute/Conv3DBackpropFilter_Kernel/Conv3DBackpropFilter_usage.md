# Conv3DBackpropFilter Usage Guide

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-10T06:24:37.210Z pushedAt=2026-11-18T11:07:26.581Z nl=2 -->

Ascend C provides a set of high-level Conv3DBackpropFilter APIs to help you quickly implement the backward operation of convolution and solve the backpropagation error.

The weight propagation for convolution backward is shown in [Figure 1](#fig1710410547569), and the weight computation for convolution backward is shown in [Figure 2](#fig25291917533).

The computation formula of Conv3dBackpropFilter is as follows:

![](../../../../figures/formulaimage_0000002067618970.png)

-   X is the feature matrix Input of the convolution.
-   ∂L/∂Y is the gradient GradOutput of the convolution forward loss function with respect to the output Y, which serves as the input for solving the backpropagation error ∂L/∂W, that is, the output backward GradOutput of the convolution.
-   ∂L/∂W is the backpropagation error GradWeight of the Weight.

**Figure 1**  Convolution backward weight propagation<a name="fig1710410547569"></a>  
![](../../../../figures/conv_backward_weight_propagation.png "Convolution backward weight propagation")

**Figure 2**  Convolution backward weight computation process<a name="fig25291917533"></a>  
![](../../../../figures/conv_backward_weight_compute.png "Convolution backward weight computation process")

The steps for implementing the Conv3DBackpropFilter backpropagation error computation on the kernel side are summarized as follows:

1.  Create a Conv3DBackpropFilter object.
2.  Initialize the operation.
3.  Set the convolution feature matrix Input and the convolution output backward GradOutput.
4.  Complete the convolution backward operation.
5.  End the convolution backward operation.

The specific steps for using the Conv3DBackpropFilter high-level API to compute the backpropagation error are as follows:

1.  Create a Conv3DBackpropFilter object.

    ```
    #include "lib/conv_backprop/conv3d_bp_filter_api.h"

    using inputType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, inputType>;
    using weightSizeType = ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::ND, int32_t>;
    using gradOutputType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, gradOutputType>;
    using gradWeightType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, gradWeightType>;
    ConvBackpropApi::Conv3DBackpropFilter<inputType, weightSizeType, gradOutputType, gradWeightType> gradWeight_;
    ```

    When creating the object, you need to  the shape information WeightSize of the feature matrix Input and the weight matrix Weight, as well as the parameter type information of GradOutput and GradWeight. The type information is defined by [ConvType](#table19081115275), including the memory logical position, data format, and data type.

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename T>
    struct ConvType {
        constexpr static TPosition pos = POSITION;   // Logical position of the Convolution input or output.
        constexpr static ConvFormat format = FORMAT; // Data format of the Convolution input or output.
        using Type = T;                              // Data type of the Convolution input or output.
    };
    ```

    The following briefly describes the data structures used when creating an object. You may read this section as needed. The data structures used to create a Conv3DBackpropFilter object are defined as follows:

    ```
    using Conv3DBackpropFilter = Conv3DBpFilterIntf<
        Conv3DBpFilterCfg<INPUT_TYPE, WEIGHT_TYPE, GRAD_OUTPUT_TYPE, GRAD_WEIGHT_TYPE>, Conv3DBpFilterImpl>;
    ```

    The Conv3DBpFilterIntf and Conv3DBpFilterCfg data structures are defined as follows:

    ```
    template <class Config_, template <typename, class> class Impl>
    struct Conv3DBpFilterIntf {}
    ```

    ```
    template <class A, class B, class C, class D>
    struct Conv3DBpFilterCfg : public ConvBpContext<A, B, C, D> {}
    ```

    **Table 1**  ConvType description

    <a name="table19081115275"></a>

    | Parameter | Description |
    | --- | --- |
    | POSITION | Logical memory position.<br>    The Input X matrix can be set to TPosition::GM.<br>WeightSize can be set to TPosition::GM.<br>The GradOutput matrix can be set to TPosition::GM.<br>The GradWeight matrix can be set to TPosition::GM. |
    | ConvFormat | Data format.<br>    The Input matrix can be set to ConvFormat::NDC1HWC0.<br>The WeightSize matrix can be set to ConvFormat::ND.<br>The GradOutput matrix can be set to ConvFormat::NDC1HWC0.<br>The GradWeight matrix can be set to ConvFormat::FRACTAL_Z_3D. |
    | TYPE | Data type.<br>The Input matrix can be set to half or bfloat16_t.<br>WeightSize can be set to int32_t.<br>The GradOutput matrix can be set to half or bfloat16_tGrad.<br>The Weight matrix can be set to float.<!-- npu="A3,910b" id6 --><br><br>Note: The Input and GradOutput data types must be consistent. See Table 2 for specific data type combinations.<!-- end id6 --> |

    <!-- npu="A3,910b" id5 -->
    **Table 2**  Data type combinations for Conv3DBackpropFilter input and output

    | Input | WeightSize | GradOutput | GradWeight | Supported Platform |
    | --- | --- | --- | --- | --- |
    | half | int32_t | half | float | <!-- npu="A3" id1 -->Atlas A3 products<br><!-- end id1 --><!-- npu="910b" id2 -->Atlas A2 products<!-- end id2 --> |
    | bfloat16_t | int32_t | bfloat16_t | float | <!-- npu="A3" id3 -->Atlas A3 products<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 products<!-- end id4 --> |

    <!-- end id5 -->

2.  Initialize the operation.

    ```
    gradWeight_.Init(&(tilingData->dwTiling)); // Initialize gradWeight_-related parameters.
    ```

3.  Set the convolution feature matrix Input and the convolution output backward GradOutput.

    ```
    gradWeight_.SetGradOutput(gradOutputGm_[offsetA_]);    // Set the gradOutput matrix.
    gradWeight_.SetInput(inputGm_[offsetB_]);    // Set the Input matrix.
    gradWeight_.SetSingleShape(singleShapeM, singleShapeN, singleShapeK); // Set the shape to be computed.
    gradWeight_.SetStartPosition(hoStartIdx_); // Set the start position.
    ```

4.  Complete the convolution backward operation.

    Call [Iterate](Iterate.md) to complete a single iteration, and use a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations to compute the required amount of data.

    ```
    while (gradWeight_.Iterate()) {
        gradWeight_.GetTensorC(gradWeightGm_[offsetC_]);
    }
    ```

5.  End the convolution backward operation.

    ```
    gradWeight_.End();
    ```

## Header Files to Include

```
#include "lib/conv_backprop/conv3d_bp_filter_api.h"
```
