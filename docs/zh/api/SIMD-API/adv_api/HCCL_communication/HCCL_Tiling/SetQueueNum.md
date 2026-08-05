# SetQueueNum

## 功能说明

设置每个向服务端下发任务的核上的BatchWrite通信队列数量。

## 函数原型

```
uint32_t SetQueueNum(uint16_t num)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| num | 输入 | 表示队列的数量。参与通信的核数*队列数量支持设置的取值范围为[0, 40]，参与通信的核数的设置请参考[SetCommBlockNum](SetCommBlockNum.md)。 |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败。

## 约束说明

<!-- npu="A3" id1 -->
本接口仅在Atlas A3 训练系列产品/Atlas A3 推理系列产品上通信类型为HCCL\_CMD\_BATCH\_WRITE时生效。
<!-- end id1 -->

## 调用示例

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_BATCH_WRITE;
std::string algConfig = "BatchWrite=level0:fullmesh";
uint32_t reduceType = 0; // BatchWrite无归约语义，此处占位
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetQueueNum(2U);
```
