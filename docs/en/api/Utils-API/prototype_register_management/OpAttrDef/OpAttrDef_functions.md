# OpAttrDef<a name="ZH-CN_TOPIC_0000002078492716"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T10:54:37.388Z pushedAt=2026-11-13T08:46:49.361Z nl=2 -->

## Description<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

Defines an operator attribute.

## Prototype<a name="zh-cn_topic_0000001656780128_section3992421457"></a>

```
class OpAttrDef {
public:
  explicit OpAttrDef(const char *name);
  OpAttrDef(const OpAttrDef &attr_def);
  ~OpAttrDef();
  OpAttrDef &operator=(const OpAttrDef &attr_def);
  OpAttrDef &AttrType(Option attr_type);
  OpAttrDef &Bool(void);
  OpAttrDef &Bool(bool value);
  OpAttrDef &Float(void);
  OpAttrDef &Float(float value);
  OpAttrDef &Int(void);
  OpAttrDef &Int(int64_t value);
  OpAttrDef &String(void);
  OpAttrDef &String(const char *value);
  OpAttrDef &ListBool(void);
  OpAttrDef &ListBool(std::vector<bool> value);
  OpAttrDef &ListFloat(void);
  OpAttrDef &ListFloat(std::vector<float> value);
  OpAttrDef &ListInt(void);
  OpAttrDef &ListInt(std::vector<int64_t> value);
  OpAttrDef &ListListInt(void);
  OpAttrDef &ListListInt(std::vector<std::vector<int64_t>> value);
  OpAttrDef &Version(uint32_t version);
  ge::AscendString &GetName(void) const;
  bool IsRequired(void);
private:
  ...
};
```

## Description<a name="zh-cn_topic_0000001656780128_section1340317244469"></a>

**Table 1** OpAttrDef class member function description

<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_table18149577913"></a>

| Function Name | Input Parameter Description | Description |
| --- | --- | --- |
| OpAttrDef | name: Operator attribute name. | Constructs the operator attribute definition OpAttrDef and sets the attribute name. |
| OpAttrDef | attr_def: Operator attribute OpAttrDef that has been constructed. | Copy-constructs OpAttrDef, copying the attribute name, attribute data type, attribute type, default value, version number, and comments from the copied object. |
| AttrType | attr_type: Attribute type. | Sets the operator attribute type. The value can be OPTIONAL (optional) or REQUIRED (required). |
| Bool | None | Sets the operator attribute data type to Bool. |
| Bool | value | Sets the operator attribute data type to Bool and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| Float | None | Sets the operator attribute data type to Float. |
| Float | value | Sets the operator attribute data type to Float and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| Int | None | Sets the operator attribute data type to Int. |
| Int | value | Sets the operator attribute data type to Int and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| String | None | Sets the operator attribute data type to String. |
| String | value | Sets the operator attribute data type to String and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| ListBool | None | Sets the operator attribute data type to ListBool. |
| ListBool | value | Sets the operator attribute data type to ListBool and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| ListFloat | None | Sets the operator attribute data type to ListFloat. |
| ListFloat | value | Sets the operator attribute data type to ListFloat and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| ListInt | None | Sets the operator attribute data type to ListInt. |
| ListInt | value | Sets the operator attribute data type to ListInt and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| ListListInt | None | Sets the operator attribute data type to ListListInt. |
| ListListInt | value | Sets the operator attribute data type to ListListInt and sets the attribute default value to value. When the attribute type is set to OPTIONAL, you must call this type of API to set the default value. |
| Version | version: Version number to configure. | When adding optional attributes, to maintain compatibility with the existing single-operator API (aclnnxxx), you can use the Version API to configure the version number of the aclnn API. The version number must start from 1 and be configured consecutively (numbered consistently with [optional input](../OpParamDef/Version.md)). After configuration, the automatically generated aclnn API carries the version number. An API with a higher version number includes all parameters of APIs with lower version numbers. |
| GetName | None | Obtains the attribute name. |
| IsRequired | None | Checks whether the operator attribute is required. Returns true if required and false if optional. |
