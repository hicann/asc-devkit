# InitValue<a name="ZH-CN_TOPIC_0000002078492712"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T11:12:41.500Z pushedAt=2026-11-13T08:46:49.447Z nl=2 -->

## Description<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

This host-side API sets the initial value of the operator output. After the value is set, the GM space of the operator output is cleared or a memset-type operator is inserted to set the initial value before operator execution.

**InitValue is used together with the SetNeedAtomic API, and the SetNeedAtomic API must be set to true.**

## Prototype<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

-   Before operator execution, the GM space corresponding to the output parameter is cleared.

    ```
    OpParamDef &InitValue(uint64_t value)
    ```

-   Specifies the type and value of the initial value of the specified output parameter. When the output parameter calls this API, a memset-type operator with the corresponding type and value is inserted into the GM space corresponding to the output parameter before operator execution.

    ```
    OpParamDef &InitValue(const ScalarVar &value)
    ```

-   Specifies a list of initial value types and values for the output parameter, which correspond in sequence to the data type and data format combinations of the output parameter. Before operator execution, a memset-type operator with the corresponding type and value is inserted into the GM space corresponding to the output parameter.

    ```
    OpParamDef &InitValue(const std::vector<ScalarVar> &value)
    ```

## Parameters<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001960169216_p466181582315"><a name="zh-cn_topic_0000001960169216_p466181582315"></a><a name="zh-cn_topic_0000001960169216_p466181582315"></a>value</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001960169216_p1464218131118"><a name="zh-cn_topic_0000001960169216_p1464218131118"></a><a name="zh-cn_topic_0000001960169216_p1464218131118"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><a name="zh-cn_topic_0000001960169216_ul130045510411"></a><a name="zh-cn_topic_0000001960169216_ul130045510411"></a><ul id="zh-cn_topic_0000001960169216_ul130045510411"><li>uint64_t type parameter<p id="zh-cn_topic_0000001960169216_p1920504112421"><a name="zh-cn_topic_0000001960169216_p1920504112421"></a><a name="zh-cn_topic_0000001960169216_p1920504112421"></a>Only 0 is supported. When an output parameter calls this API, the GM space corresponding to the output parameter is cleared to zero before execution.</p>
</li><li>ScalarVar type parameter<p id="zh-cn_topic_0000001960169216_p328318251747"><a name="zh-cn_topic_0000001960169216_p328318251747"></a><a name="zh-cn_topic_0000001960169216_p328318251747"></a>ScalarVar specifies the type ScalarType and value ScalarNum of the initial value of the output parameter. The definitions are as follows:</p>
<a name="zh-cn_topic_0000001960169216_screen1644815588715"></a><a name="zh-cn_topic_0000001960169216_screen1644815588715"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001960169216_screen1644815588715">enum class ScalarType : uint32_t {
  UINT64 = 0,
  INT64 = 1,
  UINT32 = 2,
  INT32 = 3,
  UINT16 = 4,
  INT16 = 5,
  UINT8 = 6,
  INT8 = 7,
  FLOAT32 = 8,
  FLOAT16 = 9,
  INVALID_DTYPE = static_cast&lt;uint32_t&gt;(-1),
};
union ScalarNum {
  uint64_t value_u64;
  int64_t value_i64;
  float value_f32;
  ScalarNum() : value_u64(0) {}
  explicit ScalarNum(uint64_t value) : value_u64(value) {}
  explicit ScalarNum(int64_t value) : value_i64(value) {}
  explicit ScalarNum(float value) : value_f32(value) {}
};
struct ScalarVar {
  ScalarType scalar_type;
  ScalarNum scalar_num;
  ScalarVar();
  ScalarVar(ScalarType type, uint64_t num);
  ScalarVar(ScalarType type, int64_t num);
  ScalarVar(ScalarType type, int num);
  ScalarVar(ScalarType type, unsigned int num);
  ScalarVar(ScalarType type, float num);
  ScalarVar(ScalarType type, double num);
  bool operator==(const ScalarVar& other) const;
};</pre>
<p id="zh-cn_topic_0000001960169216_p7872325173718"><a name="zh-cn_topic_0000001960169216_p7872325173718"></a><a name="zh-cn_topic_0000001960169216_p7872325173718"></a>ScalarType currently supports only UINT64/INT64/UINT32/INT32/UINT16/INT16/UINT8/INT8/FLOAT32/FLOAT16.</p>
<p id="zh-cn_topic_0000001960169216_p17482719222"><a name="zh-cn_topic_0000001960169216_p17482719222"></a><a name="zh-cn_topic_0000001960169216_p17482719222"></a>ScalarNum supports the uint64_t/int64_t/float types.</p>
<p id="zh-cn_topic_0000001960169216_p14507521395"><a name="zh-cn_topic_0000001960169216_p14507521395"></a><a name="zh-cn_topic_0000001960169216_p14507521395"></a>For ease of use, ScalarVar also supports immediate value initialization, as shown in the following example:</p>
<a name="zh-cn_topic_0000001960169216_screen9280101782212"></a><a name="zh-cn_topic_0000001960169216_screen9280101782212"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001960169216_screen9280101782212">InitValue({ScalarType::INT16, 1});</pre>
</li><li>const std::vector&lt;ScalarVar&gt; &value type<p id="zh-cn_topic_0000001960169216_p7183525183613"><a name="zh-cn_topic_0000001960169216_p7183525183613"></a><a name="zh-cn_topic_0000001960169216_p7183525183613"></a>List of the initial value types and values of the specified output parameters, corresponding in sequence to the data types and data format combinations of the output parameters.</p>
</li></ul>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   InitValue must be used together with the SetNeedAtomic API; otherwise, initialization does not take effect.
-   For the OpParamDef &InitValue\(uint64\_t value\) API, the supported data types of the operator output parameter are as follows: UINT64/INT64/UINT32/INT32/UINT16/INT16/UINT8/INT8/FLOAT32/FLOAT16. Values outside this range result in undefined behavior.
-   For the OpParamDef &InitValue\(const std::vector<ScalarVar\> &value\) API, the size of the input value must be consistent with the size of the [DataType](DataType.md) or [DataTypeList](DataTypeList.md) API parameter configured for the output parameter. In addition, for the same data type, the configured type and value must be identical; otherwise, an error is reported.
-   For the same output parameter, only one API can be called to set the initial value. Calling multiple InitValue APIs results in undefined behavior. If the same API is called multiple times, the initial value set by the last call takes effect.
-   A custom operator project generated based on an earlier CANN package (which does not support the InitValue feature) is incompatible with the InitValue API. When using a custom operator project generated by a CANN package of a version other than the current one, pay special attention to compatibility. You can check whether the current project supports this feature by checking whether the output\_init\_value field exists in cmake/util/ascendc\_impl\_build.py under the custom operator project. If the field is not found, regenerate the custom operator project to enable the InitValue feature.

## Examples<a name="zh-cn_topic_0000001960169216_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_section320753512363"></a>

```
// Example of OpParamDef &InitValue(uint64_t value)
this->Output("z")
     .ParamType(REQUIRED)
     .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
     .FormatList({ ge::FORMAT_ND})
     .InitValue(0);

// Example of OpParamDef &InitValue(const ScalarVar &value)
this->Output("z")
     .ParamType(REQUIRED)
     .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
     .FormatList({ ge::FORMAT_ND})
     .InitValue({ScalarType::INT16, 1});

// Example of OpParamDef &InitValue(const std::vector<ScalarVar> &value).
this->Output("z")
     .ParamType(REQUIRED)
     .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
     .FormatList({ ge::FORMAT_ND})
     .InitValue({{ScalarType::INT16, 1}, {ScalarType::FLOAT32, 3.2}, {ScalarType::INT64, 7}});

this->Output("z")
     .ParamType(REQUIRED)
     .DataType({ge::DT_INT32, ge::DT_FLOAT, ge::DT_INT32})  // The first and third DataType are the same.
     .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_NHWC})
     .InitValue({{ScalarType::INT16, 1}, {ScalarType::FLOAT32, 3.2}, {ScalarType::INT16, 1}}); // The data types and values corresponding to InitValue must also be the same.
```

