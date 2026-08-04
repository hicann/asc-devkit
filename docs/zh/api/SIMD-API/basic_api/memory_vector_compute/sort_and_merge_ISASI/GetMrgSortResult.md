# GetMrgSortResult<a name="ZH-CN_TOPIC_0000001790047014"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_proposal_intf.h"`

<!-- npu="950,A3,910b,310b" id12 -->

- GetMrgSortResult接口需与[MrgSort](MrgSort.md)接口配合使用。当MrgSort接口中的ifExhaustedSuspension参数设置为true时，若某个输入队列中的数据耗尽，MrgSort会停止执行。此时，可调用GetMrgSortResult接口来获取4个队列里已经处理过的数据个数。

    以上说明适用于如下型号：

    <!-- npu="950" id8 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id10 -->

    <!-- npu="310b" id11 -->
    - Atlas 200I/500 A2 推理产品
    <!-- end id11 -->

<!-- end id12 -->

<!-- npu="310p" id13 -->

- 获取MrgSort已经处理过的队列里的Region Proposal个数，并依次存储在四个出参中。

    配合[MrgSort4指令](MrgSort4.md)使用，获取MrgSort4指令处理过的队列里的Region Proposal个数。使用时，需要将MrgSort4中的MrgSort4Info.ifExhaustedSuspension参数配置为true，该配置模式下某条队列耗尽后，MrgSort4指令即停止。

    以上说明适用于如下型号：

    Atlas 推理系列产品AI Core

<!-- end id13 -->

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void GetMrgSortResult(uint16_t &mrgSortList1, uint16_t &mrgSortList2, uint16_t &mrgSortList3, uint16_t &mrgSortList4)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|:---|:---|:---|
| mrgSortList1 | 输出 | 类型为uint16\_t，表示MrgSort第一个队列里已经处理过的Region Proposal个数。 |
| mrgSortList2 | 输出 | 类型为uint16\_t，表示MrgSort第二个队列里已经处理过的Region Proposal个数。 |
| mrgSortList3 | 输出 | 类型为uint16\_t，表示MrgSort第三个队列里已经处理过的Region Proposal个数。 |
| mrgSortList4 | 输出 | 类型为uint16\_t，表示MrgSort第四个队列里已经处理过的Region Proposal个数。 |

## 约束说明<a name="section837496171220"></a>

需与MrgSort接口配合使用，且MrgSort接口中的ifExhaustedSuspension参数必须设置为true。

## 调用示例<a name="section837496171220"></a>

- 配合[MrgSort指令](MrgSort.md)使用示例。

    ```cpp
    
    uint16_t elementLengths[4] = { 0 };
    uint32_t sortedNum[4] = { 0 };
    elementLengths[0] = 32;
    elementLengths[1] = 32;
    elementLengths[2] = 32;
    elementLengths[3] = 32;
    uint16_t validBit = 0b1111;
    
    AscendC::MrgSortSrcList<float> srcList;
    srcList.src1 = workLocal[0];
    srcList.src2 = workLocal[32 * 1 * 2];
    srcList.src3 = workLocal[32 * 2 * 2];
    srcList.src4 = workLocal[32 * 3 * 2];
    
    AscendC::MrgSort4Info mrgSortInfo(elementLengths, true, validBit, 1);
    AscendC::MrgSort(dstLocal, srcList, mrgSortInfo);
    
    uint16_t mrgRes1 = 0;
    uint16_t mrgRes2 = 0;
    uint16_t mrgRes3 = 0;
    uint16_t mrgRes4 = 0;
    AscendC::GetMrgSortResult(mrgRes1, mrgRes2, mrgRes3, mrgRes4);
    ```

    输出示例:

    ```cpp
    srcList: 
     [1.   2.   3.   4.   5.   6.   7.   8.   9.  10.  11.  12.  13.  14.
      15.  16.  17.  18.  19.  20.  21.  22.  23.  24.  25.  26.  27.  28.
      29.  30.  31.  32.  33.  34.  35.  36.  37.  38.  39.  40.  41.  42.
      43.  44.  45.  46.  47.  48.  49.  50.  51.  52.  53.  54.  55.  56.
      57.  58.  59.  60.  61.  62.  63.  64.  65.  66.  67.  68.  69.  70.
      71.  72.  73.  74.  75.  76.  77.  78.  79.  80.  81.  82.  83.  84.
      85.  86.  87.  88.  89.  90.  91.  92.  93.  94.  95.  96.  97.  98.
      99. 100. 101. 102. 103. 104. 105. 106. 107. 108. 109. 110. 111. 112.
     113. 114. 115. 116. 117. 118. 119. 120. 121. 122. 123. 124. 125. 126.
     127. 128.]
    workLocal:
     [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
    dstLocal:
     [128.   0. 127.   0. 126.   0. 125.   0. 124.   0. 123.   0. 122.   0.
     121.   0. 120.   0. 119.   0. 118.   0. 117.   0. 116.   0. 115.   0.
     114.   0. 113.   0. 112.   0. 111.   0. 110.   0. 109.   0. 108.   0.
     107.   0. 106.   0. 105.   0. 104.   0. 103.   0. 102.   0. 101.   0.
     100.   0.  99.   0.  98.   0.  97.   0.  96.   0.  95.   0.  94.   0.
      93.   0.  92.   0.  91.   0.  90.   0.  89.   0.  88.   0.  87.   0.
      86.   0.  85.   0.  84.   0.  83.   0.  82.   0.  81.   0.  80.   0.
      79.   0.  78.   0.  77.   0.  76.   0.  75.   0.  74.   0.  73.   0.
      72.   0.  71.   0.  70.   0.  69.   0.  68.   0.  67.   0.  66.   0.
      65.   0.  64.   0.  63.   0.  62.   0.  61.   0.  60.   0.  59.   0.
      58.   0.  57.   0.  56.   0.  55.   0.  54.   0.  53.   0.  52.   0.
      51.   0.  50.   0.  49.   0.  48.   0.  47.   0.  46.   0.  45.   0.
      44.   0.  43.   0.  42.   0.  41.   0.  40.   0.  39.   0.  38.   0.
      37.   0.  36.   0.  35.   0.  34.   0.  33.   0.  32.   0.  31.   0.
      30.   0.  29.   0.  28.   0.  27.   0.  26.   0.  25.   0.  24.   0.
      23.   0.  22.   0.  21.   0.  20.   0.  19.   0.  18.   0.  17.   0.
      16.   0.  15.   0.  14.   0.  13.   0.  12.   0.  11.   0.  10.   0.
       9.   0.   8.   0.   7.   0.   6.   0.   5.   0.   4.   0.   3.   0.
       2.   0.   1.   0.]
    ```

- 配合[MrgSort4指令](MrgSort4.md)使用示例。

    ```cpp
    uint16_t elementLengths[4] = { 0 };
    uint32_t sortedNum[4] = { 0 };
    elementLengths[0] = 32;
    elementLengths[1] = 32;
    elementLengths[2] = 32;
    elementLengths[3] = 32;
    uint16_t validBit = 0b1111;
    
    AscendC::MrgSortSrcList<float> srcList;
    srcList.src1 = workLocal[0];
    srcList.src2 = workLocal[32 * 1 * 2];
    srcList.src3 = workLocal[32 * 2 * 2];
    srcList.src4 = workLocal[32 * 3 * 2];
    
    AscendC::MrgSort4Info mrgSortInfo(elementLengths, true, validBit, 1);
    AscendC::MrgSort4(dstLocal, srcList, mrgSortInfo);
    
    uint16_t mrgRes1 = 0;
    uint16_t mrgRes2 = 0;
    uint16_t mrgRes3 = 0;
    uint16_t mrgRes4 = 0;
    AscendC::GetMrgSortResult(mrgRes1, mrgRes2, mrgRes3, mrgRes4);
    ```
