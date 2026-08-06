# OpAttrDef<a name="ZH-CN_TOPIC_0000002078492716"></a>

## 功能说明<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

定义算子属性。

## 函数原型<a name="zh-cn_topic_0000001656780128_section3992421457"></a>

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

## 函数说明<a name="zh-cn_topic_0000001656780128_section1340317244469"></a>

**表1**  OpAttrDef类成员函数说明

<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_table18149577913"></a>

| 函数名称 | 入参说明 | 功能说明 |
| --- | --- | --- |
| OpAttrDef | name: 算子属性名称。 | 构造算子属性定义OpAttrDef，并设置属性名称。 |
| OpAttrDef | attr_def: 已构造完成的算子属性OpAttrDef。 | 拷贝构造OpAttrDef，复制被拷贝对象的属性名称、属性数据类型、属性类型、默认值、版本号和注释等配置。 |
| AttrType | attr_type: 属性类型 | 设置算子属性类型，取值为：OPTIONAL（可选）、REQUIRED（必选）。 |
| Bool | 无 | 设置算子属性数据类型为Bool |
| Bool | value | 设置算子属性数据类型为Bool，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| Float | 无 | 设置算子属性数据类型为Float |
| Float | value | 设置算子属性数据类型为Float，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| Int | 无 | 设置算子属性数据类型为Int |
| Int | value | 设置算子属性数据类型为Int，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| String | 无 | 设置算子属性数据类型为String |
| String | value | 设置算子属性数据类型为String，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| ListBool | 无 | 设置算子属性数据类型为ListBool |
| ListBool | value | 设置算子属性数据类型为ListBool，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| ListFloat | 无 | 设置算子属性数据类型为ListFloat |
| ListFloat | value | 设置算子属性数据类型为ListFloat，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| ListInt | 无 | 设置算子属性数据类型为ListInt |
| ListInt | value | 设置算子属性数据类型为ListInt，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| ListListInt | 无 | 设置算子属性数据类型为ListListInt |
| ListListInt | value | 设置算子属性数据类型为ListListInt，并设置属性默认值为value。属性类型设置为OPTIONAL时必须调用该类接口设置默认值。 |
| Version | version：配置的版本号 | 新增可选属性时，为了保持原有单算子API(aclnnxxx)接口的兼容性，可以通过Version接口配置aclnn接口的版本号，版本号需要从1开始配，且应该连续配置（和[可选输入](../OpParamDef/Version.md)统一编号）。配置后，自动生成的aclnn接口会携带版本号。高版本号的接口会包含低版本号接口的所有参数。 |
| GetName | 无 | 获取属性名称。 |
| IsRequired | 无 | 判断算子属性是否为必选，必选返回true，可选返回false。 |
