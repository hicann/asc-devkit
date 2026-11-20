# Template Parameter Definition<a name="ZH-CN_TOPIC_0000002139071917"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-13T10:02:44.731Z pushedAt=2026-11-16T07:07:41.693Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

The following function prototypes are used to define template parameters ASCENDC\_TPL\_ARGS\_DECL and template parameter combinations ASCENDC\_TPL\_ARGS\_SEL (that is, the templates that can be used). For details, see [Tiling Template Programming](Tiling_template_programming.md).

## Function Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
// ParamStruct is a structure that stores the template argument declaration ASCENDC_TPL_ARGS_DECL and the template argument selection ASCENDC_TPL_ARGS_SEL set by the user. It is used for encoding and decoding between the subsequent Tilingkey and template arguments, and users do not need to pay attention to it.
struct ParamStruct {
    const char* name;
    uint32_t paramType;
    uint8_t bitWidth;
    std::vector<uint64_t> vals;
    const char* macroType;
    ParamStruct(const char* inName, uint32_t inParamType, uint8_t inBitWidth, std::vector<uint64_t> inVals,
        const char* inMacroType):
        name(inName), paramType(inParamType), bitWidth(inBitWidth), vals(std::move(inVals)),
        macroType(inMacroType) {}
};
using TilingDeclareParams = std::vector<ParamStruct>;
using TilingSelectParams = std::vector<std::vector<ParamStruct>>;

// APIs related to template argument definition
#define ASCENDC_TPL_DTYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_DATATYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_FORMAT_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_FORMAT, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_UINT_DECL(x, bw, ...) ParamStruct{#x, ASCENDC_TPL_UINT, bw, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_BOOL_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_BOOL, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_KERNEL_TYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_SHARED_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}

#define ASCENDC_TPL_DTYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_DATATYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_FORMAT_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_FORMAT, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_UINT_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_UINT, 0, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_BOOL_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_BOOL, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_TILING_STRUCT_SEL(x, ...)
#define ASCENDC_TPL_KERNEL_TYPE_SEL(...) ParamStruct{"kernel_type", ASCENDC_TPL_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_DETERMINISTIC_SEL(...) ParamStruct{"deterministic", ASCENDC_TPL_DETERMINISTIC, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_SHARED_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}

#define ASCENDC_TPL_ARGS_DECL(x, ...) static TilingDeclareParams g_tilingDeclareParams{ __VA_ARGS__ }
#define ASCENDC_TPL_ARGS_SEL(...) { __VA_ARGS__}
#define ASCENDC_TPL_SEL(...) static TilingSelectParams g_tilingSelectParams{ __VA_ARGS__ }
```

## Parameters<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section75395119104"></a>

**Table 1**  Tiling template parameter definition

| Macro | Description | Parameter Explanation |
|---|---|---|
| ASCENDC_TPL_ARGS_DECL(args0, ...) | Used to define the template arguments of the operator. | args0: Indicates the Optype of the operator.<br>args1-argsn: The subsequent items are template argument declarations of several DTYPE, FORMAT, UINT, BOOL, and KERNEL_TYPE types, which are defined through ASCENDC_TPL_DTYPE_DECL, ASCENDC_TPL_DATATYPE_DECL, ASCENDC_TPL_FORMAT_DECL, ASCENDC_TPL_UINT_DECL, ASCENDC_TPL_BOOL_DECL, and ASCENDC_TPL_KERNEL_TYPE_DECL respectively. |
| ASCENDC_TPL_DTYPE_DECL(args0, ...) | Template argument declaration of the custom DataType type. | args0: Parameter name.<br>args1-argsn: The subsequent parameters are the exhaustively enumerated custom DataType enum values. |
| ASCENDC_TPL_DATATYPE_DECL(args0, ...) | Template argument declaration of the native DataType type. | args0: Parameter name.<br>args1-argsn: There are two cases. The subsequent parameters are the exhaustively enumerated native DataType options; or they are the index value of the corresponding input parameter (specified using ASCENDC_TPL_INPUT(x), where x is the corresponding value) or the index value of the corresponding output parameter (specified using ASCENDC_TPL_OUTPUT(x), where x is the corresponding value). Note: When multiple values exist, only the first one takes effect.<br>The native DataType values supported for setting are as follows. |
| ASCENDC_TPL_FORMAT_DECL(args0, ...) | Supports two modes:<br>1. Template argument declaration of the custom Format type.<br>2. Template argument declaration of the native Format type. | args0: Parameter name.<br>args1-argsn: There are two modes:<br>1. The subsequent parameters are the exhaustively enumerated custom Format enum values.<br>2. This mode has two cases: the subsequent parameters are the exhaustively enumerated native Format options; or they are the index value of the corresponding input parameter (specified using ASCENDC_TPL_INPUT(x), where x is the corresponding value) or the index value of the corresponding output parameter (specified using ASCENDC_TPL_OUTPUT(x), where x is the corresponding value). Note: When multiple values exist, only the first one takes effect.<br>The native Format options supported for setting are as follows. |
| ASCENDC_TPL_UINT_DECL(args0, args1, args2, ...) | Template argument declaration of the custom UINT type (unsigned integer). | args0: Parameter name.<br>args1: Maximum bit width. The number of template arguments cannot exceed the maximum bit width.<br>args2: Parameter definition mode. The following three modes are supported:<br>ASCENDC_TPL_UI_RANGE: Range mode. When this mode is set, the first value that follows indicates the number of ranges, and every two values after the first value form a group that respectively indicates the start and end positions of the range. Note that the number of defined ranges must remain consistent with the number of subsequent groups. **Example:** ASCENDC_TPL_UINT_DECL(args0, args1, ASCENDC_TPL_UI_RANGE, 2, 0, 2, 3, 5) indicates 2 groups of parameters, with the ranges of these 2 groups being {0, 2} and {3, 5}. Therefore, the valid values of the UINT parameter defined by this parameter are {0, 1, 2, 3, 4, 5}.<br>ASCENDC_TPL_UI_LIST: Exhaustive mode. When this mode is set, all parameter values are exhaustively enumerated subsequently. **Example:** ASCENDC_TPL_UINT_DECL(args0, args1, ASCENDC_TPL_UI_LIST, 10, 12, 13, 9, 8, 7, 6) indicates 1 group of exhaustively enumerated parameters, with [10, 12, 13, 9, 8, 7, 6] being the enumerated values. Therefore, the valid values of the UINT parameter defined by this parameter are {10, 12, 13, 9, 8, 7, 6}.<br>ASCENDC_TPL_UI_MIX: Mixed mode. When this mode is set, the first n values are the parameter definitions in range mode, and the last m values are the parameter definitions in exhaustive mode. **Example:** ASCENDC_TPL_UINT_DECL(args0, args1, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8) indicates 2 groups of range parameters, with the ranges of these 2 groups being {0, 2} and {3, 5}, and [10, 12, 13, 9, 8] being the enumerated values. Therefore, the valid values of the UINT parameter defined by this parameter are {0, 1, 2, 3, 4, 5, 10, 12, 13, 9, 8}.<br>args3-argsn: Parameter values corresponding to different range modes. |
| ASCENDC_TPL_BOOL_DECL(args0, ...) | Template argument declaration of the custom bool type. | args0: Parameter name.<br>args1-args2: Value range 0, 1. |
| ASCENDC_TPL_KERNEL_TYPE_DECL(args0, ...) | Defines the kernel function (Kernel) type of the operator template argument. | args0: Parameter name.<br>args1-argsn: The subsequent items are multiple kernel function (Kernel) types.<br>The currently supported kernel function (Kernel) types are as follows:<br>ASCENDC_TPL_AIV_ONLY // During operator execution, only the Vector core on the AI Core is started.<br>ASCENDC_TPL_AIC_ONLY // During operator execution, only the Cube core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIV_1_0 // In mixed AIC and AIV scenarios, during operator execution only the Vector core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIC_1_0 // In mixed AIC and AIV scenarios, during operator execution only the Cube core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIC_1_1 // In mixed AIC and AIV scenarios, during operator execution both the Cube core and the Vector core on the AI Core are started simultaneously, with a ratio of 1:1.<br>ASCENDC_TPL_MIX_AIC_1_2 // In mixed AIC and AIV scenarios, during operator execution both the Cube core and the Vector core on the AI Core are started simultaneously, with a ratio of 1:2.<br>ASCENDC_TPL_AICORE // During operator execution, only the AI Core is started.<br>ASCENDC_TPL_VECTORCORE // This parameter is a reserved parameter and is not supported yet in the current version.<br>ASCENDC_TPL_MIX_AICORE // This parameter is a reserved parameter and is not supported yet in the current version.<br>ASCENDC_TPL_MIX_VECTOR_CORE // During operator execution, both the AI Core and the Vector Core are started simultaneously.<br>This API can only be used together with ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(args0, ...). |

The native DataType values supported for setting by ASCENDC_TPL_DATATYPE_DECL are as follows:

```
C_DT_FLOAT
C_DT_FLOAT16
C_DT_INT8
C_DT_INT32
C_DT_UINT8
C_DT_INT16
C_DT_UINT16
C_DT_UINT32
C_DT_INT64
C_DT_UINT64
C_DT_DOUBLE
C_DT_BOOL
C_DT_COMPLEX64
C_DT_BF16
C_DT_INT4
C_DT_UINT1
C_DT_INT2
C_DT_COMPLEX32
C_DT_HIFLOAT8
C_DT_FLOAT8_E5M2
C_DT_FLOAT8_E4M3FN
C_DT_FLOAT4_E2M1
C_DT_FLOAT4_E1M2
```

The native Format options supported by ASCENDC_TPL_FORMAT_DECL are as follows:

```
C_FORMAT_NCHW
C_FORMAT_NHWC
C_FORMAT_ND
C_FORMAT_NC1HWC0
C_FORMAT_FRACTAL_Z
C_FORMAT_NC1C0HWPAD
C_FORMAT_NHWC1C0
C_FORMAT_FSR_NCHW
C_FORMAT_FRACTAL_DECONV
C_FORMAT_C1HWNC0
C_FORMAT_FRACTAL_DECONV_TRANSPOSE
C_FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS
C_FORMAT_NC1HWC0_C04
C_FORMAT_FRACTAL_Z_C04
C_FORMAT_CHWN
C_FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS
C_FORMAT_HWCN
C_FORMAT_NC1KHKWHWC0
C_FORMAT_BN_WEIGHT
C_FORMAT_FILTER_HWCK
C_FORMAT_HASHTABLE_LOOKUP_LOOKUPS
C_FORMAT_HASHTABLE_LOOKUP_KEYS
C_FORMAT_HASHTABLE_LOOKUP_VALUE
C_FORMAT_HASHTABLE_LOOKUP_OUTPUT
C_FORMAT_HASHTABLE_LOOKUP_HITS
C_FORMAT_C1HWNCoC0
C_FORMAT_MD
C_FORMAT_NDHWC
C_FORMAT_FRACTAL_ZZ
C_FORMAT_FRACTAL_NZ
C_FORMAT_NCDHW
C_FORMAT_DHWCN
C_FORMAT_NDC1HWC0
C_FORMAT_FRACTAL_Z_3D
C_FORMAT_CN
C_FORMAT_NC
C_FORMAT_DHWNC
C_FORMAT_FRACTAL_Z_3D_TRANSPOSE
C_FORMAT_FRACTAL_ZN_LSTM
C_FORMAT_FRACTAL_Z_G
C_FORMAT_RESERVED
C_FORMAT_ALL
C_FORMAT_NULL
C_FORMAT_ND_RNN_BIAS
C_FORMAT_FRACTAL_ZN_RNN
C_FORMAT_NYUV
C_FORMAT_NYUV_A
C_FORMAT_NCL
C_FORMAT_FRACTAL_Z_WINO
C_FORMAT_C1HWC0
C_FORMAT_FRACTAL_NZ_C0_16
C_FORMAT_FRACTAL_NZ_C0_32
C_FORMAT_FRACTAL_NZ_C0_2
C_FORMAT_FRACTAL_NZ_C0_4
C_FORMAT_FRACTAL_NZ_C0_8
```

**Table 2**  Tiling template parameter combination definition

| Macro | Description | Parameter Explanation |
|---|---|---|
| ASCENDC_TPL_SEL(...) | Overall combination of the template arguments of the operator. | Contains the template argument selection of multiple operators. |
| ASCENDC_TPL_ARGS_SEL(...) | Template argument selection of the operator. | Template argument selection of one operator. |
| ASCENDC_TPL_KERNEL_TYPE_SEL(args0) | Used to set the kernel function (Kernel) type of the operator template argument selection, but this parameter cannot be passed in as a template argument of the kernel function (Kernel). | args0: The kernel function (Kernel) type of the operator under this template argument selection. If not selected, the automatic derivation process is used. For all operators under ASCENDC_TPL_SEL, whether to select the kernel function (Kernel) type must remain consistent.<br>The currently supported kernel function (Kernel) types are as follows:<br>ASCENDC_TPL_AIV_ONLY // During operator execution, only the Vector core on the AI Core is started.<br>ASCENDC_TPL_AIC_ONLY // During operator execution, only the Cube core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIV_1_0 // In mixed AIC and AIV scenarios, during operator execution only the Vector core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIC_1_0 // In mixed AIC and AIV scenarios, during operator execution only the Cube core on the AI Core is started.<br>ASCENDC_TPL_MIX_AIC_1_1 // In mixed AIC and AIV scenarios, during operator execution both the Cube core and the Vector core on the AI Core are started simultaneously, with a ratio of 1:1.<br>ASCENDC_TPL_MIX_AIC_1_2 // In mixed AIC and AIV scenarios, during operator execution both the Cube core and the Vector core on the AI Core are started simultaneously, with a ratio of 1:2.<br>ASCENDC_TPL_AICORE // During operator execution, only the AI Core is started.<br>ASCENDC_TPL_VECTORCORE // This parameter is a reserved parameter and is not supported yet in the current version.<br>ASCENDC_TPL_MIX_AICORE // This parameter is a reserved parameter and is not supported yet in the current version.<br>ASCENDC_TPL_MIX_VECTOR_CORE // During operator execution, both the AI Core and the Vector Core are started simultaneously.<br>The kernel function (Kernel) type is configured through this API. The value range of the kernel function (Kernel) type is consistent with the KERNEL_TASK_TYPE_DEFAULT API. For details, see [Setting the Kernel Function (Kernel) Type](../../SIMD-API/basic_api/tool_interface/execution_mode/set_Kernel_type.md). |
| ASCENDC_TPL_DTYPE_SEL(args0, ...) | Template argument selection of the custom DataType type. | args0: Indicates the parameter name.<br>args1-argsn: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_DTYPE_DECL. |
| ASCENDC_TPL_DATATYPE_SEL(args0, ...) | Template argument selection of the native DataType type. | args0: Indicates the parameter name.<br>args1-argsn: The subsequent parameters are a subset of the parameter option range defined in ASCENDC_TPL_DATATYPE_DECL. |
| ASCENDC_TPL_FORMAT_SEL(args0, ...) | Template argument selection of the Format type. | args0: Indicates the parameter name.<br>args1-argsn: The subsequent parameters are a subset of the parameter option range defined in ASCENDC_TPL_FORMAT_DECL. |
| ASCENDC_TPL_UINT_SEL(args0, args1, args2, ...) | Template argument selection of the UINT type. | args0: Indicates the parameter name.<br>args1: Parameter definition mode. The following values are supported:<br>ASCENDC_TPL_UI_RANGE: Range mode.<br>ASCENDC_TPL_UI_LIST: Exhaustive mode.<br>ASCENDC_TPL_UI_MIX: Mixed mode.<br>args2-argsn: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_UINT_DECL.<br>For the configuration of modes and parameters, refer to ASCENDC_TPL_UINT_DECL(args0, args1, args2, ...). |
| ASCENDC_TPL_BOOL_SEL(args0, ...) | Template argument selection of the bool type. | args0: Indicates the parameter name.<br>args1-args2: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_BOOL_DECL. |
| ASCENDC_TPL_DETERMINISTIC_SEL(args0) | This group of template argument selection is used to configure whether to enable deterministic computation. | args0: Indicates the parameter name. The optional value range is [true, false, 1, 0], where [true/1] indicates that this group of template argument selection enables deterministic computation, and [false/0] indicates that deterministic computation is not enabled. Note that this value is not used as a template argument input parameter of the operator. When this value is enabled during compilation, "-DDETERMINISTIC_MODE=1" is added, and json and .o files ending with "_deterministic" are generated, for example: "AddCustomTemplate_816f04e052850554f4b3cacb35f8e8c6_deterministic.json"/"AddCustomTemplate_816f04e052850554f4b3cacb35f8e8c6_deterministic.o".<br>Note: If a deterministic computation version is compiled through the ASCENDC_TPL_DETERMINISTIC_SEL(true) API, the deterministic computation switch usually needs to be enabled when the operator is called. For example, when calling through the aclnn single-operator API, the aclrtCtxSetSysParamOpt API needs to be used for related configuration.<br>This parameter supports only the following models:<br>Atlas A3 products<br>Atlas A2 products |
| ASCENDC_TPL_TILING_STRUCT_SEL(args0) | Used to specify the Tiling structure for the current template argument selection. This structure is not passed in as a template argument of the kernel function (Kernel). | args0: Indicates the name of the Tiling structure used by the current template argument selection. This API can only be used in ASCENDC_TPL_ARGS_SEL(...). It does not participate in the parameter order of ASCENDC_TPL_SEL_PARAM(...) or GET_TPL_TILING_KEY(...), nor does it participate in TilingKey encoding.<br>For a template argument selection where this API is not configured, the default Tiling structure registered by REGISTER_TILING_DEFAULT is used. When this API is used, a default Tiling structure must be provided through REGISTER_TILING_DEFAULT. |
| ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(args0, ...) | Sets the kernel function (Kernel) type of the operator template argument selection. This parameter can be passed in as a template argument of the kernel function (Kernel). | args0: Parameter name.<br>args1-argsn: The kernel function (Kernel) type of the operator under this template argument selection. The subsequent parameters are several kernel function (Kernel) types. This API cannot be used together with the ASCENDC_TPL_KERNEL_TYPE_SEL API.<br>If the KERNEL_TASK_TYPE_DEFAULT(value) API is also used, this API has a higher priority. |

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

After modifying or adding values for the template parameter definitions, you must recompile the custom operator package. The previously generated operator binary can no longer be used.
