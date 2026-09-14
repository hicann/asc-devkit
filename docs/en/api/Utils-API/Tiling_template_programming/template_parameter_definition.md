# Template Parameter Definition<a name="ZH-CN_TOPIC_0000002139071917"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T16:20:52.017Z pushedAt=2026-09-11T07:15:47.130Z -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

The following function prototype is used to define template parameters ASCENDC\_TPL\_ARGS\_DECL and template parameter combinations ASCENDC\_TPL\_ARGS\_SEL (that is, the templates that can be used). For details, see [Tiling Template Programming](tiling_template_programming.md).

## Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
// ParamStruct is a structure that stores the user-defined template parameter ASCENDC_TPL_ARGS_DECL and template parameter combination ASCENDC_TPL_ARGS_SEL. It is used for encoding and decoding between the subsequent Tilingkey and template parameters, and users do not need to pay attention to it.
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

// APIs related to template parameter definition.
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

**Table 1** Tiling template parameter definition

<a name="table167201345111418"></a>
<table><thead align="left"><tr id="row172084511143"><th class="cellrowborder" valign="top" width="31.57315731573157%" id="mcps1.2.4.1.1"><p id="p772024521413"><a name="p772024521413"></a><a name="p772024521413"></a>Macro</p></th>
<th class="cellrowborder" valign="top" width="20.59205920592059%" id="mcps1.2.4.1.2"><p id="p97205455141"><a name="p97205455141"></a><a name="p97205455141"></a>Description</p></th>
<th class="cellrowborder" valign="top" width="47.83478347834784%" id="mcps1.2.4.1.3"><p id="p147201454140"><a name="p147201454140"></a><a name="p147201454140"></a>Parameter Description</p></th>
</tr>
</thead>
<tbody><tr id="row1720345141410"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p12720184531419"><a name="p12720184531419"></a><a name="p12720184531419"></a>ASCENDC_TPL_ARGS_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p0720154516147"><a name="p0720154516147"></a><a name="p0720154516147"></a>Defines the template parameters of an operator.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><a name="ul127205457146"></a><a name="ul127205457146"></a><ul id="ul127205457146"><li>args0: Indicates the operator Optype.</li><li>args1-argsn: The subsequent template parameter definitions of several DTYPE, FORMAT, UINT, BOOL, and KERNEL_TYPE types, which are defined through ASCENDC_TPL_DTYPE_DECL, ASCENDC_TPL_DATATYPE_DECL, ASCENDC_TPL_FORMAT_DECL, ASCENDC_TPL_UINT_DECL, ASCENDC_TPL_BOOL_DECL, and ASCENDC_TPL_KERNEL_TYPE_DECL, respectively.</li></ul></td>
</tr>
<tr id="row2720174531419"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p5721184561410"><a name="p5721184561410"></a><a name="p5721184561410"></a>ASCENDC_TPL_DTYPE_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p18721114510142"><a name="p18721114510142"></a><a name="p18721114510142"></a>Template parameter definition of the custom DataType type.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><a name="ul1972174581413"></a><a name="ul1972174581413"></a><ul id="ul1972174581413"><li>args0: Parameter name.</li><li>args1-argsn: The subsequent parameters are the exhaustive custom DataType enumeration values.</li></ul></td>
</tr>
<tr id="row566914112417"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p783034774115"><a name="p783034774115"></a><a name="p783034774115"></a>ASCENDC_TPL_DATATYPE_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p117171839134220"><a name="p117171839134220"></a><a name="p117171839134220"></a>Template parameter definition of the native DataType type.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><a name="ul142301058431"></a><a name="ul142301058431"></a><ul id="ul142301058431"><li>args0: Parameter name.</li><li>args1-argsn: There are two cases: the subsequent parameters are either the enumerated native DataType options, or the index value of the corresponding input parameter (specified by ASCENDC_TPL_INPUT(x), where x is the corresponding value) or the index value of the corresponding output parameter (specified by ASCENDC_TPL_OUTPUT(x), where x is the corresponding value). Note that if multiple parameters are specified, only the first one takes effect.</li><li>The supported native DataType values are as follows.<pre class="screen" id="screen429983452113"><a name="screen429983452113"></a><a name="screen429983452113"></a>C_DT_FLOAT
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
C_DT_FLOAT4_E1M2</pre>
</li></ul></td>
</tr>
<tr id="row177211245191419"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p117216458142"><a name="p117216458142"></a><a name="p117216458142"></a>ASCENDC_TPL_FORMAT_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p1468017015118"><a name="p1468017015118"></a><a name="p1468017015118"></a>Two modes are supported:</p>
<p id="p18721045121417"><a name="p18721045121417"></a><a name="p18721045121417"></a>1. Template parameter definition of the custom Format type.</p>
<p id="p97815368519"><a name="p97815368519"></a><a name="p97815368519"></a>2. Template parameter definition of the native Format type.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><a name="ul13721144581419"></a><a name="ul13721144581419"></a><ul id="ul13721144581419"><li>args0: Parameter name.</li><li>args1-argsn: There are two modes.<a name="ul38322038121613"></a><a name="ul38322038121613"></a><ul id="ul38322038121613"><li>1. The subsequent parameters are the exhaustive custom Format enumeration values.</li><li>2. This mode has two cases: the subsequent parameters are the exhaustive native Format options; or the index value of the corresponding input parameter (specified by ASCENDC_TPL_INPUT(x), where x is the corresponding value) or the index value of the corresponding output parameter (specified by ASCENDC_TPL_OUTPUT(x), where x is the corresponding value). Note: When multiple values exist, only the first one takes effect.</li></ul>
</li><li>The supported native Format options are as follows.<pre class="screen" id="screen1993917278205"><a name="screen1993917278205"></a><a name="screen1993917278205"></a>C_FORMAT_NCHW
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
C_FORMAT_FRACTAL_NZ_C0_8</pre>
</li></ul></td>
</tr>
<tr id="row2721134518149"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p2721194561419"><a name="p2721194561419"></a><a name="p2721194561419"></a>ASCENDC_TPL_UINT_DECL(args0, args1, args2, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p147211045141416"><a name="p147211045141416"></a><a name="p147211045141416"></a>Template parameter definition of the custom UINT type (unsigned integer).</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><a name="ul67211745111418"></a><a name="ul67211745111418"></a><ul id="ul67211745111418"><li>args0: Parameter name.</li><li>args1: Maximum bit width. The number of template parameters cannot exceed the maximum bit width.</li><li>args2: Mode of parameter definition. The following three modes are supported: <a name="ul12721114518144"></a><a name="ul12721114518144"></a><ul id="ul12721114518144"><li>ASCENDC_TPL_UI_RANGE: range mode. When this mode is set, the first value that follows indicates the number of ranges, and every two values after the first value form a group that indicates the start and end positions of the range. Note that the defined number of ranges must be consistent with the number of subsequent groups.<p id="p19321105417342"><a name="p19321105417342"></a><a name="p19321105417342"></a><strong id="b14721114521418"><a name="b14721114521418"></a><a name="b14721114521418"></a>Example:</strong>ASCENDC_TPL_UINT_DECL(args0, args1,ASCENDC_TPL_UI_RANGE,2,0,2,3,5) indicates 2 groups of parameters, with the ranges {0, 2} and {3, 5}. Therefore, the valid values of the UINT parameter defined by this parameter are {0, 1, 2, 3, 4, 5}.</p>
</li><li>ASCENDC_TPL_UI_LIST: exhaustive mode. When this mode is set, all parameter values are exhaustively listed.<p id="p3291227105412"><a name="p3291227105412"></a><a name="p3291227105412"></a><strong id="b127221345121417"><a name="b127221345121417"></a><a name="b127221345121417"></a>Example:</strong>ASCENDC_TPL_UINT_DECL(args0, args1,ASCENDC_TPL_UI_LIST,10,12,13,9,8,7,6) indicates 1 group of exhaustive parameters, with [10, 12, 13, 9, 8, 7, 6] as the exhaustive values. Therefore, the valid values of the UINT parameter defined by this parameter are {10, 12, 13, 9, 8, 7, 6}.</p>
</li><li>ASCENDC_TPL_UI_MIX: mixed mode. When this mode is set, the first n values are the parameter definition in range mode, and the last m values are the parameter definition in exhaustive mode.<p id="p1072284512145"><a name="p1072284512145"></a><a name="p1072284512145"></a><strong id="b2072234531415"><a name="b2072234531415"></a><a name="b2072234531415"></a>Example</strong>:</p>
<p id="p127221945111417"><a name="p127221945111417"></a><a name="p127221945111417"></a>ASCENDC_TPL_UINT_DECL(args0, args1,ASCENDC_TPL_UI_MIX,2,0,2,3, 5, 10, 12, 13, 9, 8) indicates 2 groups of exhaustive parameters, with the ranges {0, 2} and {3, 5}, and [10, 12, 13, 9, 8] as the exhaustive values. Therefore, the valid values of the UINT parameter defined by this parameter are {0, 1, 2, 3, 4, 5,  10, 12, 13, 9, 8}.</p>
</li></ul>
</li><li>args3-argsn: Parameter values corresponding to different range modes.</li></ul></td>
</tr>
<tr id="row197225458147"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p0722345131419"><a name="p0722345131419"></a><a name="p0722345131419"></a>ASCENDC_TPL_BOOL_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p3722545151411"><a name="p3722545151411"></a><a name="p3722545151411"></a>Defines a custom bool-type template parameter.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><p id="p872284541413"><a name="p872284541413"></a><a name="p872284541413"></a>args0: parameter name.</p>
<p id="p27221345121418"><a name="p27221345121418"></a><a name="p27221345121418"></a>args1-args2: Value range 0, 1.</p></td>
</tr>
<tr id="row17351183522212"><td class="cellrowborder" valign="top" width="31.57315731573157%" headers="mcps1.2.4.1.1 "><p id="p18352143518225"><a name="p18352143518225"></a><a name="p18352143518225"></a>ASCENDC_TPL_KERNEL_TYPE_DECL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="20.59205920592059%" headers="mcps1.2.4.1.2 "><p id="p1435293519223"><a name="p1435293519223"></a><a name="p1435293519223"></a>Defines the kernel type of the operator template parameter.</p></td>
<td class="cellrowborder" valign="top" width="47.83478347834784%" headers="mcps1.2.4.1.3 "><p id="p4637141672417"><a name="p4637141672417"></a><a name="p4637141672417"></a>args0: Parameter name.</p>
<p id="p06378169245"><a name="p06378169245"></a><a name="p06378169245"></a>args1-argsn: The following are several kernel types.</p>
<p id="p195465652615"><a name="p195465652615"></a><a name="p195465652615"></a>The currently supported kernel types are as follows:</p>
<a name="ul2054135610267"></a><a name="ul2054135610267"></a><ul id="ul2054135610267"><li>ASCENDC_TPL_AIV_ONLY // When the operator is executed, only the Vector core on the AI Core is started.</li><li>ASCENDC_TPL_AIC_ONLY // When the operator is executed, only the Cube core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIV_1_0 // In the AIC and AIV mixed scenario, when the operator is executed, only the Vector core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIC_1_0 // In the AIC and AIV mixed scenario, when the operator is executed, only the Cube core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIC_1_1 // In the AIC and AIV mixed scenario, when the operator is executed, both the Cube core and the Vector core on the AI Core are started at a ratio of 1:1.</li><li>ASCENDC_TPL_MIX_AIC_1_2 // In the AIC and AIV mixed scenario, when the operator is executed, both the Cube core and the Vector core on the AI Core are started at a ratio of 1:2.</li><li>ASCENDC_TPL_AICORE // When the operator is executed, only the AI Core is started.</li><li>ASCENDC_TPL_VECTORCORE // This parameter is a reserved parameter and is not supported in the current version.</li><li>ASCENDC_TPL_MIX_AICORE  //  This parameter is a reserved parameter and is not supported in the current version.</li><li>ASCENDC_TPL_MIX_VECTOR_CORE // When the operator is executed, both the AI Core and the Vector Core are started.</li></ul>
<p id="p176510543512"><a name="p176510543512"></a><a name="p176510543512"></a>This API can only be used together with ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(args0, ...).</p></td>
</tr>
</tbody>
</table>

**Table 2**  Tiling template parameter combination definition

<a name="table15722174518143"></a>
<table><thead align="left"><tr id="row1172244519146"><th class="cellrowborder" valign="top" width="23.75237523752375%" id="mcps1.2.4.1.1"><p id="p1372215455141"><a name="p1372215455141"></a><a name="p1372215455141"></a>Macro</p></th>
<th class="cellrowborder" valign="top" width="28.202820282028203%" id="mcps1.2.4.1.2"><p id="p972217453140"><a name="p972217453140"></a><a name="p972217453140"></a>Description</p></th>
<th class="cellrowborder" valign="top" width="48.04480448044804%" id="mcps1.2.4.1.3"><p id="p12722104513145"><a name="p12722104513145"></a><a name="p12722104513145"></a>Parameter</p></th>
</tr>
</thead>
<tbody><tr id="row9722194518143"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p117221445101413"><a name="p117221445101413"></a><a name="p117221445101413"></a>ASCENDC_TPL_SEL(...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p167231445161410"><a name="p167231445161410"></a><a name="p167231445161410"></a>Overall combination of the operator's template parameters.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p11723104571417"><a name="p11723104571417"></a><a name="p11723104571417"></a>Combination of template parameters for multiple operators.</p></td>
</tr>
<tr id="row13723945181410"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p1872354513149"><a name="p1872354513149"></a><a name="p1872354513149"></a>ASCENDC_TPL_ARGS_SEL(...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p072304514148"><a name="p072304514148"></a><a name="p072304514148"></a>Combination of the operator's template parameters.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p9723134510149"><a name="p9723134510149"></a><a name="p9723134510149"></a>Combination of template parameters for a single operator.</p></td>
</tr>
<tr id="row9443131565713"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p52843445011"><a name="p52843445011"></a><a name="p52843445011"></a>ASCENDC_TPL_KERNEL_TYPE_SEL(args0)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p17444815195718"><a name="p17444815195718"></a><a name="p17444815195718"></a>Used to set the Kernel type of the operator's template parameter combination, but this parameter cannot be passed as a template parameter of the kernel function.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p2651142418375"><a name="p2651142418375"></a><a name="p2651142418375"></a>args0: Kernel type of the operator under this template parameter combination. If not selected, the automatic derivation process is used. All operators under ASCENDC_TPL_SEL must be consistent in whether to select the Kernel type.</p>
<p id="p1617912310417"><a name="p1617912310417"></a><a name="p1617912310417"></a>The currently supported Kernel types are as follows:</p>
<a name="ul78891930111019"></a><a name="ul78891930111019"></a><ul id="ul78891930111019"><li>ASCENDC_TPL_AIV_ONLY // When the operator is executed, only the Vector core on the AI Core is started.</li><li>ASCENDC_TPL_AIC_ONLY // When the operator is executed, only the Cube core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIV_1_0 // In the AIC and AIV mixed scenario, when the operator is executed, only the Vector core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIC_1_0 // In the AIC and AIV mixed scenario, when the operator is executed, only the Cube core on the AI Core is started.</li><li>ASCENDC_TPL_MIX_AIC_1_1 // In the AIC and AIV mixed scenario, when the operator is executed, both the Cube core and the Vector core on the AI Core are started at a ratio of 1:1.</li><li>ASCENDC_TPL_MIX_AIC_1_2 // In the AIC and AIV mixed scenario, when the operator is executed, both the Cube core and the Vector core on the AI Core are started at a ratio of 1:2.</li><li>ASCENDC_TPL_AICORE // When the operator is executed, only the AI Core is started.</li><li>ASCENDC_TPL_VECTORCORE // This parameter is a reserved parameter and is not supported in the current version.</li><li>ASCENDC_TPL_MIX_AICORE  // This parameter is a reserved parameter and is not supported in the current version.</li><li>ASCENDC_TPL_MIX_VECTOR_CORE // When the operator is executed, both the AI Core and the Vector Core are started.<p id="p1316145282715"><a name="p1316145282715"></a><a name="p1316145282715"></a>When the Kernel type is configured through this API, the value range of the Kernel type is the same as that of the KERNEL_TASK_TYPE_DEFAULT API. For details, see <a href="../../SIMD-API/basic_api/Kernel-Tiling/set_kernel_type.md">Setting the Kernel Type</a>.</p>
</li></ul></td>
</tr>
<tr id="row177231645191418"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p1072304515143"><a name="p1072304515143"></a><a name="p1072304515143"></a>ASCENDC_TPL_DTYPE_SEL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p12723154513148"><a name="p12723154513148"></a><a name="p12723154513148"></a>Template parameter combination for the custom DataType type.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><a name="ul19723134541416"></a><a name="ul19723134541416"></a><ul id="ul19723134541416"><li>args0: Indicates the parameter name.</li><li>args1-argsn: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_DTYPE_DECL.</li></ul></td>
</tr>
<tr id="row8422954195019"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p109219811513"><a name="p109219811513"></a><a name="p109219811513"></a>ASCENDC_TPL_DATATYPE_SEL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p1542313548508"><a name="p1542313548508"></a><a name="p1542313548508"></a>Template parameter combination for the native DataType type.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><a name="ul29326220525"></a><a name="ul29326220525"></a><ul id="ul29326220525"><li>args0: Indicates the parameter name.</li><li>args1-argsn: The subsequent parameters are a subset of the parameter option range defined in ASCENDC_TPL_DATATYPE_DECL.</li></ul></td>
</tr>
<tr id="row15723144501418"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p19723104531414"><a name="p19723104531414"></a><a name="p19723104531414"></a>ASCENDC_TPL_FORMAT_SEL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p4723124511148"><a name="p4723124511148"></a><a name="p4723124511148"></a>Template parameter combination for the Format type.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><a name="ul37234453140"></a><a name="ul37234453140"></a><ul id="ul37234453140"><li>args0: Indicates the parameter name.</li><li>args1-argsn: The subsequent parameters are a subset of the parameter option range defined in ASCENDC_TPL_FORMAT_DECL.</li></ul></td>
</tr>
<tr id="row177239457144"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p1972354521419"><a name="p1972354521419"></a><a name="p1972354521419"></a>ASCENDC_TPL_UINT_SEL(args0, args1, args2, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p1572311452142"><a name="p1572311452142"></a><a name="p1572311452142"></a>Combination of UINT-type template parameters.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><a name="ul1072316455142"></a><a name="ul1072316455142"></a><ul id="ul1072316455142"><li>args0: Indicates the parameter name.</li><li>args1: Mode of the parameter definition. The following values are supported:<a name="ul47236454144"></a><a name="ul47236454144"></a><ul id="ul47236454144"><li>ASCENDC_TPL_UI_RANGE: range mode.</li><li>ASCENDC_TPL_UI_LIST: exhaustive mode.</li><li>ASCENDC_TPL_UI_MIX: mixed mode.</li></ul>
</li><li>args2-argsn: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_UINT_DECL.</li></ul>
<p id="p20724194561410"><a name="p20724194561410"></a><a name="p20724194561410"></a>For the configuration of modes and parameters, see ASCENDC_TPL_UINT_DECL(args0, args1, args2, ...).</p></td>
</tr>
<tr id="row1172419455142"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p1972414551418"><a name="p1972414551418"></a><a name="p1972414551418"></a>ASCENDC_TPL_BOOL_SEL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p472474591417"><a name="p472474591417"></a><a name="p472474591417"></a>Combination of bool-type template parameters.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p1972434591415"><a name="p1972434591415"></a><a name="p1972434591415"></a>args0: Indicates the parameter name.</p>
<p id="p1724184519144"><a name="p1724184519144"></a><a name="p1724184519144"></a>args1-args2: The subsequent parameters are a subset of the parameter range defined in ASCENDC_TPL_BOOL_DECL.</p></td>
</tr>
<tr id="row1479994315586"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p2604358145810"><a name="p2604358145810"></a><a name="p2604358145810"></a>ASCENDC_TPL_DETERMINISTIC_SEL(args0)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p19799174315815"><a name="p19799174315815"></a><a name="p19799174315815"></a>This group of template parameter combinations is used to configure whether deterministic computation is enabled.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p8111928839"><a name="p8111928839"></a><a name="p8111928839"></a>args0: Indicates the parameter name. The optional value range is [true, false, 1, 0], where [true/1] indicates that this group of template parameter combinations enables deterministic computation, and [false/0] indicates that deterministic computation is not enabled. Note that this value is not passed as a template parameter of the operator. When this value is enabled during compilation, "-DDETERMINISTIC_MODE=1" is added, and JSON and .o files ending with "_deterministic" are generated, for example: "AddCustomTemplate_816f04e052850554f4b3cacb35f8e8c6_deterministic.json"/"AddCustomTemplate_816f04e052850554f4b3cacb35f8e8c6_deterministic.o".</p>
<p id="p1937103317239"><a name="p1937103317239"></a><a name="p1937103317239"></a>Note: If a deterministic computation version is compiled through the ASCENDC_TPL_DETERMINISTIC_SEL(true) API, the deterministic computation switch usually needs to be enabled when the operator is invoked. For example, when invoking through the aclnn single-operator API, the aclrtCtxSetSysParamOpt API needs to be used for related configuration.</p>
<p id="p65913382558"><a name="p65913382558"></a><a name="p65913382558"></a>This parameter supports only the following models:</p>
<a name="ul723415519312"></a><a name="ul723415519312"></a><ul id="ul723415519312"><li><span id="ph16239174011416"><a name="ph16239174011416"></a><a name="ph16239174011416"></a><term id="zh-cn_topic_0000001312391781_term1253731311225"><a name="zh-cn_topic_0000001312391781_term1253731311225"></a><a name="zh-cn_topic_0000001312391781_term1253731311225"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115"><a name="zh-cn_topic_0000001312391781_term131434243115"></a><a name="zh-cn_topic_0000001312391781_term131434243115"></a>Atlas A3 inference products</term></span></li><li><span id="ph1292674871116"><a name="ph1292674871116"></a><a name="ph1292674871116"></a><term id="zh-cn_topic_0000001312391781_term11962195213215"><a name="zh-cn_topic_0000001312391781_term11962195213215"></a><a name="zh-cn_topic_0000001312391781_term11962195213215"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811"><a name="zh-cn_topic_0000001312391781_term184716139811"></a><a name="zh-cn_topic_0000001312391781_term184716139811"></a>Atlas A2 inference products</term></span></li></ul></td>
</tr>
<tr id="row121841620202716"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p17184172011276"><a name="p17184172011276"></a><a name="p17184172011276"></a><span>ASCENDC_TPL_TILING_STRUCT_SEL(args0)</span></p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p518414206272"><a name="p518414206272"></a><a name="p518414206272"></a><span>Specifies the Tiling structure for the current template parameter combination. This structure is not passed as a kernel function template parameter.</span></p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p482024872719"><a name="p482024872719"></a><a name="p482024872719"></a>args0: Indicates the name of the Tiling structure used by the current template parameter combination. This API can only be used in ASCENDC_TPL_ARGS_SEL(...). It does not participate in the parameter order of ASCENDC_TPL_SEL_PARAM(...) or GET_TPL_TILING_KEY(...), nor in TilingKey encoding.</p>
<p id="p13820154810279"><a name="p13820154810279"></a><a name="p13820154810279"></a>Template parameter combinations that do not configure this API use the default Tiling structure registered by REGISTER_TILING_DEFAULT. When using this API, a default Tiling structure must be provided through REGISTER_TILING_DEFAULT.</p></td>
</tr>
<tr id="row125414102715"><td class="cellrowborder" valign="top" width="23.75237523752375%" headers="mcps1.2.4.1.1 "><p id="p1554114112710"><a name="p1554114112710"></a><a name="p1554114112710"></a>ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(args0, ...)</p></td>
<td class="cellrowborder" valign="top" width="28.202820282028203%" headers="mcps1.2.4.1.2 "><p id="p5541748279"><a name="p5541748279"></a><a name="p5541748279"></a>Sets the Kernel type of the operator template parameter combination. This parameter can be passed as a kernel function template parameter.</p></td>
<td class="cellrowborder" valign="top" width="48.04480448044804%" headers="mcps1.2.4.1.3 "><p id="p1663273111272"><a name="p1663273111272"></a><a name="p1663273111272"></a>args0: Parameter name.</p>
<p id="p1063233192717"><a name="p1063233192717"></a><a name="p1063233192717"></a>args1-argsn: The Kernel type of the operator under this template parameter combination. The subsequent parameters are several Kernel types. This API cannot be used together with the ASCENDC_TPL_KERNEL_TYPE_SEL API.</p>
<p id="p1337446101011"><a name="p1337446101011"></a><a name="p1337446101011"></a>If the KERNEL_TASK_TYPE_DEFAULT(value) API is also used, this API takes higher priority.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

After modifying or adding values for the template parameter definition, you must recompile the custom operator package. The previous operator binary can no longer be used.

