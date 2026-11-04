# Naming Conversion Rule Reference Table

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:35:35.181Z pushedAt=2026-11-03T07:05:13.395Z nl=2 -->

This document covers basic content and introduces the naming conversion rules in custom operator projects. The operator type (OpType) usually uses UpperCamelCase naming, while the implementation file name and kernel function entry name in the configuration usually use lowercase underscore naming. By default, the framework generates these names based on the OpType.

## Conversion Rule Overview

Conversion occurs during the build phase. The build tool generates the configuration and related code based on the registered OpType. The operator runtime uses the generated configuration and symbols and does not convert names again.

The build tool processes characters from left to right according to the following rules:

1. **Convert the first uppercase character to lowercase**: The first character is directly converted to lowercase without inserting an underscore.
2. **Insert an underscore when the previous character is not an uppercase letter**: When the current character is an uppercase letter and the previous character is a lowercase letter or a digit, insert an underscore before the current character and convert the current character to lowercase.
3. **Insert an underscore between consecutive uppercase letters and the following word**: When both the current character and the previous character are uppercase letters but the next character is a lowercase letter, insert an underscore before the current character and convert the current character to lowercase.
4. **Convert or retain other characters directly**: Other uppercase letters are directly converted to lowercase; lowercase letters and digits remain unchanged.

The build tool also checks whether the OpType complies with the recommended naming convention: the first character must be an uppercase letter, and subsequent characters can only be letters or digits. If the OpType does not comply, a warning is output, suggesting the use of UpperCamelCase naming.

## Conversion Table

The following table lists the conversion results of common operator names. The actual conversion results of the current build tool prevail.

| Operator Type (UpperCamelCase) | File Name/Kernel Function Entry Name (lowercase underscore) | Conversion Description |
|---|---|---|
| `AddCustom` | `add_custom` | The first character is converted to lowercase, and an underscore is inserted before `Custom`. |
| `ReduceMaxCustom` | `reduce_max_custom` | Underscores are inserted before `Max` and `Custom`, respectively. |
| `MatmulCustom` | `matmul_custom` | An underscore is inserted before `Custom`. |
| `BatchNormCustom` | `batch_norm_custom` | Underscores are inserted before `Norm` and `Custom`, respectively. |
| `Conv2DCustom` | `conv2_d_custom` | `D` is preceded by the digit `2`, so an underscore is inserted before `D`. |
| `SoftmaxCustom` | `softmax_custom` | An underscore is inserted before `Custom`. |
| `ReluCustom` | `relu_custom` | An underscore is inserted before `Custom`. |
| `LayerNormCustom` | `layer_norm_custom` | Underscores are inserted before `Norm` and `Custom`, respectively. |
| `SliceCustom` | `slice_custom` | An underscore is inserted before `Custom`. |
| `GatherV2Custom` | `gather_v2_custom` | `V` is preceded by a lowercase letter, and `Custom` is preceded by the digit `2`. |
| `SplitVCustom` | `split_v_custom` | Underscores are inserted before `V` and `Custom`, respectively. |
| `TransposeCustom` | `transpose_custom` | An underscore is inserted before `Custom`. |
| `OneHotCustom` | `one_hot_custom` | Underscores are inserted before `Hot` and `Custom`, respectively. |
| `TopKCustom` | `top_k_custom` | Underscores are inserted before `K` and `Custom`, respectively. |

## Special Cases

### Consecutive Uppercase Letters

Consecutive uppercase letters usually represent an abbreviation. The conversion tool does not insert an underscore before each letter of the abbreviation; instead, it inserts an underscore at the boundary between the abbreviation and the following word.

- `AbcAAc` is converted to `abc_a_ac`: the first `A` is preceded by the lowercase letter `c`, and the second `A` is followed by the lowercase letter `c`, so an underscore is inserted at both positions.
- `GRUCell` is converted to `gru_cell`: `C` is preceded by the uppercase letter `U` and followed by the lowercase letter `e`, so an underscore is inserted before `C`.
- `HTTPServerCustom` is converted to `http_server_custom`: underscores are inserted at the word boundaries of `Server` and `Custom`.

### Adjacent Numbers and Letters

Numbers themselves remain unchanged. If the character following a number is an uppercase letter, rule 2 is triggered because a number is not an uppercase letter.

```text
GatherV2Custom -> gather_v2_custom
Conv2DCustom   -> conv2_d_custom
```

Therefore, you cannot write `Conv2D` as `conv2d` merely based on common abbreviation conventions; the actual conversion result of the current implementation is `conv2_d`.

### Initial Capital Letter

The first letter is directly converted to lowercase without inserting an underscore.

```text
Add    -> add
Reduce -> reduce
```

## Actual Impact on the Project

By default, developers need to ensure the following correspondences:

| Item | Naming Convention | `AddCustom` Example |
|---|---|---|
| Operator type, `OpDef` class name, constructor name, `OP_ADD` parameter | UpperCamelCase | `AddCustom` |
| Kernel function source file name | Lowercase underscore recommended, can be explicitly specified by CMake | `add_custom.cpp` |
| Kernel function entry name | Lowercase underscore by default, must match `opInterface.value` | `add_custom` |

When creating an operator project with msOpGen, the tool typically generates the initial file names and kernel function entry names according to the rules above. After building, you can verify the actual conversion results through the generated `ops-info.ini` and aclnn header files, and applications should also follow these build artifacts.

If you modify the aclnn project name prefix or the direct-call API prefix, the prefixes of the generated files and APIs will also change. After renaming the Host or kernel function source files, you also need to update the source file list in CMake accordingly.

## Related Documents

- [Operator Prototype Definition](../design_and_implementation/operator_prototype_definition.md): Learn about OpType, the `OpDef` class, and the `OP_ADD` registration method.
- [Kernel-Side Operator Implementation](../design_and_implementation/kernel_operator_implementation.md): Learn about the kernel function entry format and parameter order.
- [Single Operator API Call](../invocation/single_operator_api_call.md): Learn about the two-stage aclnn API and its parameter naming rules.
