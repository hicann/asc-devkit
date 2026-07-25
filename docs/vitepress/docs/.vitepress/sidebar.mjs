function removeSelfRefItems(items) {
  return items.map(item => {
    const entry = { ...item }
    if (entry.items && entry.items.length > 0) {
      const children = removeSelfRefItems(entry.items)
      if (children.length > 0 && children[0].text === entry.text && children[0].link) {
        entry.items = children.slice(1)
      } else {
        entry.items = children
      }
    }
    return entry
  })
}

function addNumbering(items, prefix = '') {
  return items.map((item, i) => {
    const num = prefix ? `${prefix}${i + 1}.` : `${i + 1}.`
    return {
      ...item,
      text: `${num} ${item.text}`,
      ...(item.items ? { items: addNumbering(item.items, num) } : {}),
    }
  })
}

export default {
  "/guide/": addNumbering(removeSelfRefItems([
  {
    "text": "入门教程",
    "collapsed": true,
    "items": [
      {
        "text": "Ascend C概述与学习路径",
        "link": "/guide/入门教程/Ascend-C概述与学习路径"
      },
      {
        "text": "环境准备",
        "link": "/guide/入门教程/环境准备"
      },
      {
        "text": "快速入门",
        "collapsed": true,
        "items": [
          {
            "text": "异构系统与编程模型",
            "link": "/guide/入门教程/快速入门/异构系统与编程模型"
          },
          {
            "text": "基于SIMD编程",
            "collapsed": true,
            "items": [
              {
                "text": "基于SIMD编程",
                "link": "/guide/入门教程/快速入门/基于SIMD编程/基于SIMD编程"
              },
              {
                "text": "HelloWorld",
                "link": "/guide/入门教程/快速入门/基于SIMD编程/HelloWorld"
              },
              {
                "text": "Add算子快速入门",
                "link": "/guide/入门教程/快速入门/基于SIMD编程/Add算子快速入门"
              }
            ]
          },
          {
            "text": "基于SIMT编程",
            "collapsed": true,
            "items": [
              {
                "text": "基于SIMT编程",
                "link": "/guide/入门教程/快速入门/基于SIMT编程/基于SIMT编程"
              },
              {
                "text": "HelloWorld",
                "link": "/guide/入门教程/快速入门/基于SIMT编程/HelloWorld"
              },
              {
                "text": "Add算子快速入门",
                "link": "/guide/入门教程/快速入门/基于SIMT编程/Add算子快速入门"
              }
            ]
          }
        ]
      }
    ]
  },
  {
    "text": "编程指南",
    "collapsed": true,
    "items": [
      {
        "text": "本文档组织结构",
        "link": "/guide/编程指南/本文档组织结构"
      },
      {
        "text": "编程模型",
        "collapsed": true,
        "items": [
          {
            "text": "编程模型",
            "link": "/guide/编程指南/编程模型/编程模型"
          },
          {
            "text": "异构系统",
            "link": "/guide/编程指南/编程模型/异构系统"
          },
          {
            "text": "编程模型概述",
            "link": "/guide/编程指南/编程模型/编程模型概述"
          },
          {
            "text": "AI Core SIMD编程",
            "collapsed": true,
            "items": [
              {
                "text": "AI Core SIMD编程",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/AI-Core-SIMD编程"
              },
              {
                "text": "概述",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/概述"
              },
              {
                "text": "抽象硬件架构",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/抽象硬件架构"
              },
              {
                "text": "核函数",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数"
              },
              {
                "text": "基于指针的C语言编程",
                "collapsed": true,
                "items": [
                  {
                    "text": "基于语言扩展层C API编程",
                    "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/基于指针的C语言编程/基于语言扩展层C-API编程"
                  }
                ]
              },
              {
                "text": "基于Tensor的C++编程",
                "collapsed": true,
                "items": [
                  {
                    "text": "静态Tensor编程",
                    "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程"
                  },
                  {
                    "text": "Reg矢量计算编程",
                    "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/Reg矢量计算编程"
                  }
                ]
              },
              {
                "text": "基于TPipe和TQue编程",
                "collapsed": true,
                "items": [
                  {
                    "text": "TPipe-TQue框架编程原理",
                    "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程原理"
                  },
                  {
                    "text": "TPipe-TQue框架编程范式",
                    "link": "/guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程范式"
                  }
                ]
              }
            ]
          },
          {
            "text": "AI Core SIMT编程",
            "collapsed": true,
            "items": [
              {
                "text": "AI Core SIMT编程",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/AI-Core-SIMT编程"
              },
              {
                "text": "抽象硬件架构",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/抽象硬件架构"
              },
              {
                "text": "线程架构",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/线程架构"
              },
              {
                "text": "内存层级",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/内存层级"
              },
              {
                "text": "核函数",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/核函数"
              },
              {
                "text": "同步机制",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/同步机制"
              },
              {
                "text": "编程示例",
                "link": "/guide/编程指南/编程模型/AI-Core-SIMT编程/编程示例"
              }
            ]
          },
          {
            "text": "AI CPU编程",
            "link": "/guide/编程指南/编程模型/AI-CPU编程"
          }
        ]
      },
      {
        "text": "编译与运行",
        "collapsed": true,
        "items": [
          {
            "text": "异步执行",
            "link": "/guide/编程指南/编译与运行/异步执行"
          },
          {
            "text": "算子编译",
            "collapsed": true,
            "items": [
              {
                "text": "毕昇编译器",
                "link": "/guide/编程指南/编译与运行/算子编译/毕昇编译器"
              },
              {
                "text": "AI Core算子编译基本用法",
                "link": "/guide/编程指南/编译与运行/算子编译/AI-Core算子编译基本用法"
              },
              {
                "text": "AI CPU算子编译基本用法",
                "link": "/guide/编程指南/编译与运行/算子编译/AI-CPU算子编译基本用法"
              },
              {
                "text": "RTC运行时编译",
                "link": "/guide/编程指南/编译与运行/算子编译/RTC运行时编译"
              },
              {
                "text": "约束说明",
                "link": "/guide/编程指南/编译与运行/算子编译/约束说明"
              }
            ]
          }
        ]
      },
      {
        "text": "语言扩展层",
        "collapsed": true,
        "items": [
          {
            "text": "语言扩展层",
            "link": "/guide/编程指南/语言扩展层/语言扩展层"
          },
          {
            "text": "SIMD BuiltIn关键字",
            "link": "/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字"
          },
          {
            "text": "SIMT BuiltIn关键字",
            "link": "/guide/编程指南/语言扩展层/SIMT-BuiltIn关键字"
          },
          {
            "text": "SIMD语言扩展层C API",
            "link": "/guide/编程指南/语言扩展层/SIMD语言扩展层C-API"
          },
          {
            "text": "SIMT语言扩展层C API",
            "link": "/guide/编程指南/语言扩展层/SIMT语言扩展层C-API"
          }
        ]
      },
      {
        "text": "C++类库API",
        "collapsed": true,
        "items": [
          {
            "text": "C++类库API",
            "link": "/guide/编程指南/类库API/类库API"
          },
          {
            "text": "编程接口概述",
            "link": "/guide/编程指南/类库API/编程接口概述"
          },
          {
            "text": "基础API",
            "collapsed": true,
            "items": [
              {
                "text": "概述",
                "link": "/guide/编程指南/类库API/基础API/概述"
              },
              {
                "text": "接口分类说明",
                "collapsed": true,
                "items": [
                  {
                    "text": "连续计算API",
                    "link": "/guide/编程指南/类库API/基础API/接口分类说明/连续计算API"
                  },
                  {
                    "text": "高维切分API",
                    "link": "/guide/编程指南/类库API/基础API/接口分类说明/高维切分API"
                  }
                ]
              },
              {
                "text": "常用操作速查指导",
                "collapsed": true,
                "items": [
                  {
                    "text": "如何使用掩码操作API",
                    "link": "/guide/编程指南/类库API/基础API/常用操作速查指导/如何使用掩码操作API"
                  },
                  {
                    "text": "如何使用归约计算API",
                    "link": "/guide/编程指南/类库API/基础API/常用操作速查指导/如何使用归约计算API"
                  }
                ]
              }
            ]
          },
          {
            "text": "高阶API",
            "collapsed": true,
            "items": [
              {
                "text": "概述",
                "link": "/guide/编程指南/类库API/高阶API/概述"
              },
              {
                "text": "常用操作速查指导",
                "collapsed": true,
                "items": [
                  {
                    "text": "如何使用Tiling依赖的头文件",
                    "link": "/guide/编程指南/类库API/高阶API/常用操作速查指导/如何使用Tiling依赖的头文件"
                  },
                  {
                    "text": "如何使用Kernel侧临时空间",
                    "link": "/guide/编程指南/类库API/高阶API/常用操作速查指导/如何使用Kernel侧临时空间"
                  }
                ]
              }
            ]
          },
          {
            "text": "Utils API",
            "link": "/guide/编程指南/类库API/Utils-API"
          }
        ]
      },
      {
        "text": "调试调优",
        "collapsed": true,
        "items": [
          {
            "text": "调试调优",
            "link": "/guide/编程指南/调试调优/调试调优"
          },
          {
            "text": "概述",
            "link": "/guide/编程指南/调试调优/概述"
          },
          {
            "text": "功能调试",
            "collapsed": true,
            "items": [
              {
                "text": "功能调试",
                "link": "/guide/编程指南/调试调优/功能调试/功能调试"
              },
              {
                "text": "CPU域孪生调试",
                "link": "/guide/编程指南/调试调优/功能调试/CPU域孪生调试"
              },
              {
                "text": "NPU域上板调试",
                "link": "/guide/编程指南/调试调优/功能调试/NPU域上板调试"
              }
            ]
          },
          {
            "text": "性能调优",
            "link": "/guide/编程指南/调试调优/性能调优"
          }
        ]
      },
      {
        "text": "高级编程",
        "collapsed": true,
        "items": [
          {
            "text": "高级编程",
            "link": "/guide/编程指南/高级编程/硬件实现/硬件实现"
          },
          {
            "text": "硬件实现",
            "collapsed": true,
            "items": [
              {
                "text": "硬件实现",
                "link": "/guide/编程指南/高级编程/硬件实现/硬件实现"
              },
              {
                "text": "基本架构",
                "link": "/guide/编程指南/高级编程/硬件实现/基本架构"
              },
              {
                "text": "架构规格",
                "collapsed": true,
                "items": [
                  {
                    "text": "架构规格",
                    "link": "/guide/编程指南/高级编程/硬件实现/架构规格/架构规格"
                  },
                  {
                    "text": "NPU架构版本2002",
                    "link": "/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本2002"
                  },
                  {
                    "text": "NPU架构版本2201",
                    "link": "/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本2201"
                  },
                  {
                    "text": "NPU架构版本3002",
                    "link": "/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3002"
                  },
                  {
                    "text": "NPU架构版本3510",
                    "link": "/guide/编程指南/高级编程/硬件实现/架构规格/NPU架构版本3510"
                  }
                ]
              },
              {
                "text": "硬件约束",
                "collapsed": true,
                "items": [
                  {
                    "text": "NPU架构版本2002",
                    "link": "/guide/编程指南/高级编程/硬件实现/硬件约束/NPU架构版本2002"
                  },
                  {
                    "text": "NPU架构版本2201",
                    "link": "/guide/编程指南/高级编程/硬件实现/硬件约束/NPU架构版本2201"
                  }
                ]
              }
            ]
          },
          {
            "text": "高级AI-Core编程模型",
            "collapsed": true,
            "items": [
              {
                "text": "SIMD与SIMT混合编程",
                "link": "/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程"
              }
            ]
          },
          {
            "text": "高级特性",
            "collapsed": true,
            "items": [
              {
                "text": "高级特性",
                "link": "/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程"
              },
              {
                "text": "Aclnn算子工程化开发",
                "collapsed": true,
                "items": [
                  {
                    "text": "概述",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/概述"
                  },
                  {
                    "text": "创建算子工程",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/创建算子工程"
                  },
                  {
                    "text": "算子原型定义",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/算子原型定义"
                  },
                  {
                    "text": "Kernel侧算子实现",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Kernel侧算子实现"
                  },
                  {
                    "text": "Host侧Tiling实现",
                    "collapsed": true,
                    "items": [
                      {
                        "text": "基本流程",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Host侧Tiling实现/基本流程"
                      },
                      {
                        "text": "通过TilingData传递属性信息",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Host侧Tiling实现/通过TilingData传递属性信息"
                      },
                      {
                        "text": "使用高阶API时配套的Tiling实现",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Host侧Tiling实现/使用高阶API时配套的Tiling实现"
                      },
                      {
                        "text": "使用标准C++语法定义Tiling结构体",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Host侧Tiling实现/使用标准CPP语法定义Tiling结构体"
                      },
                      {
                        "text": "Tiling模板编程",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/Host侧Tiling实现/Tiling模板编程"
                      }
                    ]
                  },
                  {
                    "text": "算子包编译",
                    "collapsed": true,
                    "items": [
                      {
                        "text": "算子工程编译",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/算子包编译/算子工程编译"
                      },
                      {
                        "text": "算子包部署",
                        "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/算子包编译/算子包部署"
                      }
                    ]
                  },
                  {
                    "text": "算子动态库和静态库编译",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/算子动态库和静态库编译"
                  },
                  {
                    "text": "算子工程编译拓展",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/算子工程编译拓展"
                  },
                  {
                    "text": "单算子API调用",
                    "link": "/guide/编程指南/高级编程/高级特性/Aclnn算子工程化开发/单算子API调用"
                  }
                ]
              },
              {
                "text": "算子入图开发",
                "collapsed": true,
                "items": [
                  {
                    "text": "概述",
                    "link": "/guide/编程指南/高级编程/高级特性/算子入图开发/概述"
                  },
                  {
                    "text": "基本开发流程",
                    "link": "/guide/编程指南/高级编程/高级特性/算子入图开发/基本开发流程"
                  },
                  {
                    "text": "使能Tiling下沉",
                    "link": "/guide/编程指南/高级编程/高级特性/算子入图开发/使能Tiling下沉"
                  },
                  {
                    "text": "SuperKernel开发",
                    "link": "/guide/编程指南/高级编程/高级特性/算子入图开发/SuperKernel开发"
                  },
                  {
                    "text": "图编译和图执行",
                    "link": "/guide/编程指南/高级编程/高级特性/算子入图开发/图编译和图执行"
                  }
                ]
              },
              {
                "text": "AI框架算子适配",
                "collapsed": true,
                "items": [
                  {
                    "text": "概述",
                    "link": "/guide/编程指南/高级编程/高级特性/AI框架算子适配/概述"
                  },
                  {
                    "text": "PyTorch框架",
                    "link": "/guide/编程指南/高级编程/高级特性/AI框架算子适配/PyTorch框架"
                  },
                  {
                    "text": "ONNX框架",
                    "collapsed": true,
                    "items": [
                      {
                        "text": "适配插件开发",
                        "link": "/guide/编程指南/高级编程/高级特性/AI框架算子适配/ONNX框架/适配插件开发"
                      },
                      {
                        "text": "调用样例",
                        "link": "/guide/编程指南/高级编程/高级特性/AI框架算子适配/ONNX框架/调用样例"
                      }
                    ]
                  },
                  {
                    "text": "TensorFlow框架",
                    "link": "/guide/编程指南/高级编程/高级特性/AI框架算子适配/TensorFlow框架"
                  }
                ]
              }
            ]
          }
        ]
      },
      {
        "text": "附录",
        "collapsed": true,
        "items": [
          {
            "text": "show_kernel_debug_data工具",
            "link": "/guide/编程指南/附录/show_kernel_debug_data工具"
          },
          {
            "text": "msobjdump工具",
            "link": "/guide/编程指南/附录/msobjdump工具"
          },
          {
            "text": "基于样例工程完成Kernel直调",
            "link": "/guide/编程指南/附录/基于样例工程完成Kernel直调"
          },
          {
            "text": "简易自定义算子工程",
            "link": "/guide/编程指南/附录/简易自定义算子工程"
          },
          {
            "text": "常用操作",
            "collapsed": true,
            "items": [
              {
                "text": "如何开发动态输入算子",
                "link": "/guide/编程指南/附录/常用操作/如何开发动态输入算子"
              },
              {
                "text": "如何在矢量编程时使能Vector Core",
                "link": "/guide/编程指南/附录/常用操作/如何在矢量编程时使能Vector-Core"
              },
              {
                "text": "如何使用workspace",
                "link": "/guide/编程指南/附录/常用操作/如何使用workspace"
              },
              {
                "text": "如何进行Tiling调测",
                "link": "/guide/编程指南/附录/常用操作/如何进行Tiling调测"
              },
              {
                "text": "如何使用Tensor原地操作提升算子性能",
                "link": "/guide/编程指南/附录/常用操作/如何使用Tensor原地操作提升算子性能"
              }
            ]
          },
          {
            "text": "FAQ",
            "collapsed": true,
            "items": [
              {
                "text": "核函数运行验证时算子存在精度问题",
                "link": "/guide/编程指南/附录/FAQ/核函数运行验证时算子存在精度问题"
              },
              {
                "text": "运行验证时AllocTensor/FreeTensor失败",
                "link": "/guide/编程指南/附录/FAQ/运行验证时AllocTensor-FreeTensor失败"
              },
              {
                "text": "kernel侧获取Tiling信息不正确",
                "link": "/guide/编程指南/附录/FAQ/kernel侧获取Tiling信息不正确"
              },
              {
                "text": "Kernel编译时报错\"error: out of jump/jumpc imm range\"",
                "link": "/guide/编程指南/附录/FAQ/Kernel编译时报错-error-out-of-jump-jumpc-imm-range"
              },
              {
                "text": "含有Matmul高阶API的算子精度问题",
                "link": "/guide/编程指南/附录/FAQ/含有Matmul高阶API的算子精度问题"
              },
              {
                "text": "算子工程编译时出现文件名过长报错",
                "link": "/guide/编程指南/附录/FAQ/算子工程编译时出现文件名过长报错"
              },
              {
                "text": "调用算子时出现无法打开config.ini的报错",
                "link": "/guide/编程指南/附录/FAQ/调用算子时出现无法打开config-ini的报错"
              },
              {
                "text": "算子包部署时出现权限不足报错",
                "link": "/guide/编程指南/附录/FAQ/算子包部署时出现权限不足报错"
              }
            ]
          }
        ]
      }
    ]
  },
  {
    "text": "算子实践参考",
    "collapsed": true,
    "items": [
      {
        "text": "本文档组织结构",
        "link": "/guide/算子实践参考/本文档组织结构"
      },
      {
        "text": "异构计算",
        "link": "/guide/算子实践参考/异构计算"
      },
      {
        "text": "SIMD算子实现",
        "collapsed": true,
        "items": [
          {
            "text": "SIMD算子实现",
            "link": "/guide/算子实践参考/SIMD算子实现/SIMD算子实现"
          },
          {
            "text": "概述",
            "link": "/guide/算子实践参考/SIMD算子实现/概述"
          },
          {
            "text": "矢量编程",
            "collapsed": true,
            "items": [
              {
                "text": "矢量编程",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/矢量编程"
              },
              {
                "text": "概述",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/概述"
              },
              {
                "text": "基础矢量算子",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/基础矢量算子"
              },
              {
                "text": "TBuf的使用",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/TBuf的使用"
              },
              {
                "text": "多核&Tiling切分",
                "collapsed": true,
                "items": [
                  {
                    "text": "概述",
                    "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/多核-Tiling切分/概述"
                  },
                  {
                    "text": "多核Tiling",
                    "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/多核-Tiling切分/多核Tiling"
                  },
                  {
                    "text": "尾块Tiling",
                    "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/多核-Tiling切分/尾块Tiling"
                  },
                  {
                    "text": "尾核Tiling",
                    "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/多核-Tiling切分/尾核Tiling"
                  },
                  {
                    "text": "尾核&尾块",
                    "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/多核-Tiling切分/尾核-尾块"
                  }
                ]
              },
              {
                "text": "DoubleBuffer场景",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/DoubleBuffer场景"
              },
              {
                "text": "Broadcast场景",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/Broadcast场景"
              },
              {
                "text": "非对齐场景",
                "link": "/guide/算子实践参考/SIMD算子实现/矢量编程/非对齐场景"
              }
            ]
          },
          {
            "text": "矩阵编程（高阶API）",
            "collapsed": true,
            "items": [
              {
                "text": "矩阵编程（高阶API）",
                "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/矩阵编程（高阶API）"
              },
              {
                "text": "基础知识",
                "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/基础知识"
              },
              {
                "text": "算子实现",
                "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/算子实现"
              },
              {
                "text": "特性场景",
                "collapsed": true,
                "items": [
                  {
                    "text": "Matmul特性介绍",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/Matmul特性介绍"
                  },
                  {
                    "text": "多核对齐切分",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/多核对齐切分"
                  },
                  {
                    "text": "多核非对齐切分",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/多核非对齐切分"
                  },
                  {
                    "text": "异步场景处理",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/异步场景处理"
                  },
                  {
                    "text": "矩阵乘输出的量化/反量化",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵乘输出的量化-反量化"
                  },
                  {
                    "text": "矩阵乘输出的Channel拆分",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵乘输出的Channel拆分"
                  },
                  {
                    "text": "矩阵向量乘",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵向量乘"
                  },
                  {
                    "text": "4:2稀疏矩阵乘",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/4-2稀疏矩阵乘"
                  },
                  {
                    "text": "TSCM输入的矩阵乘",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/TSCM输入的矩阵乘"
                  },
                  {
                    "text": "矩阵乘输出的N方向对齐",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/矩阵乘输出的N方向对齐"
                  },
                  {
                    "text": "单次矩阵乘局部输出",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/单次矩阵乘局部输出"
                  },
                  {
                    "text": "AIC和AIV独立运行机制",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/AIC和AIV独立运行机制"
                  },
                  {
                    "text": "MxMatmul场景",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/MxMatmul场景"
                  },
                  {
                    "text": "Batch Matmul基础功能",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/Batch-Matmul基础功能"
                  },
                  {
                    "text": "Batch Matmul复用Bias矩阵",
                    "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/Batch-Matmul复用Bias矩阵"
                  }
                ]
              }
            ]
          },
          {
            "text": "矩阵编程（基础API）",
            "collapsed": true,
            "items": [
              {
                "text": "耦合模式",
                "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（基础API）/耦合模式"
              },
              {
                "text": "分离模式",
                "link": "/guide/算子实践参考/SIMD算子实现/矩阵编程（基础API）/分离模式"
              }
            ]
          },
          {
            "text": "融合算子编程",
            "collapsed": true,
            "items": [
              {
                "text": "融合算子编程",
                "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/融合算子编程"
              },
              {
                "text": "CV融合",
                "collapsed": true,
                "items": [
                  {
                    "text": "基础知识",
                    "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/CV融合/基础知识"
                  },
                  {
                    "text": "算子实现",
                    "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/CV融合/算子实现"
                  }
                ]
              },
              {
                "text": "通算融合",
                "collapsed": true,
                "items": [
                  {
                    "text": "基础知识",
                    "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/通算融合/基础知识"
                  },
                  {
                    "text": "算子实现",
                    "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/通算融合/算子实现"
                  },
                  {
                    "text": "特性场景",
                    "link": "/guide/算子实践参考/SIMD算子实现/融合算子编程/通算融合/特性场景"
                  }
                ]
              }
            ]
          }
        ]
      },
      {
        "text": "SIMT算子实现",
        "collapsed": true,
        "items": [
          {
            "text": "SIMT算子实现",
            "link": "/guide/算子实践参考/SIMT算子实现/SIMT算子实现"
          },
          {
            "text": "基础知识",
            "link": "/guide/算子实践参考/SIMT算子实现/基础知识"
          },
          {
            "text": "算子实现",
            "link": "/guide/算子实践参考/SIMT算子实现/算子实现"
          }
        ]
      },
      {
        "text": "SIMD与SIMT混合算子实现",
        "collapsed": true,
        "items": [
          {
            "text": "SIMD与SIMT混合算子实现",
            "link": "/guide/算子实践参考/SIMD与SIMT混合算子实现/SIMD与SIMT混合算子实现"
          },
          {
            "text": "基础知识",
            "link": "/guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识"
          },
          {
            "text": "算子实现",
            "link": "/guide/算子实践参考/SIMD与SIMT混合算子实现/算子实现"
          }
        ]
      },
      {
        "text": "功能调试",
        "collapsed": true,
        "items": [
          {
            "text": "功能调试",
            "link": "/guide/算子实践参考/功能调试/功能调试"
          },
          {
            "text": "运行正常",
            "link": "/guide/算子实践参考/功能调试/运行正常"
          },
          {
            "text": "精度正常",
            "link": "/guide/算子实践参考/功能调试/精度正常"
          },
          {
            "text": "算子调试",
            "link": "/guide/算子实践参考/功能调试/算子调试"
          }
        ]
      },
      {
        "text": "性能分析",
        "collapsed": true,
        "items": [
          {
            "text": "性能分析",
            "link": "/guide/算子实践参考/性能分析/性能分析"
          },
          {
            "text": "获取性能数据",
            "link": "/guide/算子实践参考/性能分析/获取性能数据"
          },
          {
            "text": "分析性能数据",
            "link": "/guide/算子实践参考/性能分析/分析性能数据"
          }
        ]
      },
      {
        "text": "SIMD算子性能优化",
        "collapsed": true,
        "items": [
          {
            "text": "SIMD算子性能优化",
            "link": "/guide/算子实践参考/SIMD算子性能优化/SIMD算子性能优化"
          },
          {
            "text": "优化建议总览表",
            "link": "/guide/算子实践参考/SIMD算子性能优化/优化建议总览表"
          },
          {
            "text": "Tiling策略",
            "collapsed": true,
            "items": [
              {
                "text": "Tiling策略",
                "link": "/guide/算子实践参考/SIMD算子性能优化/Tiling策略/Tiling策略"
              },
              {
                "text": "核间负载均衡",
                "link": "/guide/算子实践参考/SIMD算子性能优化/Tiling策略/核间负载均衡"
              }
            ]
          },
          {
            "text": "头尾开销优化",
            "collapsed": true,
            "items": [
              {
                "text": "头尾开销优化",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/头尾开销优化"
              },
              {
                "text": "设置合适的核数和算子Kernel类型",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/设置合适的核数和算子Kernel类型"
              },
              {
                "text": "限制TilingData结构大小",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/限制TilingData结构大小"
              },
              {
                "text": "避免TPipe在对象内创建和初始化",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/避免TPipe在对象内创建和初始化"
              },
              {
                "text": "核函数内删除Workspace相关冗余操作",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/核函数内删除Workspace相关冗余操作"
              },
              {
                "text": "设置DCI编译选项来减少算子尾开销",
                "link": "/guide/算子实践参考/SIMD算子性能优化/头尾开销优化/设置DCI编译选项来减少算子尾开销"
              }
            ]
          },
          {
            "text": "流水编排",
            "collapsed": true,
            "items": [
              {
                "text": "流水编排",
                "link": "/guide/算子实践参考/SIMD算子性能优化/流水编排/流水编排"
              },
              {
                "text": "使能DoubleBuffer",
                "link": "/guide/算子实践参考/SIMD算子性能优化/流水编排/使能DoubleBuffer"
              },
              {
                "text": "使能Iterate或IterateAll异步接口避免AIC/AIV同步依赖",
                "link": "/guide/算子实践参考/SIMD算子性能优化/流水编排/使能Iterate或IterateAll异步接口避免AIC-AIV同步依赖"
              }
            ]
          },
          {
            "text": "内存访问",
            "collapsed": true,
            "items": [
              {
                "text": "内存访问",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/内存访问"
              },
              {
                "text": "尽量一次搬运较大的数据块",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/尽量一次搬运较大的数据块"
              },
              {
                "text": "GM地址尽量512B对齐",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/GM地址尽量512B对齐"
              },
              {
                "text": "高效的使用搬运API",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/高效的使用搬运API"
              },
              {
                "text": "非对齐场景减少无效数据的搬运",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/非对齐场景减少无效数据的搬运"
              },
              {
                "text": "非连续搬运场景减少搬运次数",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/非连续搬运场景减少搬运次数"
              },
              {
                "text": "避免同地址访问",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/避免同地址访问"
              },
              {
                "text": "设置合理的L2 CacheMode",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/设置合理的L2-CacheMode"
              },
              {
                "text": "算子与高阶API共享临时Buffer",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/算子与高阶API共享临时Buffer"
              },
              {
                "text": "纯搬运类算子VECIN和VECOUT建议复用",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/纯搬运类算子VECIN和VECOUT建议复用"
              },
              {
                "text": "通过缩减Tensor ShapeInfo维度，优化栈空间",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/通过缩减Tensor-ShapeInfo维度-优化栈空间"
              },
              {
                "text": "避免UB的bank冲突",
                "collapsed": true,
                "items": [
                  {
                    "text": "避免UB的bank冲突",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免UB的bank冲突"
                  },
                  {
                    "text": "概述",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/概述"
                  },
                  {
                    "text": "避免bank冲突（NPU架构版本2201）",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本2201）"
                  },
                  {
                    "text": "避免bank冲突（NPU架构版本3510）",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本3510）"
                  }
                ]
              },
              {
                "text": "L2 Cache切分",
                "link": "/guide/算子实践参考/SIMD算子性能优化/内存访问/L2-Cache切分"
              }
            ]
          },
          {
            "text": "矢量计算",
            "collapsed": true,
            "items": [
              {
                "text": "通过Unified Buffer融合实现连续vector计算",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/通过Unified-Buffer融合实现连续vector计算"
              },
              {
                "text": "Vector算子灵活运用Counter模式",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/Vector算子灵活运用Counter模式"
              },
              {
                "text": "选择低延迟指令，优化归约操作性能",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/选择低延迟指令-优化归约操作性能"
              },
              {
                "text": "VF性能优化",
                "collapsed": true,
                "items": [
                  {
                    "text": "VF循环优化",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF循环优化"
                  },
                  {
                    "text": "指令双发优化",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/指令双发优化"
                  },
                  {
                    "text": "连续非对齐场景优化",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/连续非对齐场景优化"
                  },
                  {
                    "text": "VF融合优化",
                    "link": "/guide/算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF融合优化"
                  }
                ]
              }
            ]
          },
          {
            "text": "矩阵计算",
            "collapsed": true,
            "items": [
              {
                "text": "通过BT Buffer实现高效的bias计算",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过BT-Buffer实现高效的bias计算"
              },
              {
                "text": "通过FP Buffer存放量化参数实现高效随路量化",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过FP-Buffer存放量化参数实现高效随路量化"
              },
              {
                "text": "通过L0C Buffer数据暂存实现高效的矩阵乘结果累加",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矩阵计算/通过L0C-Buffer数据暂存实现高效的矩阵乘结果累加"
              },
              {
                "text": "较小矩阵长驻L1 Buffer，仅分次搬运较大矩阵",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矩阵计算/较小矩阵长驻L1-Buffer-仅分次搬运较大矩阵"
              },
              {
                "text": "Matmul使能AtomicAdd选项",
                "link": "/guide/算子实践参考/SIMD算子性能优化/矩阵计算/Matmul使能AtomicAdd选项"
              }
            ]
          }
        ]
      },
      {
        "text": "SIMD与SIMT混合算子性能优化",
        "collapsed": true,
        "items": [
          {
            "text": "SIMD与SIMT混合算子性能优化",
            "link": "/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率"
          },
          {
            "text": "内存访问",
            "collapsed": true,
            "items": [
              {
                "text": "使用Unified Buffer提升内存访问效率",
                "link": "/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率"
              }
            ]
          },
          {
            "text": "计算优化",
            "collapsed": true,
            "items": [
              {
                "text": "通过SIMT实现分支判断",
                "link": "/guide/算子实践参考/SIMD与SIMT混合算子性能优化/计算优化/通过SIMT实现分支判断"
              }
            ]
          }
        ]
      },
      {
        "text": "内存访问",
        "collapsed": true,
        "items": [
          {
            "text": "访存合并",
            "link": "/guide/算子实践参考/SIMT算子性能优化/内存访问/访存合并"
          },
          {
            "text": "避免UB的Bank冲突",
            "link": "/guide/算子实践参考/SIMT算子性能优化/内存访问/避免UB的Bank冲突"
          }
        ]
      },
      {
        "text": "优秀实践",
        "collapsed": true,
        "items": [
          {
            "text": "优秀实践",
            "link": "/guide/算子实践参考/优秀实践/优秀实践"
          },
          {
            "text": "FlashAttention算子性能调优案例",
            "link": "/guide/算子实践参考/优秀实践/FlashAttention算子性能调优案例"
          },
          {
            "text": "GroupedMatmul算子性能调优案例",
            "link": "/guide/算子实践参考/优秀实践/GroupedMatmul算子性能调优案例"
          },
          {
            "text": "MC²算子性能调优案例",
            "link": "/guide/算子实践参考/优秀实践/MC-算子性能调优案例"
          },
          {
            "text": "Matmul性能调优案例",
            "collapsed": true,
            "items": [
              {
                "text": "Matmul性能优化策略总览",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul性能优化策略总览"
              },
              {
                "text": "Matmul算子优化Tiling策略",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul算子优化Tiling策略"
              },
              {
                "text": "Matmul高阶API使能纯Cube模式",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能纯Cube模式"
              },
              {
                "text": "Matmul高阶API使能MDL模板",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能MDL模板"
              },
              {
                "text": "Matmul高阶API使能UnitFlag",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能UnitFlag"
              },
              {
                "text": "Matmul高阶API使能Tiling全量常量化",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能Tiling全量常量化"
              },
              {
                "text": "Matmul高阶API使能L2 Cache切分",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能L2-Cache切分"
              },
              {
                "text": "Matmul高阶API使能多核切K",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能多核切K"
              },
              {
                "text": "Matmul高阶API使能多核K轴错峰访问内存",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能多核K轴错峰访问内存"
              },
              {
                "text": "Matmul高阶API使能NBuffer33模板",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能NBuffer33模板"
              },
              {
                "text": "Matmul高阶API使能IBShare模板共享B矩阵数据",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能IBShare模板共享B矩阵数据"
              },
              {
                "text": "Matmul高阶API使能IBShare模板共享A和B矩阵数据",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能IBShare模板共享A和B矩阵数据"
              },
              {
                "text": "AIV核上的ND2NZ格式转换",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/AIV核上的ND2NZ格式转换"
              },
              {
                "text": "Matmul高阶API使能MTE2 Preload",
                "link": "/guide/算子实践参考/优秀实践/Matmul性能调优案例/Matmul高阶API使能MTE2-Preload"
              }
            ]
          },
          {
            "text": "SIMT实现InsertHashTable算子案例",
            "link": "/guide/算子实践参考/优秀实践/SIMT实现InsertHashTable算子案例"
          }
        ]
      }
    ]
  },
  {
    "text": "跨代迁移兼容性指南",
    "collapsed": true,
    "items": [
      {
        "text": "概述",
        "link": "/guide/跨代迁移兼容性指南/概述"
      },
      {
        "text": "Ascend-C-API兼容策略",
        "link": "/guide/跨代迁移兼容性指南/Ascend-C-API兼容策略"
      },
      {
        "text": "3510架构迁移指导",
        "collapsed": true,
        "items": [
          {
            "text": "2201到3510架构变更",
            "link": "/guide/跨代迁移兼容性指南/3510架构迁移指导/2201到3510架构变更"
          },
          {
            "text": "2201迁移3510指导",
            "collapsed": true,
            "items": [
              {
                "text": "基础API迁移指导",
                "link": "/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/基础API迁移指导"
              },
              {
                "text": "高阶API迁移指导",
                "link": "/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/高阶API迁移指导"
              },
              {
                "text": "算子编译迁移指导",
                "link": "/guide/跨代迁移兼容性指南/3510架构迁移指导/2201迁移3510指导/算子编译迁移指导"
              }
            ]
          }
        ]
      }
    ]
  },
  {
    "text": "技术附录",
    "collapsed": true,
    "items": [
      {
        "text": "概念原理和术语",
        "collapsed": true,
        "items": [
          {
            "text": "术语表",
            "link": "/guide/技术附录/概念原理和术语/术语表"
          },
          {
            "text": "神经网络和算子",
            "collapsed": true,
            "items": [
              {
                "text": "算子基本概念",
                "link": "/guide/技术附录/概念原理和术语/神经网络和算子/算子基本概念"
              },
              {
                "text": "数据排布格式",
                "link": "/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式"
              }
            ]
          },
          {
            "text": "内存访问原理",
            "collapsed": true,
            "items": [
              {
                "text": "Scalar读写数据",
                "link": "/guide/技术附录/概念原理和术语/内存访问原理/Scalar读写数据"
              }
            ]
          },
          {
            "text": "性能优化技术原理",
            "collapsed": true,
            "items": [
              {
                "text": "DoubleBuffer",
                "link": "/guide/技术附录/概念原理和术语/性能优化技术原理/DoubleBuffer"
              }
            ]
          }
        ]
      },
      {
        "text": "C++标准支持",
        "collapsed": true,
        "items": [
          {
            "text": "概述",
            "link": "/guide/技术附录/CPP标准支持/概述"
          },
          {
            "text": "C/C++语法限制",
            "collapsed": true,
            "items": [
              {
                "text": "C/C++语法限制",
                "link": "/guide/技术附录/CPP标准支持/语法限制/语法限制"
              },
              {
                "text": "特性",
                "link": "/guide/技术附录/CPP标准支持/语法限制/特性"
              },
              {
                "text": "函数",
                "link": "/guide/技术附录/CPP标准支持/语法限制/函数"
              }
            ]
          }
        ]
      }
    ]
  }
])),


   "/api/": addNumbering(removeSelfRefItems([
    {
      "text": "Ascend C API列表",
      "link": "/api/Ascend-C-API列表"
    },
    {
      "text": "SIMD API",
      "collapsed": true,
      "items": [
        {
          "text": "SIMD API列表",
          "link": "/api/SIMD-API/SIMD-API列表"
        },
        {
          "text": "通用说明和约束",
          "link": "/api/SIMD-API/通用说明和约束"
        },
        {
          "text": "基础数据结构",
          "collapsed": true,
          "items": [
            {
              "text": "LocalTensor",
              "collapsed": true,
              "items": [
                {
                  "text": "LocalTensor简介",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/LocalTensor简介"
                },
                {
                  "text": "LocalTensor构造函数",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/LocalTensor构造函数"
                },
                {
                  "text": "SetValue",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetValue"
                },
                {
                  "text": "GetValue",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetValue"
                },
                {
                  "text": "operator[]",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/operator"
                },
                {
                  "text": "operator()",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/operator()"
                },
                {
                  "text": "SetSize",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetSize"
                },
                {
                  "text": "GetSize",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetSize"
                },
                {
                  "text": "SetUserTag",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetUserTag"
                },
                {
                  "text": "GetUserTag",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetUserTag"
                },
                {
                  "text": "ReinterpretCast",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/ReinterpretCast"
                },
                {
                  "text": "GetPhyAddr",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetPhyAddr"
                },
                {
                  "text": "GetPosition",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetPosition"
                },
                {
                  "text": "GetLength",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetLength"
                },
                {
                  "text": "SetShapeInfo",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetShapeInfo"
                },
                {
                  "text": "GetShapeInfo",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/GetShapeInfo"
                },
                {
                  "text": "SetAddrWithOffset",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetAddrWithOffset"
                },
                {
                  "text": "SetBufferLen",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/SetBufferLen"
                },
                {
                  "text": "ToFile",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/ToFile"
                },
                {
                  "text": "Print",
                  "link": "/api/SIMD-API/基础数据结构/LocalTensor/Print"
                }
              ]
            },
            {
              "text": "GlobalTensor",
              "collapsed": true,
              "items": [
                {
                  "text": "GlobalTensor简介",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GlobalTensor简介"
                },
                {
                  "text": "GlobalTensor构造函数",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GlobalTensor构造函数"
                },
                {
                  "text": "SetGlobalBuffer",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/SetGlobalBuffer"
                },
                {
                  "text": "GetPhyAddr",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GetPhyAddr-0"
                },
                {
                  "text": "GetValue",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GetValue-1"
                },
                {
                  "text": "operator()",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/operator()-2"
                },
                {
                  "text": "SetValue",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/SetValue-3"
                },
                {
                  "text": "GetSize",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GetSize-4"
                },
                {
                  "text": "operator[]",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/operator-5"
                },
                {
                  "text": "SetShapeInfo",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/SetShapeInfo-6"
                },
                {
                  "text": "GetShapeInfo",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/GetShapeInfo-7"
                },
                {
                  "text": "SetL2CacheHint",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/SetL2CacheHint"
                },
                {
                  "text": "ReinterpretCast",
                  "link": "/api/SIMD-API/基础数据结构/GlobalTensor/ReinterpretCast-8"
                }
              ]
            },
            {
              "text": "Coordinate",
              "collapsed": true,
              "items": [
                {
                  "text": "Coordinate简介",
                  "link": "/api/SIMD-API/基础数据结构/Coordinate/Coordinate简介"
                },
                {
                  "text": "MakeCoord",
                  "link": "/api/SIMD-API/基础数据结构/Coordinate/MakeCoord"
                },
                {
                  "text": "Crd2Idx",
                  "link": "/api/SIMD-API/基础数据结构/Coordinate/Crd2Idx"
                }
              ]
            },
            {
              "text": "Layout",
              "collapsed": true,
              "items": [
                {
                  "text": "Layout简介",
                  "link": "/api/SIMD-API/基础数据结构/Layout/Layout简介"
                },
                {
                  "text": "Layout构造函数",
                  "link": "/api/SIMD-API/基础数据结构/Layout/Layout构造函数"
                },
                {
                  "text": "layout",
                  "link": "/api/SIMD-API/基础数据结构/Layout/layout1"
                },
                {
                  "text": "运算符重载",
                  "link": "/api/SIMD-API/基础数据结构/Layout/运算符重载"
                },
                {
                  "text": "GetShape",
                  "link": "/api/SIMD-API/基础数据结构/Layout/GetShape"
                },
                {
                  "text": "GetStride",
                  "link": "/api/SIMD-API/基础数据结构/Layout/GetStride"
                },
                {
                  "text": "MakeShape",
                  "link": "/api/SIMD-API/基础数据结构/Layout/MakeShape"
                },
                {
                  "text": "MakeStride",
                  "link": "/api/SIMD-API/基础数据结构/Layout/MakeStride"
                },
                {
                  "text": "MakeLayout",
                  "link": "/api/SIMD-API/基础数据结构/Layout/MakeLayout"
                },
                {
                  "text": "is_layout",
                  "link": "/api/SIMD-API/基础数据结构/Layout/is_layout"
                }
              ]
            },
            {
              "text": "TensorTrait",
              "collapsed": true,
              "items": [
                {
                  "text": "TensorTrait简介",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/TensorTrait简介"
                },
                {
                  "text": "TensorTrait构造函数",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/TensorTrait构造函数"
                },
                {
                  "text": "GetLayout",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/GetLayout"
                },
                {
                  "text": "SetLayout",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/SetLayout"
                },
                {
                  "text": "MakeTensorTrait",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/MakeTensorTrait"
                },
                {
                  "text": "is_tensorTrait",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/is_tensorTrait"
                },
                {
                  "text": "更多样例",
                  "link": "/api/SIMD-API/基础数据结构/TensorTrait/更多样例"
                }
              ]
            },
            {
              "text": "内置数据类型",
              "link": "/api/SIMD-API/基础数据结构/内置数据类型"
            }
          ]
        },
        {
          "text": "基础API",
          "collapsed": true,
          "items": [
            {
              "text": "Memory数据搬运",
              "collapsed": true,
              "items": [
                {
                  "text": "DataCopy",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "DataCopy简介",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/DataCopy简介"
                    },
                    {
                      "text": "基础数据搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/基础数据搬运"
                    },
                    {
                      "text": "增强数据搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/增强数据搬运"
                    },
                    {
                      "text": "切片数据搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/切片数据搬运"
                    },
                    {
                      "text": "随路转换ND2NZ搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/随路转换ND2NZ搬运"
                    },
                    {
                      "text": "随路转换NZ2ND搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/随路转换NZ2ND搬运"
                    },
                    {
                      "text": "随路转换DN2NZ搬运（ISASI）",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/随路转换DN2NZ搬运（ISASI）"
                    },
                    {
                      "text": "随路量化激活搬运",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/随路量化激活搬运"
                    },
                    {
                      "text": "多维数据搬运（ISASI）",
                      "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopy/多维数据搬运（ISASI）"
                    }
                  ]
                },
                {
                  "text": "DataCopyL1ToUB",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopyL1ToUB"
                },
                {
                  "text": "DataCopyPad(ISASI)",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/DataCopyPad(ISASI)"
                },
                {
                  "text": "Copy",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/Copy"
                },
                {
                  "text": "SetPadValue(ISASI)",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/SetPadValue(ISASI)"
                },
                {
                  "text": "SetLoopModePara",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/SetLoopModePara"
                },
                {
                  "text": "ResetLoopModePara",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/ResetLoopModePara"
                },
                {
                  "text": "BroadCastVecToMM(ISASI)",
                  "link": "/api/SIMD-API/基础API/Memory数据搬运/BroadCastVecToMM(ISASI)"
                }
              ]
            },
            {
              "text": "矩阵计算（ISASI）",
              "collapsed": true,
              "items": [
                {
                  "text": "数据搬运",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Fill",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/Fill"
                    },
                    {
                      "text": "LoadData",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Load2D",
                          "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadData/Load2D"
                        },
                        {
                          "text": "Load2DMX",
                          "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadData/Load2DMX"
                        },
                        {
                          "text": "Load2DBitMode",
                          "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadData/Load2DBitMode"
                        },
                        {
                          "text": "Load3D",
                          "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadData/Load3D"
                        },
                        {
                          "text": "Load3DBitMode",
                          "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadData/Load3DBitMode"
                        }
                      ]
                    },
                    {
                      "text": "LoadUnzipIndex",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadUnzipIndex"
                    },
                    {
                      "text": "LoadDataUnzip",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadDataUnzip"
                    },
                    {
                      "text": "LoadDataWithTranspose",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadDataWithTranspose"
                    },
                    {
                      "text": "LoadDataWithSparse",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadDataWithSparse"
                    },
                    {
                      "text": "SetFmatrix",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFmatrix"
                    },
                    {
                      "text": "SetFmatrixBitMode",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFmatrixBitMode"
                    },
                    {
                      "text": "SetLoadDataBoundary",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetLoadDataBoundary"
                    },
                    {
                      "text": "SetLoadDataRepeat",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetLoadDataRepeat"
                    },
                    {
                      "text": "SetLoadDataPaddingValue",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetLoadDataPaddingValue"
                    },
                    {
                      "text": "Fixpipe",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/Fixpipe"
                    },
                    {
                      "text": "SetFixPipeConfig",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFixPipeConfig"
                    },
                    {
                      "text": "SetFixpipeNz2ndFlag",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFixpipeNz2ndFlag"
                    },
                    {
                      "text": "SetFixpipePreQuantFlag",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFixpipePreQuantFlag"
                    },
                    {
                      "text": "SetFixPipeClipRelu",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFixPipeClipRelu"
                    },
                    {
                      "text": "SetFixPipeAddr",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetFixPipeAddr"
                    },
                    {
                      "text": "SetAippFunctions",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/SetAippFunctions"
                    },
                    {
                      "text": "LoadImageToLocal",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/数据搬运/LoadImageToLocal"
                    }
                  ]
                },
                {
                  "text": "矩阵计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Mmad",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/Mmad"
                    },
                    {
                      "text": "MmadBitMode",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/MmadBitMode"
                    },
                    {
                      "text": "MmadWithSparse",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/MmadWithSparse"
                    },
                    {
                      "text": "SetMMRowMajor",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/SetMMRowMajor"
                    },
                    {
                      "text": "SetMMColumnMajor",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/SetMMColumnMajor"
                    },
                    {
                      "text": "SetHF32Mode",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/SetHF32Mode"
                    },
                    {
                      "text": "SetHF32TransMode",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/SetHF32TransMode"
                    },
                    {
                      "text": "Conv2D（废弃）",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/Conv2D（废弃）"
                    },
                    {
                      "text": "Gemm（废弃）",
                      "link": "/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算/Gemm（废弃）"
                    }
                  ]
                }
              ]
            },
            {
              "text": "Memory矢量计算",
              "collapsed": true,
              "items": [
                {
                  "text": "基础算术",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Exp",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Exp"
                    },
                    {
                      "text": "Ln",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Ln"
                    },
                    {
                      "text": "Abs",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Abs"
                    },
                    {
                      "text": "Reciprocal",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Reciprocal"
                    },
                    {
                      "text": "Sqrt",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Sqrt"
                    },
                    {
                      "text": "Rsqrt",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Rsqrt"
                    },
                    {
                      "text": "Relu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Relu"
                    },
                    {
                      "text": "Neg(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Neg(ISASI)"
                    },
                    {
                      "text": "Add",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Add"
                    },
                    {
                      "text": "Sub",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Sub"
                    },
                    {
                      "text": "Mul",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Mul"
                    },
                    {
                      "text": "Div",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Div"
                    },
                    {
                      "text": "Max",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Max"
                    },
                    {
                      "text": "Min",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Min"
                    },
                    {
                      "text": "BilinearInterpolation(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/BilinearInterpolation(ISASI)"
                    },
                    {
                      "text": "Prelu(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Prelu(ISASI)"
                    },
                    {
                      "text": "Mull(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Mull(ISASI)"
                    },
                    {
                      "text": "Adds",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Adds"
                    },
                    {
                      "text": "Adds（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Adds（灵活标量位置）"
                    },
                    {
                      "text": "Muls",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Muls"
                    },
                    {
                      "text": "Muls（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Muls（灵活标量位置）"
                    },
                    {
                      "text": "Maxs",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Maxs"
                    },
                    {
                      "text": "Maxs（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Maxs（灵活标量位置）"
                    },
                    {
                      "text": "Mins",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Mins"
                    },
                    {
                      "text": "Mins（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Mins（灵活标量位置）"
                    },
                    {
                      "text": "Subs",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Subs"
                    },
                    {
                      "text": "Divs",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/Divs"
                    },
                    {
                      "text": "LeakyRelu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu"
                    },
                    {
                      "text": "更多样例",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/基础算术/更多样例-9"
                    }
                  ]
                },
                {
                  "text": "逻辑计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Not",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/Not"
                    },
                    {
                      "text": "And",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/And"
                    },
                    {
                      "text": "Or",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/Or"
                    },
                    {
                      "text": "Ands",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/Ands"
                    },
                    {
                      "text": "Ors",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/Ors"
                    },
                    {
                      "text": "ShiftLeft",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftLeft"
                    },
                    {
                      "text": "ShiftLeft（左移位数为Tensor）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftLeft（左移位数为Tensor）"
                    },
                    {
                      "text": "ShiftRight",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftRight"
                    },
                    {
                      "text": "ShiftRight（右移位数为Tensor）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftRight（右移位数为Tensor）"
                    }
                  ]
                },
                {
                  "text": "复合计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Axpy",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/Axpy"
                    },
                    {
                      "text": "CastDequant",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/CastDequant"
                    },
                    {
                      "text": "AddRelu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/AddRelu"
                    },
                    {
                      "text": "AddReluCast",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/AddReluCast"
                    },
                    {
                      "text": "AddDeqRelu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/AddDeqRelu"
                    },
                    {
                      "text": "SubRelu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/SubRelu"
                    },
                    {
                      "text": "SubReluCast",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/SubReluCast"
                    },
                    {
                      "text": "MulAddDst",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/MulAddDst"
                    },
                    {
                      "text": "MulCast",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/MulCast"
                    },
                    {
                      "text": "FusedMulAdd",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/FusedMulAdd"
                    },
                    {
                      "text": "MulAddRelu",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/MulAddRelu"
                    },
                    {
                      "text": "AbsSub(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/AbsSub(ISASI)"
                    },
                    {
                      "text": "ExpSub(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/ExpSub(ISASI)"
                    },
                    {
                      "text": "MulsCast(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/复合计算/MulsCast(ISASI)"
                    }
                  ]
                },
                {
                  "text": "比较与选择",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Compare",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Compare"
                    },
                    {
                      "text": "Compare（结果存入寄存器）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Compare（结果存入寄存器）"
                    },
                    {
                      "text": "Compares",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Compares"
                    },
                    {
                      "text": "Compares（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Compares（灵活标量位置）"
                    },
                    {
                      "text": "GetCmpMask(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/GetCmpMask(ISASI)"
                    },
                    {
                      "text": "SetCmpMask(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/SetCmpMask(ISASI)"
                    },
                    {
                      "text": "Select",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Select"
                    },
                    {
                      "text": "Select（灵活标量位置）",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/Select（灵活标量位置）"
                    },
                    {
                      "text": "GatherMask",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/比较与选择/GatherMask"
                    }
                  ]
                },
                {
                  "text": "类型转换",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Cast",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/类型转换/Cast"
                    },
                    {
                      "text": "Truncate(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/类型转换/Truncate(ISASI)"
                    }
                  ]
                },
                {
                  "text": "归约计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceMax",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMax"
                    },
                    {
                      "text": "ReduceMin",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMin"
                    },
                    {
                      "text": "ReduceSum",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceSum"
                    },
                    {
                      "text": "WholeReduceMax",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/WholeReduceMax"
                    },
                    {
                      "text": "WholeReduceMin",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/WholeReduceMin"
                    },
                    {
                      "text": "WholeReduceSum",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/WholeReduceSum"
                    },
                    {
                      "text": "BlockReduceMax",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/BlockReduceMax"
                    },
                    {
                      "text": "BlockReduceMin",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/BlockReduceMin"
                    },
                    {
                      "text": "BlockReduceSum",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/BlockReduceSum"
                    },
                    {
                      "text": "PairReduceSum",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/PairReduceSum"
                    },
                    {
                      "text": "RepeatReduceSum",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/RepeatReduceSum"
                    },
                    {
                      "text": "GetReduceRepeatSumSpr(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/GetReduceRepeatSumSpr(ISASI)"
                    },
                    {
                      "text": "GetReduceRepeatMaxMinSpr(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/归约计算/GetReduceRepeatMaxMinSpr(ISASI)"
                    }
                  ]
                },
                {
                  "text": "数据转换",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Transpose",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据转换/Transpose"
                    },
                    {
                      "text": "TransDataTo5HD",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据转换/TransDataTo5HD"
                    }
                  ]
                },
                {
                  "text": "数据填充",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Duplicate",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据填充/Duplicate"
                    },
                    {
                      "text": "Brcb",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据填充/Brcb"
                    },
                    {
                      "text": "CreateVecIndex",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据填充/CreateVecIndex"
                    },
                    {
                      "text": "VectorPadding(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据填充/VectorPadding(ISASI)"
                    }
                  ]
                },
                {
                  "text": "排序组合（ISASI）",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ProposalConcat",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/ProposalConcat"
                    },
                    {
                      "text": "ProposalExtract",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/ProposalExtract"
                    },
                    {
                      "text": "RpSort16",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/RpSort16"
                    },
                    {
                      "text": "MrgSort4",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/MrgSort4"
                    },
                    {
                      "text": "Sort32",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/Sort32"
                    },
                    {
                      "text": "MrgSort",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/MrgSort"
                    },
                    {
                      "text": "GetMrgSortResult",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/GetMrgSortResult"
                    }
                  ]
                },
                {
                  "text": "离散与聚合",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Gather",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/离散与聚合/Gather"
                    },
                    {
                      "text": "Gatherb(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/离散与聚合/Gatherb(ISASI)"
                    },
                    {
                      "text": "Scatter(ISASI)",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/离散与聚合/Scatter(ISASI)"
                    }
                  ]
                },
                {
                  "text": "掩码操作",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SetMaskCount",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskCount"
                    },
                    {
                      "text": "SetMaskNorm",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskNorm"
                    },
                    {
                      "text": "SetVectorMask",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/掩码操作/SetVectorMask"
                    },
                    {
                      "text": "ResetMask",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/掩码操作/ResetMask"
                    }
                  ]
                },
                {
                  "text": "量化设置",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SetDeqScale",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/量化设置/SetDeqScale"
                    }
                  ]
                },
                {
                  "text": "数据重排（ISASI）",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Interleave",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据重排（ISASI）/Interleave"
                    },
                    {
                      "text": "DeInterleave",
                      "link": "/api/SIMD-API/基础API/Memory矢量计算/数据重排（ISASI）/DeInterleave"
                    }
                  ]
                }
              ]
            },
            {
              "text": "Reg矢量计算",
              "collapsed": true,
              "items": [
                {
                  "text": "寄存器数据类型",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "RegTensor",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/RegTensor"
                    },
                    {
                      "text": "MaskReg",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg"
                    },
                    {
                      "text": "UnalignRegForLoad & UnalignRegForStore",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/UnalignRegForLoad-UnalignRegForStore"
                    },
                    {
                      "text": "AddrReg",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/AddrReg"
                    }
                  ]
                },
                {
                  "text": "asc_vf_call",
                  "link": "/api/SIMD-API/基础API/Reg矢量计算/asc_vf_call"
                },
                {
                  "text": "Reg数据搬运",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "连续对齐搬入",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/连续对齐搬入"
                    },
                    {
                      "text": "连续对齐搬出",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/连续对齐搬出"
                    },
                    {
                      "text": "非连续对齐搬入",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/非连续对齐搬入"
                    },
                    {
                      "text": "非连续对齐搬出",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/非连续对齐搬出"
                    },
                    {
                      "text": "连续非对齐搬入",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/连续非对齐搬入"
                    },
                    {
                      "text": "连续非对齐搬出",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/连续非对齐搬出"
                    },
                    {
                      "text": "MaskReg搬入",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/MaskReg搬入"
                    },
                    {
                      "text": "MaskReg搬出",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/MaskReg搬出"
                    },
                    {
                      "text": "Move",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬运/Move"
                    }
                  ]
                },
                {
                  "text": "MaskReg计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Move",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/Move-10"
                    },
                    {
                      "text": "Interleave",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/Interleave-11"
                    },
                    {
                      "text": "DeInterleave",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/DeInterleave-12"
                    },
                    {
                      "text": "Select",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/Select-13"
                    },
                    {
                      "text": "Pack",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/Pack"
                    },
                    {
                      "text": "UnPack",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/UnPack"
                    },
                    {
                      "text": "MoveMask",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/MaskReg计算/MoveMask"
                    }
                  ]
                },
                {
                  "text": "基础算术",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Abs",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Abs-15"
                    },
                    {
                      "text": "Relu",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Relu-16"
                    },
                    {
                      "text": "Exp",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Exp-17"
                    },
                    {
                      "text": "Sqrt",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Sqrt-18"
                    },
                    {
                      "text": "Ln",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Ln-19"
                    },
                    {
                      "text": "Log",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Log"
                    },
                    {
                      "text": "Log2",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Log2"
                    },
                    {
                      "text": "Log10",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Log10"
                    },
                    {
                      "text": "Neg",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Neg"
                    },
                    {
                      "text": "Add",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Add-20"
                    },
                    {
                      "text": "Sub",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Sub-21"
                    },
                    {
                      "text": "Mul",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Mul-22"
                    },
                    {
                      "text": "Div",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Div-23"
                    },
                    {
                      "text": "Max",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Max-24"
                    },
                    {
                      "text": "Min",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Min-25"
                    },
                    {
                      "text": "Mull",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Mull"
                    },
                    {
                      "text": "AddC",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/AddC"
                    },
                    {
                      "text": "SubC",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/SubC"
                    },
                    {
                      "text": "Prelu",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Prelu"
                    },
                    {
                      "text": "Adds",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Adds-26"
                    },
                    {
                      "text": "Muls",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Muls-27"
                    },
                    {
                      "text": "Maxs",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Maxs-28"
                    },
                    {
                      "text": "Mins",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/Mins-29"
                    },
                    {
                      "text": "LeakyRelu",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/基础算术/LeakyRelu-30"
                    }
                  ]
                },
                {
                  "text": "逻辑计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Not",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/Not-32"
                    },
                    {
                      "text": "And",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/And-33"
                    },
                    {
                      "text": "Or",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/Or-34"
                    },
                    {
                      "text": "Xor",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/Xor"
                    },
                    {
                      "text": "ShiftLeft",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/ShiftLeft-35"
                    },
                    {
                      "text": "ShiftRight",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/ShiftRight-36"
                    },
                    {
                      "text": "ShiftLefts",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/ShiftLefts"
                    },
                    {
                      "text": "ShiftRights",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/逻辑计算/ShiftRights"
                    }
                  ]
                },
                {
                  "text": "复合计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Axpy",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/Axpy-38"
                    },
                    {
                      "text": "AbsSub",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/AbsSub"
                    },
                    {
                      "text": "ExpSub",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/ExpSub"
                    },
                    {
                      "text": "MulDstAdd",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/MulDstAdd"
                    },
                    {
                      "text": "MulAddDst",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/MulAddDst-39"
                    },
                    {
                      "text": "MulsCast",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/复合计算/MulsCast"
                    }
                  ]
                },
                {
                  "text": "比较与选择",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Compare",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/比较与选择/Compare-41"
                    },
                    {
                      "text": "Compares",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/比较与选择/Compares-42"
                    },
                    {
                      "text": "Select",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/比较与选择/Select-43"
                    },
                    {
                      "text": "Squeeze",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/比较与选择/Squeeze"
                    }
                  ]
                },
                {
                  "text": "类型转换",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Cast",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/类型转换/Cast-45"
                    },
                    {
                      "text": "Truncate",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/类型转换/Truncate"
                    }
                  ]
                },
                {
                  "text": "归约计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Reduce",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/归约计算/Reduce"
                    },
                    {
                      "text": "ReduceDataBlock",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/归约计算/ReduceDataBlock"
                    },
                    {
                      "text": "PairReduceElem",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/归约计算/PairReduceElem"
                    }
                  ]
                },
                {
                  "text": "数据填充",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Duplicate",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据填充/Duplicate-48"
                    }
                  ]
                },
                {
                  "text": "离散与聚合",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Gather",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/离散与聚合/Gather-50"
                    },
                    {
                      "text": "Gather（支持寄存器为源操作数）",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/离散与聚合/Gather（支持寄存器为源操作数）"
                    },
                    {
                      "text": "GatherB",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/离散与聚合/GatherB"
                    },
                    {
                      "text": "Scatter",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/离散与聚合/Scatter"
                    }
                  ]
                },
                {
                  "text": "数据重排",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Interleave",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据重排/Interleave-51"
                    },
                    {
                      "text": "DeInterleave",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据重排/DeInterleave-52"
                    }
                  ]
                },
                {
                  "text": "数据压缩",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Unsqueeze",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据压缩/Unsqueeze"
                    },
                    {
                      "text": "Pack",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据压缩/Pack-53"
                    },
                    {
                      "text": "UnPack",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据压缩/UnPack-54"
                    }
                  ]
                },
                {
                  "text": "直方图计算",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Histograms",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/直方图计算/Histograms"
                    }
                  ]
                },
                {
                  "text": "索引操作",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Arange",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/索引操作/Arange"
                    }
                  ]
                },
                {
                  "text": "同步控制",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LocalMemBar",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/同步控制/LocalMemBar"
                    }
                  ]
                },
                {
                  "text": "系统变量访问",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "GetSpr",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/系统变量访问/GetSpr"
                    },
                    {
                      "text": "ClearSpr",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/系统变量访问/ClearSpr"
                    }
                  ]
                },
                {
                  "text": "数据类型",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "PostLiteral",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/PostLiteral"
                    },
                    {
                      "text": "RegLayout",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/RegLayout"
                    },
                    {
                      "text": "CastTrait",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/CastTrait"
                    },
                    {
                      "text": "SatMode",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/SatMode"
                    },
                    {
                      "text": "MaskMergeMode",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/MaskMergeMode"
                    },
                    {
                      "text": "RoundMode",
                      "link": "/api/SIMD-API/基础API/Reg矢量计算/数据类型/RoundMode"
                    }
                  ]
                }
              ]
            },
            {
              "text": "标量计算",
              "collapsed": true,
              "items": [
                {
                  "text": "GetBitCount",
                  "link": "/api/SIMD-API/基础API/标量计算/GetBitCount"
                },
                {
                  "text": "CountLeadingZero",
                  "link": "/api/SIMD-API/基础API/标量计算/CountLeadingZero"
                },
                {
                  "text": "CountBitsCntSameAsSignBit",
                  "link": "/api/SIMD-API/基础API/标量计算/CountBitsCntSameAsSignBit"
                },
                {
                  "text": "GetSFFValue",
                  "link": "/api/SIMD-API/基础API/标量计算/GetSFFValue"
                },
                {
                  "text": "Cast（float转half/int32_t）",
                  "link": "/api/SIMD-API/基础API/标量计算/Cast（float转half-int32_t）"
                },
                {
                  "text": "Cast（float转bfloat16_t）",
                  "link": "/api/SIMD-API/基础API/标量计算/Cast（float转bfloat16_t）"
                },
                {
                  "text": "Cast（多类型转float）",
                  "link": "/api/SIMD-API/基础API/标量计算/Cast（多类型转float）"
                },
                {
                  "text": "Nop",
                  "link": "/api/SIMD-API/基础API/标量计算/Nop"
                },
                {
                  "text": "WriteGmByPassDCache(ISASI)",
                  "link": "/api/SIMD-API/基础API/标量计算/WriteGmByPassDCache(ISASI)"
                },
                {
                  "text": "ReadGmByPassDCache(ISASI)",
                  "link": "/api/SIMD-API/基础API/标量计算/ReadGmByPassDCache(ISASI)"
                }
              ]
            },
            {
              "text": "资源管理",
              "collapsed": true,
              "items": [
                {
                  "text": "Pipe和Que框架",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "TPipe",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TPipe简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/TPipe简介"
                        },
                        {
                          "text": "TPipe构造函数",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/TPipe构造函数"
                        },
                        {
                          "text": "Init",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/Init"
                        },
                        {
                          "text": "Destroy",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/Destroy"
                        },
                        {
                          "text": "DestroyWithoutPipeAll",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/DestroyWithoutPipeAll"
                        },
                        {
                          "text": "InitBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/InitBuffer"
                        },
                        {
                          "text": "Reset",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/Reset"
                        },
                        {
                          "text": "AllocEventID",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/AllocEventID"
                        },
                        {
                          "text": "ReleaseEventID",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/ReleaseEventID"
                        },
                        {
                          "text": "FetchEventID",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/FetchEventID"
                        },
                        {
                          "text": "GetBaseAddr",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/GetBaseAddr"
                        },
                        {
                          "text": "InitBufPool",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/InitBufPool"
                        },
                        {
                          "text": "InitSpmBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/InitSpmBuffer"
                        },
                        {
                          "text": "WriteSpmBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/WriteSpmBuffer"
                        },
                        {
                          "text": "ReadSpmBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TPipe/ReadSpmBuffer"
                        }
                      ]
                    },
                    {
                      "text": "GetTPipePtr",
                      "link": "/api/SIMD-API/基础API/资源管理/GetTPipePtr"
                    },
                    {
                      "text": "TBufPool",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TBufPool简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TBufPool/TBufPool简介"
                        },
                        {
                          "text": "TBufPool构造函数",
                          "link": "/api/SIMD-API/基础API/资源管理/TBufPool/TBufPool构造函数"
                        },
                        {
                          "text": "InitBufPool",
                          "link": "/api/SIMD-API/基础API/资源管理/TBufPool/InitBufPool-55"
                        },
                        {
                          "text": "InitBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TBufPool/InitBuffer-56"
                        },
                        {
                          "text": "Reset",
                          "link": "/api/SIMD-API/基础API/资源管理/TBufPool/Reset-57"
                        }
                      ]
                    },
                    {
                      "text": "自定义TBufPool",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "EXTERN_IMPL_BUFPOOL宏",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/EXTERN_IMPL_BUFPOOL宏"
                        },
                        {
                          "text": "Reset",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/Reset-58"
                        },
                        {
                          "text": "Init",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/Init-59"
                        },
                        {
                          "text": "GetBufHandle",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/GetBufHandle"
                        },
                        {
                          "text": "SetCurAddr",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/SetCurAddr"
                        },
                        {
                          "text": "GetCurAddr",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/GetCurAddr"
                        },
                        {
                          "text": "SetCurBufSize",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/SetCurBufSize"
                        },
                        {
                          "text": "GetCurBufSize",
                          "link": "/api/SIMD-API/基础API/资源管理/自定义TBufPool/GetCurBufSize"
                        }
                      ]
                    },
                    {
                      "text": "TQue",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TQue简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/TQue简介"
                        },
                        {
                          "text": "AllocTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/AllocTensor"
                        },
                        {
                          "text": "FreeTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/FreeTensor"
                        },
                        {
                          "text": "EnQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/EnQue"
                        },
                        {
                          "text": "DeQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/DeQue"
                        },
                        {
                          "text": "VacantInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/VacantInQue"
                        },
                        {
                          "text": "HasTensorInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/HasTensorInQue"
                        },
                        {
                          "text": "GetTensorCountInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/GetTensorCountInQue"
                        },
                        {
                          "text": "HasIdleBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TQue/HasIdleBuffer"
                        }
                      ]
                    },
                    {
                      "text": "TSCM",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TSCM简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TSCM/TSCM简介"
                        },
                        {
                          "text": "AllocTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TSCM/AllocTensor-60"
                        },
                        {
                          "text": "FreeTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TSCM/FreeTensor-61"
                        },
                        {
                          "text": "EnQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TSCM/EnQue-62"
                        },
                        {
                          "text": "DeQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TSCM/DeQue-63"
                        }
                      ]
                    },
                    {
                      "text": "TQueBind",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TQueBind简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/TQueBind简介"
                        },
                        {
                          "text": "TQueBind构造函数",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/TQueBind构造函数"
                        },
                        {
                          "text": "AllocTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/AllocTensor-64"
                        },
                        {
                          "text": "FreeTensor",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/FreeTensor-65"
                        },
                        {
                          "text": "EnQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/EnQue-66"
                        },
                        {
                          "text": "DeQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/DeQue-67"
                        },
                        {
                          "text": "VacantInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/VacantInQue-68"
                        },
                        {
                          "text": "HasTensorInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/HasTensorInQue-69"
                        },
                        {
                          "text": "GetTensorCountInQue",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/GetTensorCountInQue-70"
                        },
                        {
                          "text": "HasIdleBuffer",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/HasIdleBuffer-71"
                        },
                        {
                          "text": "FreeAllEvent",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/FreeAllEvent"
                        },
                        {
                          "text": "InitBufHandle",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/InitBufHandle"
                        },
                        {
                          "text": "InitStartBufHandle",
                          "link": "/api/SIMD-API/基础API/资源管理/TQueBind/InitStartBufHandle"
                        }
                      ]
                    },
                    {
                      "text": "TBuf",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "TBuf简介",
                          "link": "/api/SIMD-API/基础API/资源管理/TBuf/TBuf简介"
                        },
                        {
                          "text": "TBuf构造函数",
                          "link": "/api/SIMD-API/基础API/资源管理/TBuf/TBuf构造函数"
                        },
                        {
                          "text": "Get",
                          "link": "/api/SIMD-API/基础API/资源管理/TBuf/Get"
                        },
                        {
                          "text": "GetWithOffset",
                          "link": "/api/SIMD-API/基础API/资源管理/TBuf/GetWithOffset"
                        }
                      ]
                    }
                  ]
                },
                {
                  "text": "临时空间管理",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "workspace",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "GetSysWorkSpacePtr",
                          "link": "/api/SIMD-API/基础API/资源管理/workspace/GetSysWorkSpacePtr"
                        },
                        {
                          "text": "SetSysWorkSpace",
                          "link": "/api/SIMD-API/基础API/资源管理/workspace/SetSysWorkSpace"
                        },
                        {
                          "text": "GetUserWorkspace",
                          "link": "/api/SIMD-API/基础API/资源管理/workspace/GetUserWorkspace"
                        }
                      ]
                    },
                    {
                      "text": "PopStackBuffer",
                      "link": "/api/SIMD-API/基础API/资源管理/PopStackBuffer"
                    }
                  ]
                },
                {
                  "text": "内存管理",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LocalMemAllocator",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "LocalMemAllocator简介",
                          "link": "/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介"
                        },
                        {
                          "text": "LocalMemAllocator构造函数",
                          "link": "/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator构造函数"
                        },
                        {
                          "text": "GetCurAddr",
                          "link": "/api/SIMD-API/基础API/资源管理/LocalMemAllocator/GetCurAddr-72"
                        },
                        {
                          "text": "Alloc",
                          "link": "/api/SIMD-API/基础API/资源管理/LocalMemAllocator/Alloc"
                        }
                      ]
                    }
                  ]
                }
              ]
            },
            {
              "text": "同步控制",
              "collapsed": true,
              "items": [
                {
                  "text": "核内同步",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "同步控制简介",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/同步控制简介"
                    },
                    {
                      "text": "TQueSync模板参数",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/TQueSync模板参数"
                    },
                    {
                      "text": "SetFlag/WaitFlag",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag"
                    }
                    {
                      "text": "SetFlag/WaitFlag(ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI)"
                    },
                    {
                      "text": "PipeBarrier(ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI)"
                    },
                    {
                      "text": "DataSyncBarrier(ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/DataSyncBarrier(ISASI)"
                    },
                    {
                      "text": "Mutex（ISASI）",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Mutex (ISASI)",
                          "link": "/api/SIMD-API/基础API/同步控制/核内同步/Mutex_ISASI"
                        },
                        {
                          "text": "Lock",
                          "link": "/api/SIMD-API/基础API/同步控制/核内同步/Lock"
                        },
                        {
                          "text": "Unlock",
                          "link": "/api/SIMD-API/基础API/同步控制/核内同步/Unlock"
                        }
                      ]
                    },
                    {
                      "text": "AllocMutexID (ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/AllocMutexID-(ISASI)"
                    },
                    {
                      "text": "ReleaseMutexID (ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核内同步/ReleaseMutexID-(ISASI)"
                    }
                  ]
                },
                {
                  "text": "核间同步",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "IBSet",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/IBSet"
                    },
                    {
                      "text": "IBWait",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/IBWait"
                    },
                    {
                      "text": "SyncAll",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/SyncAll"
                    },
                    {
                      "text": "CrossCoreSetFlag(ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI)"
                    },
                    {
                      "text": "CrossCoreWaitFlag(ISASI)",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI)"
                    },
                    {
                      "text": "InitDetermineComputeWorkspace",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/InitDetermineComputeWorkspace"
                    },
                    {
                      "text": "NotifyNextBlock",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/NotifyNextBlock"
                    },
                    {
                      "text": "WaitPreBlock",
                      "link": "/api/SIMD-API/基础API/同步控制/核间同步/WaitPreBlock"
                    }
                  ]
                },
                {
                  "text": "任务间同步",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SetNextTaskStart",
                      "link": "/api/SIMD-API/基础API/同步控制/任务间同步/SetNextTaskStart"
                    },
                    {
                      "text": "WaitPreTaskEnd",
                      "link": "/api/SIMD-API/基础API/同步控制/任务间同步/WaitPreTaskEnd"
                    }
                  ]
                }
              ]
            },
            {
              "text": "缓存控制",
              "collapsed": true,
              "items": [
                {
                  "text": "DataCachePreload",
                  "link": "/api/SIMD-API/基础API/缓存控制/DataCachePreload"
                },
                {
                  "text": "DataCacheCleanAndInvalid",
                  "link": "/api/SIMD-API/基础API/缓存控制/DataCacheCleanAndInvalid"
                },
                {
                  "text": "ICachePreLoad(ISASI)",
                  "link": "/api/SIMD-API/基础API/缓存控制/ICachePreLoad(ISASI)"
                },
                {
                  "text": "GetICachePreloadStatus(ISASI)",
                  "link": "/api/SIMD-API/基础API/缓存控制/GetICachePreloadStatus(ISASI)"
                }
              ]
            },
            {
              "text": "系统变量访问",
              "collapsed": true,
              "items": [
                {
                  "text": "GetBlockNum",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetBlockNum"
                },
                {
                  "text": "GetBlockIdx",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetBlockIdx"
                },
                {
                  "text": "GetDataBlockSizeInBytes",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetDataBlockSizeInBytes"
                },
                {
                  "text": "GetArchVersion",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetArchVersion"
                },
                {
                  "text": "InitSocState",
                  "link": "/api/SIMD-API/基础API/系统变量访问/InitSocState"
                },
                {
                  "text": "GetProgramCounter(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetProgramCounter(ISASI)"
                },
                {
                  "text": "GetSubBlockNum(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetSubBlockNum(ISASI)"
                },
                {
                  "text": "GetSubBlockIdx(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetSubBlockIdx(ISASI)"
                },
                {
                  "text": "GetSystemCycle(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetSystemCycle(ISASI)"
                },
                {
                  "text": "SetCtrlSpr(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/SetCtrlSpr(ISASI)"
                },
                {
                  "text": "GetCtrlSpr(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/GetCtrlSpr(ISASI)"
                },
                {
                  "text": "ResetCtrlSpr(ISASI)",
                  "link": "/api/SIMD-API/基础API/系统变量访问/ResetCtrlSpr(ISASI)"
                }
              ]
            },
            {
              "text": "原子操作",
              "collapsed": true,
              "items": [
                {
                  "text": "SetAtomicAdd",
                  "link": "/api/SIMD-API/基础API/原子操作/SetAtomicAdd"
                },
                {
                  "text": "SetAtomicType",
                  "link": "/api/SIMD-API/基础API/原子操作/SetAtomicType"
                },
                {
                  "text": "DisableDmaAtomic",
                  "link": "/api/SIMD-API/基础API/原子操作/DisableDmaAtomic"
                },
                {
                  "text": "SetAtomicMax(ISASI)",
                  "link": "/api/SIMD-API/基础API/原子操作/SetAtomicMax(ISASI)"
                },
                {
                  "text": "SetAtomicMin(ISASI)",
                  "link": "/api/SIMD-API/基础API/原子操作/SetAtomicMin(ISASI)"
                },
                {
                  "text": "SetStoreAtomicConfig(ISASI)",
                  "link": "/api/SIMD-API/基础API/原子操作/SetStoreAtomicConfig(ISASI)"
                },
                {
                  "text": "GetStoreAtomicConfig(ISASI)",
                  "link": "/api/SIMD-API/基础API/原子操作/GetStoreAtomicConfig(ISASI)"
                },
                {
                  "text": "AtomicAdd",
                  "link": "/api/SIMD-API/基础API/原子操作/AtomicAdd"
                },
                {
                  "text": "AtomicMin",
                  "link": "/api/SIMD-API/基础API/原子操作/AtomicMin"
                },
                {
                  "text": "AtomicMax",
                  "link": "/api/SIMD-API/基础API/原子操作/AtomicMax"
                },
                {
                  "text": "AtomicCas",
                  "link": "/api/SIMD-API/基础API/原子操作/AtomicCas"
                },
                {
                  "text": "AtomicExch",
                  "link": "/api/SIMD-API/基础API/原子操作/AtomicExch"
                }
              ]
            },
            {
              "text": "调试接口",
              "collapsed": true,
              "items": [
                {
                  "text": "上板打印",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "DumpTensor",
                      "link": "/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor"
                    },
                    {
                      "text": "printf",
                      "link": "/api/SIMD-API/基础API/调试接口/上板打印/printf"
                    },
                    {
                      "text": "DumpAccChkPoint",
                      "link": "/api/SIMD-API/基础API/调试接口/上板打印/DumpAccChkPoint"
                    },
                    {
                      "text": "PrintTimeStamp",
                      "link": "/api/SIMD-API/基础API/调试接口/上板打印/PrintTimeStamp"
                    }
                  ]
                },
                {
                  "text": "异常检测",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ascendc_assert",
                      "link": "/api/SIMD-API/基础API/调试接口/异常检测/ascendc_assert"
                    },
                    {
                      "text": "assert",
                      "link": "/api/SIMD-API/基础API/调试接口/异常检测/assert"
                    },
                    {
                      "text": "Trap",
                      "link": "/api/SIMD-API/基础API/调试接口/异常检测/Trap"
                    },
                    {
                      "text": "CheckLocalMemoryIA(ISASI)",
                      "link": "/api/SIMD-API/基础API/调试接口/异常检测/CheckLocalMemoryIA(ISASI)"
                    }
                  ]
                },
                {
                  "text": "CPU孪生调试",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "GmAlloc",
                      "link": "/api/SIMD-API/基础API/调试接口/CPU孪生调试/GmAlloc"
                    },
                    {
                      "text": "ICPU_RUN_KF",
                      "link": "/api/SIMD-API/基础API/调试接口/CPU孪生调试/ICPU_RUN_KF"
                    },
                    {
                      "text": "ICPU_SET_TILING_KEY",
                      "link": "/api/SIMD-API/基础API/调试接口/CPU孪生调试/ICPU_SET_TILING_KEY"
                    },
                    {
                      "text": "GmFree",
                      "link": "/api/SIMD-API/基础API/调试接口/CPU孪生调试/GmFree"
                    },
                    {
                      "text": "SetKernelMode",
                      "link": "/api/SIMD-API/基础API/调试接口/CPU孪生调试/SetKernelMode"
                    }
                  ]
                },
                {
                  "text": "性能统计",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "MetricsProfStart",
                      "link": "/api/SIMD-API/基础API/调试接口/性能统计/MetricsProfStart"
                    },
                    {
                      "text": "MetricsProfStop",
                      "link": "/api/SIMD-API/基础API/调试接口/性能统计/MetricsProfStop"
                    },
                    {
                      "text": "MarkStamp",
                      "link": "/api/SIMD-API/基础API/调试接口/性能统计/MarkStamp"
                    }
                  ]
                }
              ]
            },
            {
              "text": "Cube分组管理（ISASI）",
              "collapsed": true,
              "items": [
                {
                  "text": "CubeResGroupHandle",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "CubeResGroupHandle使用说明",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/CubeResGroupHandle使用说明"
                    },
                    {
                      "text": "CubeResGroupHandle构造函数",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/CubeResGroupHandle构造函数"
                    },
                    {
                      "text": "CreateCubeResGroup",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/CreateCubeResGroup"
                    },
                    {
                      "text": "AssignQueue",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/AssignQueue"
                    },
                    {
                      "text": "AllocMessage",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/AllocMessage"
                    },
                    {
                      "text": "PostMessage",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/PostMessage"
                    },
                    {
                      "text": "PostFakeMsg",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/PostFakeMsg"
                    },
                    {
                      "text": "SetQuit",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/SetQuit"
                    },
                    {
                      "text": "Wait",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/Wait"
                    },
                    {
                      "text": "FreeMessage",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/FreeMessage"
                    },
                    {
                      "text": "SetSkipMsg",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/CubeResGroupHandle/SetSkipMsg"
                    }
                  ]
                },
                {
                  "text": "GroupBarrier",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "GroupBarrier使用说明",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/GroupBarrier/GroupBarrier使用说明"
                    },
                    {
                      "text": "GroupBarrier构造函数",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/GroupBarrier/GroupBarrier构造函数"
                    },
                    {
                      "text": "Arrive",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/GroupBarrier/Arrive"
                    },
                    {
                      "text": "Wait",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/GroupBarrier/Wait-75"
                    },
                    {
                      "text": "GetWorkspaceLen",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/GroupBarrier/GetWorkspaceLen"
                    }
                  ]
                },
                {
                  "text": "KfcWorkspace",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "构造函数与析构函数",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/KfcWorkspace/构造函数与析构函数"
                    },
                    {
                      "text": "UpdateKfcWorkspace",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/KfcWorkspace/UpdateKfcWorkspace"
                    },
                    {
                      "text": "GetKfcWorkspace",
                      "link": "/api/SIMD-API/基础API/Cube分组管理（ISASI）/KfcWorkspace/GetKfcWorkspace"
                    }
                  ]
                }
              ]
            },
            {
              "text": "工具函数",
              "collapsed": true,
              "items": [
                {
                  "text": "NumericLimits",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "简介",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/简介-76"
                    },
                    {
                      "text": "Max",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/Max-77"
                    },
                    {
                      "text": "Lowest",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/Lowest"
                    },
                    {
                      "text": "Min",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/Min-78"
                    },
                    {
                      "text": "Infinity",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/Infinity"
                    },
                    {
                      "text": "NegativeInfinity",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/NegativeInfinity"
                    },
                    {
                      "text": "QuietNaN",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/QuietNaN"
                    },
                    {
                      "text": "SignalingNaN",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/SignalingNaN"
                    },
                    {
                      "text": "DeNormMin",
                      "link": "/api/SIMD-API/基础API/工具函数/NumericLimits/DeNormMin"
                    }
                  ]
                },
                {
                  "text": "Async",
                  "link": "/api/SIMD-API/基础API/工具函数/Async"
                },
                {
                  "text": "GetTaskRatio",
                  "link": "/api/SIMD-API/基础API/工具函数/GetTaskRatio"
                },
                {
                  "text": "GetUBSizeInBytes",
                  "link": "/api/SIMD-API/基础API/工具函数/GetUBSizeInBytes"
                },
                {
                  "text": "GetRuntimeUBSize",
                  "link": "/api/SIMD-API/基础API/工具函数/GetRuntimeUBSize"
                },
                {
                  "text": "GetVecLen",
                  "link": "/api/SIMD-API/基础API/工具函数/GetVecLen"
                },
                {
                  "text": "CeilDivision",
                  "link": "/api/SIMD-API/基础API/工具函数/CeilDivision"
                },
                {
                  "text": "GetSsbufBaseAddr",
                  "link": "/api/SIMD-API/基础API/工具函数/GetSsbufBaseAddr"
                }
              ]
            },
            {
              "text": "Kernel Tiling",
              "collapsed": true,
              "items": [
                {
                  "text": "GET_TILING_DATA",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/GET_TILING_DATA"
                },
                {
                  "text": "GET_TILING_DATA_WITH_STRUCT",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT"
                },
                {
                  "text": "GET_TILING_DATA_MEMBER",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/GET_TILING_DATA_MEMBER"
                },
                {
                  "text": "GET_TILING_DATA_PTR_WITH_STRUCT",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT"
                },
                {
                  "text": "COPY_TILING_WITH_STRUCT",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/COPY_TILING_WITH_STRUCT"
                },
                {
                  "text": "COPY_TILING_WITH_ARRAY",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/COPY_TILING_WITH_ARRAY"
                },
                {
                  "text": "TILING_KEY_IS",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/TILING_KEY_IS"
                },
                {
                  "text": "TILING_KEY_LIST",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/TILING_KEY_LIST"
                },
                {
                  "text": "REGISTER_TILING_DEFAULT",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/REGISTER_TILING_DEFAULT"
                },
                {
                  "text": "REGISTER_TILING_FOR_TILINGKEY",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY"
                },
                {
                  "text": "REGISTER_NONE_TILING",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/REGISTER_NONE_TILING"
                },
                {
                  "text": "设置Kernel类型",
                  "link": "/api/SIMD-API/基础API/Kernel-Tiling/设置Kernel类型"
                }
              ]
            }
          ]
        },
        {
          "text": "高阶API",
          "collapsed": true,
          "items": [
            {
              "text": "数学计算",
              "collapsed": true,
              "items": [
                {
                  "text": "Tanh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Tanh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tanh接口/Tanh"
                    },
                    {
                      "text": "GetTanhMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tanh接口/GetTanhMaxMinTmpSize"
                    },
                    {
                      "text": "GetTanhTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tanh接口/GetTanhTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Asin接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Asin",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asin接口/Asin"
                    },
                    {
                      "text": "GetAsinMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asin接口/GetAsinMaxMinTmpSize"
                    },
                    {
                      "text": "GetAsinTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asin接口/GetAsinTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Sin接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Sin",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sin接口/Sin"
                    },
                    {
                      "text": "GetSinMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sin接口/GetSinMaxMinTmpSize"
                    },
                    {
                      "text": "GetSinTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sin接口/GetSinTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Acos接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Acos",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acos接口/Acos"
                    },
                    {
                      "text": "GetAcosMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acos接口/GetAcosMaxMinTmpSize"
                    },
                    {
                      "text": "GetAcosTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acos接口/GetAcosTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Cos接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Cos",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cos接口/Cos"
                    },
                    {
                      "text": "GetCosMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cos接口/GetCosMaxMinTmpSize"
                    },
                    {
                      "text": "GetCosTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cos接口/GetCosTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Log接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Log",
                      "link": "/api/SIMD-API/高阶API/数学计算/Log接口/Log-79"
                    },
                    {
                      "text": "GetLogMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Log接口/GetLogMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Log接口/GetLogTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Atan接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Atan",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atan接口/Atan"
                    },
                    {
                      "text": "GetAtanMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atan接口/GetAtanMaxMinTmpSize"
                    },
                    {
                      "text": "GetAtanTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atan接口/GetAtanTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Power接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Power",
                      "link": "/api/SIMD-API/高阶API/数学计算/Power接口/Power"
                    },
                    {
                      "text": "GetPowerMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Power接口/GetPowerMaxMinTmpSize"
                    },
                    {
                      "text": "GetPowerTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Power接口/GetPowerTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Sinh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Sinh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sinh接口/Sinh"
                    },
                    {
                      "text": "GetSinhMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sinh接口/GetSinhMaxMinTmpSize"
                    },
                    {
                      "text": "GetSinhTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sinh接口/GetSinhTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Cosh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Cosh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cosh接口/Cosh"
                    },
                    {
                      "text": "GetCoshMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cosh接口/GetCoshMaxMinTmpSize"
                    },
                    {
                      "text": "GetCoshTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Cosh接口/GetCoshTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Tan接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Tan",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tan接口/Tan"
                    },
                    {
                      "text": "GetTanMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tan接口/GetTanMaxMinTmpSize"
                    },
                    {
                      "text": "GetTanTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Tan接口/GetTanTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Trunc接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Trunc",
                      "link": "/api/SIMD-API/高阶API/数学计算/Trunc接口/Trunc"
                    },
                    {
                      "text": "GetTruncMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Trunc接口/GetTruncMaxMinTmpSize"
                    },
                    {
                      "text": "GetTruncTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Trunc接口/GetTruncTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Frac接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Frac",
                      "link": "/api/SIMD-API/高阶API/数学计算/Frac接口/Frac"
                    },
                    {
                      "text": "GetFracMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Frac接口/GetFracMaxMinTmpSize"
                    },
                    {
                      "text": "GetFracTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Frac接口/GetFracTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Erf接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Erf",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erf接口/Erf"
                    },
                    {
                      "text": "GetErfMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erf接口/GetErfMaxMinTmpSize"
                    },
                    {
                      "text": "GetErfTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erf接口/GetErfTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Erfc接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Erfc",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erfc接口/Erfc"
                    },
                    {
                      "text": "GetErfcMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erfc接口/GetErfcMaxMinTmpSize"
                    },
                    {
                      "text": "GetErfcTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Erfc接口/GetErfcTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Sign接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Sign",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sign接口/Sign"
                    },
                    {
                      "text": "GetSignMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sign接口/GetSignMaxMinTmpSize"
                    },
                    {
                      "text": "GetSignTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Sign接口/GetSignTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Atanh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Atanh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atanh接口/Atanh"
                    },
                    {
                      "text": "GetAtanhMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atanh接口/GetAtanhMaxMinTmpSize"
                    },
                    {
                      "text": "GetAtanhTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Atanh接口/GetAtanhTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Asinh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Asinh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asinh接口/Asinh"
                    },
                    {
                      "text": "GetAsinhMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asinh接口/GetAsinhMaxMinTmpSize"
                    },
                    {
                      "text": "GetAsinhTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Asinh接口/GetAsinhTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Acosh接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Acosh",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acosh接口/Acosh"
                    },
                    {
                      "text": "GetAcoshMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acosh接口/GetAcoshMaxMinTmpSize"
                    },
                    {
                      "text": "GetAcoshTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Acosh接口/GetAcoshTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Floor接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Floor",
                      "link": "/api/SIMD-API/高阶API/数学计算/Floor接口/Floor"
                    },
                    {
                      "text": "GetFloorMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Floor接口/GetFloorMaxMinTmpSize"
                    },
                    {
                      "text": "GetFloorTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Floor接口/GetFloorTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Ceil接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Ceil",
                      "link": "/api/SIMD-API/高阶API/数学计算/Ceil接口/Ceil"
                    },
                    {
                      "text": "GetCeilMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Ceil接口/GetCeilMaxMinTmpSize"
                    },
                    {
                      "text": "GetCeilTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Ceil接口/GetCeilTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Clamp接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Clamp",
                      "link": "/api/SIMD-API/高阶API/数学计算/Clamp接口/Clamp"
                    },
                    {
                      "text": "ClampMax",
                      "link": "/api/SIMD-API/高阶API/数学计算/Clamp接口/ClampMax"
                    },
                    {
                      "text": "ClampMin",
                      "link": "/api/SIMD-API/高阶API/数学计算/Clamp接口/ClampMin"
                    },
                    {
                      "text": "GetClampMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Clamp接口/GetClampMaxMinTmpSize"
                    },
                    {
                      "text": "GetClampTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Clamp接口/GetClampTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Round接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Round",
                      "link": "/api/SIMD-API/高阶API/数学计算/Round接口/Round"
                    },
                    {
                      "text": "GetRoundMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Round接口/GetRoundMaxMinTmpSize"
                    },
                    {
                      "text": "GetRoundTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Round接口/GetRoundTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Axpy接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Axpy",
                      "link": "/api/SIMD-API/高阶API/数学计算/Axpy接口/Axpy-80"
                    },
                    {
                      "text": "GetAxpyMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Axpy接口/GetAxpyMaxMinTmpSize"
                    },
                    {
                      "text": "GetAxpyTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Axpy接口/GetAxpyTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Exp接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Exp",
                      "link": "/api/SIMD-API/高阶API/数学计算/Exp接口/Exp-81"
                    },
                    {
                      "text": "GetExpMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Exp接口/GetExpMaxMinTmpSize"
                    },
                    {
                      "text": "GetExpTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Exp接口/GetExpTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Lgamma接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Lgamma",
                      "link": "/api/SIMD-API/高阶API/数学计算/Lgamma接口/Lgamma"
                    },
                    {
                      "text": "GetLgammaMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Lgamma接口/GetLgammaMaxMinTmpSize"
                    },
                    {
                      "text": "GetLgammaTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Lgamma接口/GetLgammaTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Digamma接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Digamma",
                      "link": "/api/SIMD-API/高阶API/数学计算/Digamma接口/Digamma"
                    },
                    {
                      "text": "GetDigammaMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Digamma接口/GetDigammaMaxMinTmpSize"
                    },
                    {
                      "text": "GetDigammaTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Digamma接口/GetDigammaTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Xor接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Xor",
                      "link": "/api/SIMD-API/高阶API/数学计算/Xor接口/Xor-82"
                    },
                    {
                      "text": "GetXorMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Xor接口/GetXorMaxMinTmpSize"
                    },
                    {
                      "text": "GetXorTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Xor接口/GetXorTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "CumSum接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "CumSum",
                      "link": "/api/SIMD-API/高阶API/数学计算/CumSum接口/CumSum"
                    },
                    {
                      "text": "GetCumSumMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/CumSum接口/GetCumSumMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "Fmod接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Fmod",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fmod接口/Fmod"
                    },
                    {
                      "text": "GetFmodMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fmod接口/GetFmodMaxMinTmpSize"
                    },
                    {
                      "text": "GetFmodTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fmod接口/GetFmodTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "IsFinite接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "IsFinite",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsFinite接口/IsFinite"
                    }
                  ]
                },
                {
                  "text": "Hypot接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Hypot",
                      "link": "/api/SIMD-API/高阶API/数学计算/Hypot接口/Hypot"
                    },
                    {
                      "text": "GetHypotMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Hypot接口/GetHypotMaxMinTmpSize"
                    },
                    {
                      "text": "GetHypotTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Hypot接口/GetHypotTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Fma接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Fma",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fma接口/Fma"
                    },
                    {
                      "text": "GetFmaMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fma接口/GetFmaMaxMinTmpSize"
                    },
                    {
                      "text": "GetFmaTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Fma接口/GetFmaTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "IsNan接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "IsNan",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsNan接口/IsNan"
                    },
                    {
                      "text": "GetIsNanMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsNan接口/GetIsNanMaxMinTmpSize"
                    },
                    {
                      "text": "GetIsNanTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsNan接口/GetIsNanTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "IsInf接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "IsInf",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsInf接口/IsInf"
                    },
                    {
                      "text": "GetIsInfMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsInf接口/GetIsInfMaxMinTmpSize"
                    },
                    {
                      "text": "GetIsInfTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/IsInf接口/GetIsInfTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Rint接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Rint",
                      "link": "/api/SIMD-API/高阶API/数学计算/Rint接口/Rint"
                    },
                    {
                      "text": "GetRintMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Rint接口/GetRintMaxMinTmpSize"
                    },
                    {
                      "text": "GetRintTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Rint接口/GetRintTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "SinCos接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SinCos",
                      "link": "/api/SIMD-API/高阶API/数学计算/SinCos接口/SinCos"
                    },
                    {
                      "text": "GetSinCosMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/SinCos接口/GetSinCosMaxMinTmpSize"
                    },
                    {
                      "text": "GetSinCosTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/SinCos接口/GetSinCosTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalNot接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalNot",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalNot接口/LogicalNot"
                    },
                    {
                      "text": "GetLogicalNotMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalNot接口/GetLogicalNotMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalNotTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalNot接口/GetLogicalNotTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalAnd接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalAnd",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnd接口/LogicalAnd"
                    },
                    {
                      "text": "GetLogicalAndMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnd接口/GetLogicalAndMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalAndTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnd接口/GetLogicalAndTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalAnds接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalAnds",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnds接口/LogicalAnds"
                    },
                    {
                      "text": "GetLogicalAndsMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnds接口/GetLogicalAndsMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalAndsTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalAnds接口/GetLogicalAndsTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalOr接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalOr",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOr接口/LogicalOr"
                    },
                    {
                      "text": "GetLogicalOrMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOr接口/GetLogicalOrMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalOrTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOr接口/GetLogicalOrTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalOrs接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalOrs",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOrs接口/LogicalOrs"
                    },
                    {
                      "text": "GetLogicalOrsMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOrs接口/GetLogicalOrsMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalOrsTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalOrs接口/GetLogicalOrsTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "LogicalXor接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogicalXor",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalXor接口/LogicalXor"
                    },
                    {
                      "text": "GetLogicalXorMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalXor接口/GetLogicalXorMaxMinTmpSize"
                    },
                    {
                      "text": "GetLogicalXorTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/LogicalXor接口/GetLogicalXorTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "BitwiseNot接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "BitwiseNot",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseNot接口/BitwiseNot"
                    },
                    {
                      "text": "GetBitwiseNotMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseNot接口/GetBitwiseNotMaxMinTmpSize"
                    },
                    {
                      "text": "GetBitwiseNotTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseNot接口/GetBitwiseNotTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "BitwiseAnd接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "BitwiseAnd",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseAnd接口/BitwiseAnd"
                    },
                    {
                      "text": "GetBitwiseAndMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseAnd接口/GetBitwiseAndMaxMinTmpSize"
                    },
                    {
                      "text": "GetBitwiseAndTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseAnd接口/GetBitwiseAndTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "BitwiseOr接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "BitwiseOr",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseOr接口/BitwiseOr"
                    },
                    {
                      "text": "GetBitwiseOrMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseOr接口/GetBitwiseOrMaxMinTmpSize"
                    },
                    {
                      "text": "GetBitwiseOrTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseOr接口/GetBitwiseOrTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "BitwiseXor接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "BitwiseXor",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseXor接口/BitwiseXor"
                    },
                    {
                      "text": "GetBitwiseXorMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseXor接口/GetBitwiseXorMaxMinTmpSize"
                    },
                    {
                      "text": "GetBitwiseXorTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/BitwiseXor接口/GetBitwiseXorTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Where接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Where",
                      "link": "/api/SIMD-API/高阶API/数学计算/Where接口/Where"
                    },
                    {
                      "text": "GetWhereMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Where接口/GetWhereMaxMinTmpSize"
                    },
                    {
                      "text": "GetWhereTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/数学计算/Where接口/GetWhereTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "更多样例",
                  "link": "/api/SIMD-API/高阶API/数学计算/更多样例-83"
                }
              ]
            },
            {
              "text": "矩阵计算",
              "collapsed": true,
              "items": [
                {
                  "text": "Matmul Kernel侧接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Matmul使用说明",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/Matmul使用说明"
                    },
                    {
                      "text": "Matmul模板参数",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/Matmul模板参数"
                    },
                    {
                      "text": "MatmulConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/MatmulConfig"
                    },
                    {
                      "text": "MatmulCallBackFunc",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/MatmulCallBackFunc"
                    },
                    {
                      "text": "MatmulPolicy",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/MatmulPolicy"
                    },
                    {
                      "text": "GetNormalConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetNormalConfig"
                    },
                    {
                      "text": "GetMDLConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetMDLConfig"
                    },
                    {
                      "text": "GetSpecialMDLConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetSpecialMDLConfig"
                    },
                    {
                      "text": "GetIBShareNormConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetIBShareNormConfig"
                    },
                    {
                      "text": "GetBasicConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetBasicConfig"
                    },
                    {
                      "text": "GetMMConfig",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetMMConfig"
                    },
                    {
                      "text": "REGIST_MATMUL_OBJ",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/REGIST_MATMUL_OBJ"
                    },
                    {
                      "text": "Init",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/Init-85"
                    },
                    {
                      "text": "SetTensorA",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorA"
                    },
                    {
                      "text": "SetTensorB",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorB"
                    },
                    {
                      "text": "SetTensorScaleA",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorScaleA"
                    },
                    {
                      "text": "SetTensorScaleB",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorScaleB"
                    },
                    {
                      "text": "SetBias",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetBias"
                    },
                    {
                      "text": "DisableBias",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/DisableBias"
                    },
                    {
                      "text": "GetBatchTensorC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetBatchTensorC"
                    },
                    {
                      "text": "Iterate",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/Iterate"
                    },
                    {
                      "text": "GetTensorC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetTensorC"
                    },
                    {
                      "text": "IterateAll",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/IterateAll"
                    },
                    {
                      "text": "WaitIterateAll",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/WaitIterateAll"
                    },
                    {
                      "text": "IterateBatch",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/IterateBatch"
                    },
                    {
                      "text": "WaitIterateBatch",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/WaitIterateBatch"
                    },
                    {
                      "text": "IterateNBatch",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/IterateNBatch"
                    },
                    {
                      "text": "End",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/End"
                    },
                    {
                      "text": "SetHF32",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetHF32"
                    },
                    {
                      "text": "SetTail",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTail"
                    },
                    {
                      "text": "SetBatchNum",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetBatchNum"
                    },
                    {
                      "text": "SetQuantScalar",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetQuantScalar"
                    },
                    {
                      "text": "SetQuantVector",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetQuantVector"
                    },
                    {
                      "text": "SetOrgShape",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetOrgShape"
                    },
                    {
                      "text": "SetSingleShape",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetSingleShape"
                    },
                    {
                      "text": "SetLocalWorkspace",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetLocalWorkspace"
                    },
                    {
                      "text": "SetWorkspace",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetWorkspace"
                    },
                    {
                      "text": "SetAntiQuantScalar",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetAntiQuantScalar"
                    },
                    {
                      "text": "SetAntiQuantVector",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetAntiQuantVector"
                    },
                    {
                      "text": "WaitGetTensorC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/WaitGetTensorC"
                    },
                    {
                      "text": "GetOffsetC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetOffsetC"
                    },
                    {
                      "text": "AsyncGetTensorC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/AsyncGetTensorC"
                    },
                    {
                      "text": "SetSubBlockIdx",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetSubBlockIdx"
                    },
                    {
                      "text": "GetSubBlockIdx",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetSubBlockIdx"
                    },
                    {
                      "text": "SetUserDefInfo",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetUserDefInfo"
                    },
                    {
                      "text": "SetSelfDefineData",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetSelfDefineData"
                    },
                    {
                      "text": "GetMatmulApiTiling",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetMatmulApiTiling"
                    },
                    {
                      "text": "ClearBias",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/ClearBias"
                    },
                    {
                      "text": "GetBatchC",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetBatchC"
                    },
                    {
                      "text": "SetSparseIndex",
                      "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetSparseIndex"
                    }
                  ]
                },
                {
                  "text": "Matmul Tiling侧接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Matmul Tiling类",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Matmul Tiling类使用说明",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/Matmul-Tiling类使用说明"
                        },
                        {
                          "text": "Matmul Tiling类构造函数",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/Matmul-Tiling类构造函数"
                        },
                        {
                          "text": "TCubeTiling结构体",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/TCubeTiling结构体"
                        },
                        {
                          "text": "SetAType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetAType"
                        },
                        {
                          "text": "SetBType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBType"
                        },
                        {
                          "text": "SetScaleAType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetScaleAType"
                        },
                        {
                          "text": "SetScaleBType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetScaleBType"
                        },
                        {
                          "text": "SetCType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetCType"
                        },
                        {
                          "text": "SetBiasType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBiasType"
                        },
                        {
                          "text": "SetSingleShape",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetSingleShape-86"
                        },
                        {
                          "text": "SetShape",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetShape"
                        },
                        {
                          "text": "SetOrgShape",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetOrgShape-87"
                        },
                        {
                          "text": "SetFixSplit",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetFixSplit"
                        },
                        {
                          "text": "EnableMultiCoreSplitK",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/EnableMultiCoreSplitK"
                        },
                        {
                          "text": "EnableBias",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/EnableBias"
                        },
                        {
                          "text": "SetBufferSpace",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBufferSpace"
                        },
                        {
                          "text": "SetTraverse",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetTraverse"
                        },
                        {
                          "text": "SetMadType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetMadType"
                        },
                        {
                          "text": "SetSplitRange",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetSplitRange"
                        },
                        {
                          "text": "SetDoubleBuffer",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetDoubleBuffer"
                        },
                        {
                          "text": "GetBaseM",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetBaseM"
                        },
                        {
                          "text": "GetBaseN",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetBaseN"
                        },
                        {
                          "text": "GetBaseK",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetBaseK"
                        },
                        {
                          "text": "GetTiling",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetTiling"
                        },
                        {
                          "text": "EnableL1BankConflictOptimise",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/EnableL1BankConflictOptimise"
                        },
                        {
                          "text": "SetDim",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetDim"
                        },
                        {
                          "text": "SetSingleRange",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetSingleRange"
                        },
                        {
                          "text": "GetSingleShape",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetSingleShape"
                        },
                        {
                          "text": "GetCoreNum",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/GetCoreNum"
                        },
                        {
                          "text": "SetAlignSplit",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetAlignSplit"
                        },
                        {
                          "text": "SetDequantType",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetDequantType"
                        },
                        {
                          "text": "SetALayout",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetALayout"
                        },
                        {
                          "text": "SetBLayout",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBLayout"
                        },
                        {
                          "text": "SetCLayout",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetCLayout"
                        },
                        {
                          "text": "SetBatchNum",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBatchNum-88"
                        },
                        {
                          "text": "SetBatchInfoForNormal",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBatchInfoForNormal"
                        },
                        {
                          "text": "SetMatmulConfigParams",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetMatmulConfigParams"
                        },
                        {
                          "text": "SetBias",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetBias-89"
                        },
                        {
                          "text": "SetSplitK",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetSplitK"
                        },
                        {
                          "text": "SetSparse",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/SetSparse"
                        }
                      ]
                    },
                    {
                      "text": "获取Matmul计算所需空间",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "MultiCoreMatmulGetTmpBufSize",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/MultiCoreMatmulGetTmpBufSize"
                        },
                        {
                          "text": "MultiCoreMatmulGetTmpBufSizeV2",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/MultiCoreMatmulGetTmpBufSizeV2"
                        },
                        {
                          "text": "BatchMatmulGetTmpBufSize",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/BatchMatmulGetTmpBufSize"
                        },
                        {
                          "text": "BatchMatmulGetTmpBufSizeV2",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/BatchMatmulGetTmpBufSizeV2"
                        },
                        {
                          "text": "MatmulGetTmpBufSize",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/MatmulGetTmpBufSize"
                        },
                        {
                          "text": "MatmulGetTmpBufSizeV2",
                          "link": "/api/SIMD-API/高阶API/矩阵计算/获取Matmul计算所需空间/MatmulGetTmpBufSizeV2"
                        }
                      ]
                    }
                  ]
                }
              ]
            },
            {
              "text": "激活函数",
              "collapsed": true,
              "items": [
                {
                  "text": "SoftMax接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SoftMax",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftMax"
                    },
                    {
                      "text": "SimpleSoftMax",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SimpleSoftMax"
                    },
                    {
                      "text": "SoftmaxFlash",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlash"
                    },
                    {
                      "text": "SoftmaxGrad",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxGrad"
                    },
                    {
                      "text": "SoftmaxFlashV2",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlashV2"
                    },
                    {
                      "text": "SoftmaxFlashV3",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlashV3"
                    },
                    {
                      "text": "SoftmaxGradFront",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxGradFront"
                    },
                    {
                      "text": "AdjustSoftMaxRes",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/AdjustSoftMaxRes"
                    },
                    {
                      "text": "SoftMax Tiling使用说明",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftMax-Tiling使用说明"
                    },
                    {
                      "text": "SoftMax/SimpleSoftMax Tiling",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftMax-SimpleSoftMax-Tiling"
                    },
                    {
                      "text": "SoftmaxFlash Tiling接口",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlash-Tiling接口"
                    },
                    {
                      "text": "SoftmaxGrad Tiling接口",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxGrad-Tiling接口"
                    },
                    {
                      "text": "SoftmaxFlashV2 Tiling接口",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlashV2-Tiling接口"
                    },
                    {
                      "text": "SoftmaxFlashV3 Tiling接口",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/SoftmaxFlashV3-Tiling接口"
                    },
                    {
                      "text": "IsBasicBlockInSoftMax",
                      "link": "/api/SIMD-API/高阶API/激活函数/SoftMax接口/IsBasicBlockInSoftMax"
                    }
                  ]
                },
                {
                  "text": "LogSoftMax接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "LogSoftMax",
                      "link": "/api/SIMD-API/高阶API/激活函数/LogSoftMax接口/LogSoftMax"
                    },
                    {
                      "text": "LogSoftMax Tiling",
                      "link": "/api/SIMD-API/高阶API/激活函数/LogSoftMax接口/LogSoftMax-Tiling"
                    }
                  ]
                },
                {
                  "text": "Gelu接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Gelu",
                      "link": "/api/SIMD-API/高阶API/激活函数/Gelu接口/Gelu"
                    },
                    {
                      "text": "FasterGelu",
                      "link": "/api/SIMD-API/高阶API/激活函数/Gelu接口/FasterGelu"
                    },
                    {
                      "text": "FasterGeluV2",
                      "link": "/api/SIMD-API/高阶API/激活函数/Gelu接口/FasterGeluV2"
                    },
                    {
                      "text": "GetGeluMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/Gelu接口/GetGeluMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "SwiGLU接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "SwiGLU",
                      "link": "/api/SIMD-API/高阶API/激活函数/SwiGLU接口/SwiGLU"
                    },
                    {
                      "text": "GetSwiGLUMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/SwiGLU接口/GetSwiGLUMaxMinTmpSize"
                    },
                    {
                      "text": "GetSwiGLUTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/SwiGLU接口/GetSwiGLUTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "Silu接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Silu",
                      "link": "/api/SIMD-API/高阶API/激活函数/Silu接口/Silu"
                    },
                    {
                      "text": "GetSiluTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/Silu接口/GetSiluTmpSize"
                    }
                  ]
                },
                {
                  "text": "Swish接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Swish",
                      "link": "/api/SIMD-API/高阶API/激活函数/Swish接口/Swish"
                    },
                    {
                      "text": "GetSwishTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/Swish接口/GetSwishTmpSize"
                    }
                  ]
                },
                {
                  "text": "GeGLU接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "GeGLU",
                      "link": "/api/SIMD-API/高阶API/激活函数/GeGLU接口/GeGLU"
                    },
                    {
                      "text": "GetGeGLUMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/GeGLU接口/GetGeGLUMaxMinTmpSize"
                    },
                    {
                      "text": "GetGeGLUTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/GeGLU接口/GetGeGLUTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "ReGlu接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReGlu",
                      "link": "/api/SIMD-API/高阶API/激活函数/ReGlu接口/ReGlu"
                    },
                    {
                      "text": "GetReGluMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/ReGlu接口/GetReGluMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "Sigmoid接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Sigmoid",
                      "link": "/api/SIMD-API/高阶API/激活函数/Sigmoid接口/Sigmoid"
                    },
                    {
                      "text": "GetSigmoidMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/激活函数/Sigmoid接口/GetSigmoidMaxMinTmpSize"
                    }
                  ]
                }
              ]
            },
            {
              "text": "归一化操作",
              "collapsed": true,
              "items": [
                {
                  "text": "LayerNorm",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNorm"
                },
                {
                  "text": "LayerNorm Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNorm-Tiling"
                },
                {
                  "text": "LayerNormGrad",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNormGrad"
                },
                {
                  "text": "LayerNormGrad Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNormGrad-Tiling"
                },
                {
                  "text": "LayerNormGradBeta",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNormGradBeta"
                },
                {
                  "text": "LayerNormGradBeta Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/LayerNormGradBeta-Tiling"
                },
                {
                  "text": "RmsNorm",
                  "link": "/api/SIMD-API/高阶API/归一化操作/RmsNorm"
                },
                {
                  "text": "RmsNorm Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/RmsNorm-Tiling"
                },
                {
                  "text": "BatchNorm",
                  "link": "/api/SIMD-API/高阶API/归一化操作/BatchNorm"
                },
                {
                  "text": "BatchNorm Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/BatchNorm-Tiling"
                },
                {
                  "text": "DeepNorm",
                  "link": "/api/SIMD-API/高阶API/归一化操作/DeepNorm"
                },
                {
                  "text": "DeepNorm Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/DeepNorm-Tiling"
                },
                {
                  "text": "Normalize",
                  "link": "/api/SIMD-API/高阶API/归一化操作/Normalize"
                },
                {
                  "text": "Normalize Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/Normalize-Tiling"
                },
                {
                  "text": "WelfordUpdate",
                  "link": "/api/SIMD-API/高阶API/归一化操作/WelfordUpdate"
                },
                {
                  "text": "WelfordUpdate Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/WelfordUpdate-Tiling"
                },
                {
                  "text": "WelfordFinalize",
                  "link": "/api/SIMD-API/高阶API/归一化操作/WelfordFinalize"
                },
                {
                  "text": "WelfordFinalize Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/WelfordFinalize-Tiling"
                },
                {
                  "text": "GroupNorm",
                  "link": "/api/SIMD-API/高阶API/归一化操作/GroupNorm"
                },
                {
                  "text": "GroupNorm Tiling",
                  "link": "/api/SIMD-API/高阶API/归一化操作/GroupNorm-Tiling"
                }
              ]
            },
            {
              "text": "量化操作",
              "collapsed": true,
              "items": [
                {
                  "text": "AscendQuant",
                  "link": "/api/SIMD-API/高阶API/量化操作/AscendQuant"
                },
                {
                  "text": "GetAscendQuantMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendQuantMaxMinTmpSize"
                },
                {
                  "text": "GetAscendQuantTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendQuantTmpBufferFactorSize"
                },
                {
                  "text": "AscendDequant",
                  "link": "/api/SIMD-API/高阶API/量化操作/AscendDequant"
                },
                {
                  "text": "GetAscendDequantMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendDequantMaxMinTmpSize"
                },
                {
                  "text": "GetAscendDequantTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendDequantTmpBufferFactorSize"
                },
                {
                  "text": "AscendAntiQuant",
                  "link": "/api/SIMD-API/高阶API/量化操作/AscendAntiQuant"
                },
                {
                  "text": "GetAscendAntiQuantMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendAntiQuantMaxMinTmpSize"
                },
                {
                  "text": "GetAscendAntiQuantTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAscendAntiQuantTmpBufferFactorSize"
                },
                {
                  "text": "Quantize",
                  "link": "/api/SIMD-API/高阶API/量化操作/Quantize"
                },
                {
                  "text": "GetQuantizeMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetQuantizeMaxMinTmpSize"
                },
                {
                  "text": "GetQuantizeTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetQuantizeTmpBufferFactorSize"
                },
                {
                  "text": "Dequantize",
                  "link": "/api/SIMD-API/高阶API/量化操作/Dequantize"
                },
                {
                  "text": "GetDequantizeMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetDequantizeMaxMinTmpSize"
                },
                {
                  "text": "GetDequantizeTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetDequantizeTmpBufferFactorSize"
                },
                {
                  "text": "AntiQuantize",
                  "link": "/api/SIMD-API/高阶API/量化操作/AntiQuantize"
                },
                {
                  "text": "GetAntiQuantizeMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAntiQuantizeMaxMinTmpSize"
                },
                {
                  "text": "GetAntiQuantizeTmpBufferFactorSize",
                  "link": "/api/SIMD-API/高阶API/量化操作/GetAntiQuantizeTmpBufferFactorSize"
                }
              ]
            },
            {
              "text": "归约操作",
              "collapsed": true,
              "items": [
                {
                  "text": "Sum接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Sum",
                      "link": "/api/SIMD-API/高阶API/归约操作/Sum接口/Sum"
                    },
                    {
                      "text": "GetSumMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/Sum接口/GetSumMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "Mean接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Mean",
                      "link": "/api/SIMD-API/高阶API/归约操作/Mean接口/Mean"
                    },
                    {
                      "text": "GetMeanMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/Mean接口/GetMeanMaxMinTmpSize"
                    },
                    {
                      "text": "GetMeanTmpBufferFactorSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/Mean接口/GetMeanTmpBufferFactorSize"
                    }
                  ]
                },
                {
                  "text": "ReduceXorSum接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceXorSum",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceXorSum接口/ReduceXorSum"
                    },
                    {
                      "text": "GetReduceXorSumMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceXorSum接口/GetReduceXorSumMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceSum接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceSum",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceSum接口/ReduceSum-90"
                    },
                    {
                      "text": "GetReduceSumMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceSum接口/GetReduceSumMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceMean接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceMean",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMean接口/ReduceMean"
                    },
                    {
                      "text": "GetReduceMeanMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMean接口/GetReduceMeanMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceMax接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceMax",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMax接口/ReduceMax-91"
                    },
                    {
                      "text": "GetReduceMaxMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMax接口/GetReduceMaxMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceMin接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceMin",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMin接口/ReduceMin-92"
                    },
                    {
                      "text": "GetReduceMinMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceMin接口/GetReduceMinMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceAny接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceAny",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceAny接口/ReduceAny"
                    },
                    {
                      "text": "GetReduceAnyMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceAny接口/GetReduceAnyMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceAll接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceAll",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceAll接口/ReduceAll"
                    },
                    {
                      "text": "GetReduceAllMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceAll接口/GetReduceAllMaxMinTmpSize"
                    }
                  ]
                },
                {
                  "text": "ReduceProd接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "ReduceProd",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceProd接口/ReduceProd"
                    },
                    {
                      "text": "GetReduceProdMaxMinTmpSize",
                      "link": "/api/SIMD-API/高阶API/归约操作/ReduceProd接口/GetReduceProdMaxMinTmpSize"
                    }
                  ]
                }
              ]
            },
            {
              "text": "排序操作",
              "collapsed": true,
              "items": [
                {
                  "text": "TopK",
                  "link": "/api/SIMD-API/高阶API/排序操作/TopK"
                },
                {
                  "text": "TopK Tiling",
                  "link": "/api/SIMD-API/高阶API/排序操作/TopK-Tiling"
                },
                {
                  "text": "Concat",
                  "link": "/api/SIMD-API/高阶API/排序操作/Concat"
                },
                {
                  "text": "GetConcatTmpSize",
                  "link": "/api/SIMD-API/高阶API/排序操作/GetConcatTmpSize"
                },
                {
                  "text": "Extract",
                  "link": "/api/SIMD-API/高阶API/排序操作/Extract"
                },
                {
                  "text": "Sort",
                  "link": "/api/SIMD-API/高阶API/排序操作/Sort"
                },
                {
                  "text": "GetSortTmpSize",
                  "link": "/api/SIMD-API/高阶API/排序操作/GetSortTmpSize"
                },
                {
                  "text": "GetSortMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/排序操作/GetSortMaxMinTmpSize"
                },
                {
                  "text": "GetSortOffset",
                  "link": "/api/SIMD-API/高阶API/排序操作/GetSortOffset"
                },
                {
                  "text": "GetSortLen",
                  "link": "/api/SIMD-API/高阶API/排序操作/GetSortLen"
                },
                {
                  "text": "MrgSort",
                  "link": "/api/SIMD-API/高阶API/排序操作/MrgSort-93"
                }
              ]
            },
            {
              "text": "索引计算",
              "collapsed": true,
              "items": [
                {
                  "text": "Arange",
                  "link": "/api/SIMD-API/高阶API/索引计算/Arange-94"
                },
                {
                  "text": "GetArangeMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/索引计算/GetArangeMaxMinTmpSize"
                }
              ]
            },
            {
              "text": "数据过滤",
              "collapsed": true,
              "items": [
                {
                  "text": "Select",
                  "link": "/api/SIMD-API/高阶API/数据过滤/Select-95"
                },
                {
                  "text": "GetSelectMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/数据过滤/GetSelectMaxMinTmpSize"
                },
                {
                  "text": "DropOut",
                  "link": "/api/SIMD-API/高阶API/数据过滤/DropOut"
                },
                {
                  "text": "GetDropOutMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/数据过滤/GetDropOutMaxMinTmpSize"
                }
              ]
            },
            {
              "text": "张量变换",
              "collapsed": true,
              "items": [
                {
                  "text": "Transpose",
                  "link": "/api/SIMD-API/高阶API/张量变换/Transpose-96"
                },
                {
                  "text": "Transpose Tiling",
                  "link": "/api/SIMD-API/高阶API/张量变换/Transpose-Tiling"
                },
                {
                  "text": "TransData",
                  "link": "/api/SIMD-API/高阶API/张量变换/TransData"
                },
                {
                  "text": "GetTransDataMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/张量变换/GetTransDataMaxMinTmpSize"
                },
                {
                  "text": "Pad",
                  "link": "/api/SIMD-API/高阶API/张量变换/Pad"
                },
                {
                  "text": "Pad Tiling",
                  "link": "/api/SIMD-API/高阶API/张量变换/Pad-Tiling"
                },
                {
                  "text": "UnPad",
                  "link": "/api/SIMD-API/高阶API/张量变换/UnPad"
                },
                {
                  "text": "UnPad Tiling",
                  "link": "/api/SIMD-API/高阶API/张量变换/UnPad-Tiling"
                },
                {
                  "text": "Broadcast",
                  "link": "/api/SIMD-API/高阶API/张量变换/Broadcast"
                },
                {
                  "text": "GetBroadCastMaxMinTmpSize",
                  "link": "/api/SIMD-API/高阶API/张量变换/GetBroadCastMaxMinTmpSize"
                },
                {
                  "text": "Fill",
                  "link": "/api/SIMD-API/高阶API/张量变换/Fill-97"
                }
              ]
            },
            {
              "text": "HCCL通信类",
              "collapsed": true,
              "items": [
                {
                  "text": "HCCL Kernel侧接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "HCCL使用说明",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/HCCL使用说明"
                    },
                    {
                      "text": "HCCL模板参数",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/HCCL模板参数"
                    },
                    {
                      "text": "InitV2",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/InitV2"
                    },
                    {
                      "text": "SetCcTilingV2",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/SetCcTilingV2"
                    },
                    {
                      "text": "AllReduce",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/AllReduce"
                    },
                    {
                      "text": "AllGather",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/AllGather"
                    },
                    {
                      "text": "ReduceScatter",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/ReduceScatter"
                    },
                    {
                      "text": "AlltoAll",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/AlltoAll"
                    },
                    {
                      "text": "AlltoAllV",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/AlltoAllV"
                    },
                    {
                      "text": "AlltoAllvWrite",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/AlltoAllvWrite"
                    },
                    {
                      "text": "BatchWrite",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/BatchWrite"
                    },
                    {
                      "text": "Commit",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Commit"
                    },
                    {
                      "text": "Wait",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Wait-98"
                    },
                    {
                      "text": "Iterate",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Iterate-99"
                    },
                    {
                      "text": "Query",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Query"
                    },
                    {
                      "text": "InterHcclGroupSync",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/InterHcclGroupSync"
                    },
                    {
                      "text": "Finalize",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Finalize"
                    },
                    {
                      "text": "GetWindowsInAddr",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/GetWindowsInAddr"
                    },
                    {
                      "text": "GetWindowsOutAddr",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/GetWindowsOutAddr"
                    },
                    {
                      "text": "GetRankId",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/GetRankId"
                    },
                    {
                      "text": "GetRankDim",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/GetRankDim"
                    },
                    {
                      "text": "QueueBarrier",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/QueueBarrier"
                    },
                    {
                      "text": "GetQueueNum",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/GetQueueNum"
                    },
                    {
                      "text": "Init（废弃）",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/Init（废弃）"
                    },
                    {
                      "text": "SetCcTiling（废弃）",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Kernel侧接口/SetCcTiling（废弃）"
                    }
                  ]
                },
                {
                  "text": "HCCL Tiling侧接口",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "HCCL Tiling使用说明",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/HCCL-Tiling使用说明"
                    },
                    {
                      "text": "TilingData结构体",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/TilingData结构体"
                    },
                    {
                      "text": "HCCL Tiling构造函数",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/HCCL-Tiling构造函数"
                    },
                    {
                      "text": "GetTiling",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/GetTiling-100"
                    },
                    {
                      "text": "SetOpType",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetOpType"
                    },
                    {
                      "text": "SetGroupName",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetGroupName"
                    },
                    {
                      "text": "SetAlgConfig",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetAlgConfig"
                    },
                    {
                      "text": "SetReduceType",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetReduceType"
                    },
                    {
                      "text": "SetStepSize",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetStepSize"
                    },
                    {
                      "text": "SetSkipLocalRankCopy",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetSkipLocalRankCopy"
                    },
                    {
                      "text": "SetSkipBufferWindowCopy",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetSkipBufferWindowCopy"
                    },
                    {
                      "text": "SetDebugMode",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetDebugMode"
                    },
                    {
                      "text": "SetCommBlockNum",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetCommBlockNum"
                    },
                    {
                      "text": "SetQueueNum",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetQueueNum"
                    },
                    {
                      "text": "SetCommEngine",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/SetCommEngine"
                    },
                    {
                      "text": "v1版本TilingData（废弃）",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/v1版本TilingData（废弃）"
                    },
                    {
                      "text": "v2版本TilingData（废弃）",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Tiling侧接口/v2版本TilingData（废弃）"
                    }
                  ]
                },
                {
                  "text": "HCCL Context",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "HCCL Context简介",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Context/HCCL-Context简介"
                    },
                    {
                      "text": "SetHcclContext",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Context/SetHcclContext"
                    },
                    {
                      "text": "GetHcclContext",
                      "link": "/api/SIMD-API/高阶API/HCCL通信类/HCCL-Context/GetHcclContext"
                    }
                  ]
                }
              ]
            },
            {
              "text": "卷积计算",
              "collapsed": true,
              "items": [
                {
                  "text": "Conv3D",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Conv3D Kernel侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3D使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/Conv3D使用说明"
                        },
                        {
                          "text": "Conv3D模板参数",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/Conv3D模板参数"
                        },
                        {
                          "text": "Init",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/Init-101"
                        },
                        {
                          "text": "SetInput",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/SetInput"
                        },
                        {
                          "text": "SetWeight",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/SetWeight"
                        },
                        {
                          "text": "SetBias",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/SetBias-102"
                        },
                        {
                          "text": "SetSingleOutputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/SetSingleOutputShape"
                        },
                        {
                          "text": "SetInputStartPosition",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/SetInputStartPosition"
                        },
                        {
                          "text": "IterateAll",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/IterateAll-103"
                        },
                        {
                          "text": "End",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Kernel侧接口/End-104"
                        }
                      ]
                    },
                    {
                      "text": "Conv3D Tiling侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3D Tiling使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/Conv3D-Tiling使用说明"
                        },
                        {
                          "text": "Conv3D Tiling构造函数",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/Conv3D-Tiling构造函数"
                        },
                        {
                          "text": "TConv3DApiTiling结构体",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/TConv3DApiTiling结构体"
                        },
                        {
                          "text": "GetTiling",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/GetTiling-105"
                        },
                        {
                          "text": "SetOrgWeightShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetOrgWeightShape"
                        },
                        {
                          "text": "SetOrgInputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetOrgInputShape"
                        },
                        {
                          "text": "SetSingleWeightShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetSingleWeightShape"
                        },
                        {
                          "text": "SetSingleOutputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetSingleOutputShape-106"
                        },
                        {
                          "text": "SetInputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetInputType"
                        },
                        {
                          "text": "SetWeightType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetWeightType"
                        },
                        {
                          "text": "SetBiasType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetBiasType-107"
                        },
                        {
                          "text": "SetOutputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetOutputType"
                        },
                        {
                          "text": "SetPadding",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetPadding"
                        },
                        {
                          "text": "SetDilation",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetDilation"
                        },
                        {
                          "text": "SetStride",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetStride"
                        },
                        {
                          "text": "SetGroups",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3D/Conv3D-Tiling侧接口/SetGroups"
                        }
                      ]
                    }
                  ]
                },
                {
                  "text": "Conv3DBackpropInput",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Conv3DBackpropInput Kernel侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3DBackpropInput使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/Conv3DBackpropInput使用说明"
                        },
                        {
                          "text": "Init",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/Init-108"
                        },
                        {
                          "text": "SetGradOutput",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/SetGradOutput"
                        },
                        {
                          "text": "SetWeight",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/SetWeight-109"
                        },
                        {
                          "text": "SetSingleShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/SetSingleShape-110"
                        },
                        {
                          "text": "SetStartPosition",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/SetStartPosition"
                        },
                        {
                          "text": "Iterate",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/Iterate-111"
                        },
                        {
                          "text": "GetTensorC",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/GetTensorC-112"
                        },
                        {
                          "text": "End",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Kernel侧接口/End-113"
                        }
                      ]
                    },
                    {
                      "text": "Conv3DBackpropInput Tiling侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3DBackpropInput Tiling使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/Conv3DBackpropInput-Tiling使用说明"
                        },
                        {
                          "text": "Conv3DBackpropInput Tiling构造函数",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/Conv3DBackpropInput-Tiling构造函数"
                        },
                        {
                          "text": "TConv3DBackpropInputTiling结构体",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/TConv3DBackpropInputTiling结构体"
                        },
                        {
                          "text": "GetTiling",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/GetTiling-114"
                        },
                        {
                          "text": "SetWeightShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetWeightShape"
                        },
                        {
                          "text": "SetInputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetInputShape"
                        },
                        {
                          "text": "SetGradOutputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetGradOutputShape"
                        },
                        {
                          "text": "SetWeightType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetWeightType-115"
                        },
                        {
                          "text": "SetInputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetInputType-116"
                        },
                        {
                          "text": "SetGradOutputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetGradOutputType"
                        },
                        {
                          "text": "SetPadding",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetPadding-117"
                        },
                        {
                          "text": "SetStride",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetStride-118"
                        },
                        {
                          "text": "SetDilation",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetDilation-119"
                        },
                        {
                          "text": "SetOutputPadding",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropInput/Conv3DBackpropInput-Tiling侧接口/SetOutputPadding"
                        }
                      ]
                    }
                  ]
                },
                {
                  "text": "Conv3DBackpropFilter",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "Conv3DBackpropFilter Kernel侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3DBackpropFilter使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/Conv3DBackpropFilter使用说明"
                        },
                        {
                          "text": "Init",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/Init-120"
                        },
                        {
                          "text": "SetInput",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/SetInput-121"
                        },
                        {
                          "text": "SetGradOutput",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/SetGradOutput-122"
                        },
                        {
                          "text": "SetSingleShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/SetSingleShape-123"
                        },
                        {
                          "text": "SetStartPosition",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/SetStartPosition-124"
                        },
                        {
                          "text": "Iterate",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/Iterate-125"
                        },
                        {
                          "text": "GetTensorC",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/GetTensorC-126"
                        },
                        {
                          "text": "End",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Kernel侧接口/End-127"
                        }
                      ]
                    },
                    {
                      "text": "Conv3DBackpropFilter Tiling侧接口",
                      "collapsed": true,
                      "items": [
                        {
                          "text": "Conv3DBackpropFilter Tiling使用说明",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/Conv3DBackpropFilter-Tiling使用说明"
                        },
                        {
                          "text": "Conv3DBackpropFilter Tiling构造函数",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/Conv3DBackpropFilter-Tiling构造函数"
                        },
                        {
                          "text": "TConv3DBpFilterTiling结构体",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/TConv3DBpFilterTiling结构体"
                        },
                        {
                          "text": "GetTiling",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/GetTiling-128"
                        },
                        {
                          "text": "SetWeightShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetWeightShape-129"
                        },
                        {
                          "text": "SetInputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetInputShape-130"
                        },
                        {
                          "text": "SetGradOutputShape",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetGradOutputShape-131"
                        },
                        {
                          "text": "SetWeightType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetWeightType-132"
                        },
                        {
                          "text": "SetInputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetInputType-133"
                        },
                        {
                          "text": "SetGradOutputType",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetGradOutputType-134"
                        },
                        {
                          "text": "SetPadding",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetPadding-135"
                        },
                        {
                          "text": "SetStride",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetStride-136"
                        },
                        {
                          "text": "SetDilation",
                          "link": "/api/SIMD-API/高阶API/卷积计算/Conv3DBackpropFilter/Conv3DBackpropFilter-Tiling侧接口/SetDilation-137"
                        }
                      ]
                    }
                  ]
                }
              ]
            },
            {
              "text": "随机函数",
              "collapsed": true,
              "items": [
                {
                  "text": "PhiloxRandom",
                  "link": "/api/SIMD-API/高阶API/随机函数/PhiloxRandom"
                }
              ]
            }
          ]
        },
        {
          "text": "C API",
          "link": "/api/SIMD-API/C-API"
        },
        {
          "text": "其他数据类型",
          "collapsed": true,
          "items": [
            {
              "text": "ShapeInfo",
              "link": "/api/SIMD-API/其他数据类型/ShapeInfo"
            },
            {
              "text": "ListTensorDesc",
              "link": "/api/SIMD-API/其他数据类型/ListTensorDesc"
            },
            {
              "text": "TensorDesc",
              "collapsed": true,
              "items": [
                {
                  "text": "TensorDesc简介",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/TensorDesc简介"
                },
                {
                  "text": "构造和析构函数",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/构造和析构函数"
                },
                {
                  "text": "SetShapeAddr",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/SetShapeAddr"
                },
                {
                  "text": "GetDim",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/GetDim"
                },
                {
                  "text": "GetIndex",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/GetIndex"
                },
                {
                  "text": "GetShape",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/GetShape-138"
                },
                {
                  "text": "GetDataPtr",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/GetDataPtr"
                },
                {
                  "text": "GetDataObj",
                  "link": "/api/SIMD-API/其他数据类型/TensorDesc/GetDataObj"
                }
              ]
            },
            {
              "text": "UnaryRepeatParams",
              "link": "/api/SIMD-API/其他数据类型/UnaryRepeatParams"
            },
            {
              "text": "BinaryRepeatParams",
              "link": "/api/SIMD-API/其他数据类型/BinaryRepeatParams"
            },
            {
              "text": "complex32/complex64",
              "link": "/api/SIMD-API/其他数据类型/complex32-complex64"
            },
            {
              "text": "TPosition",
              "link": "/api/SIMD-API/其他数据类型/TPosition"
            }
          ]
        }
      ]
    },
    {
      "text": "SIMT API",
      "collapsed": true,
      "items": [
        {
          "text": "概述",
          "link": "/api/SIMT-API/概述"
        },
        {
          "text": "SIMT编程简介",
          "collapsed": true,
          "items": [
            {
              "text": "编程模型",
              "link": "/api/SIMT-API/SIMT编程简介/编程模型"
            },
            {
              "text": "扩展语法",
              "collapsed": true,
              "items": [
                {
                  "text": "函数执行空间限定符",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/函数执行空间限定符"
                },
                {
                  "text": "内存空间限定符",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/内存空间限定符"
                },
                {
                  "text": "内置数据类型",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/内置数据类型-139"
                },
                {
                  "text": "内置变量",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/内置变量"
                },
                {
                  "text": "内置宏",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/内置宏"
                },
                {
                  "text": "核函数配置",
                  "link": "/api/SIMT-API/SIMT编程简介/扩展语法/核函数配置"
                }
              ]
            },
            {
              "text": "API列表",
              "link": "/api/SIMT-API/SIMT编程简介/API列表"
            }
          ]
        },
        {
          "text": "SIMD与SIMT混合编程简介",
          "collapsed": true,
          "items": [
            {
              "text": "编程模型",
              "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/编程模型-140"
            },
            {
              "text": "扩展语法",
              "collapsed": true,
              "items": [
                {
                  "text": "函数执行空间限定符",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/函数执行空间限定符-142"
                },
                {
                  "text": "内存空间限定符",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内存空间限定符-143"
                },
                {
                  "text": "内置数据类型",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内置数据类型-144"
                },
                {
                  "text": "内置变量",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内置变量-145"
                },
                {
                  "text": "内置宏",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/内置宏-146"
                },
                {
                  "text": "核函数配置",
                  "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/核函数配置-147"
                }
              ]
            },
            {
              "text": "API列表",
              "link": "/api/SIMT-API/SIMD与SIMT混合编程简介/API列表-148"
            }
          ]
        },
        {
          "text": "协作组",
          "collapsed": true,
          "items": [
            {
              "text": "协作组简介",
              "link": "/api/SIMT-API/协作组/协作组简介"
            },
            {
              "text": "tiled_partition",
              "link": "/api/SIMT-API/协作组/tiled_partition"
            },
            {
              "text": "binary_partition",
              "link": "/api/SIMT-API/协作组/binary_partition"
            },
            {
              "text": "thread_block",
              "collapsed": true,
              "items": [
                {
                  "text": "thread_block简介",
                  "link": "/api/SIMT-API/协作组/thread_block/thread_block简介"
                },
                {
                  "text": "thread_block构造函数",
                  "link": "/api/SIMT-API/协作组/thread_block/thread_block构造函数"
                },
                {
                  "text": "sync",
                  "link": "/api/SIMT-API/协作组/thread_block/sync"
                },
                {
                  "text": "size",
                  "link": "/api/SIMT-API/协作组/thread_block/size"
                },
                {
                  "text": "num_threads",
                  "link": "/api/SIMT-API/协作组/thread_block/num_threads"
                },
                {
                  "text": "thread_rank",
                  "link": "/api/SIMT-API/协作组/thread_block/thread_rank"
                },
                {
                  "text": "thread_index",
                  "link": "/api/SIMT-API/协作组/thread_block/thread_index"
                },
                {
                  "text": "group_index",
                  "link": "/api/SIMT-API/协作组/thread_block/group_index"
                },
                {
                  "text": "group_dim",
                  "link": "/api/SIMT-API/协作组/thread_block/group_dim"
                },
                {
                  "text": "dim_threads",
                  "link": "/api/SIMT-API/协作组/thread_block/dim_threads"
                }
              ]
            },
            {
              "text": "coalesced_group",
              "collapsed": true,
              "items": [
                {
                  "text": "coalesced_group简介",
                  "link": "/api/SIMT-API/协作组/coalesced_group/coalesced_group简介"
                },
                {
                  "text": "coalesced_group构造函数",
                  "link": "/api/SIMT-API/协作组/coalesced_group/coalesced_group构造函数"
                },
                {
                  "text": "sync",
                  "link": "/api/SIMT-API/协作组/coalesced_group/sync"
                },
                {
                  "text": "size",
                  "link": "/api/SIMT-API/协作组/coalesced_group/size"
                },
                {
                  "text": "num_threads",
                  "link": "/api/SIMT-API/协作组/coalesced_group/num_threads"
                },
                {
                  "text": "thread_rank",
                  "link": "/api/SIMT-API/协作组/coalesced_group/thread_rank"
                },
                {
                  "text": "meta_group_rank",
                  "link": "/api/SIMT-API/协作组/coalesced_group/meta_group_rank"
                },
                {
                  "text": "meta_group_size",
                  "link": "/api/SIMT-API/协作组/coalesced_group/meta_group_size"
                },
                {
                  "text": "shfl",
                  "link": "/api/SIMT-API/协作组/coalesced_group/shfl"
                },
                {
                  "text": "shfl_up",
                  "link": "/api/SIMT-API/协作组/coalesced_group/shfl_up"
                },
                {
                  "text": "shfl_down",
                  "link": "/api/SIMT-API/协作组/coalesced_group/shfl_down"
                },
                {
                  "text": "ballot",
                  "link": "/api/SIMT-API/协作组/coalesced_group/ballot"
                },
                {
                  "text": "any",
                  "link": "/api/SIMT-API/协作组/coalesced_group/any"
                },
                {
                  "text": "all",
                  "link": "/api/SIMT-API/协作组/coalesced_group/all"
                }
              ]
            },
            {
              "text": "thread_block_tile",
              "collapsed": true,
              "items": [
                {
                  "text": "thread_block_tile简介",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/thread_block_tile简介"
                },
                {
                  "text": "thread_block_tile构造函数",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/thread_block_tile构造函数"
                },
                {
                  "text": "sync",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/sync"
                },
                {
                  "text": "size",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/size"
                },
                {
                  "text": "num_threads",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/num_threads"
                },
                {
                  "text": "thread_rank",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/thread_rank"
                },
                {
                  "text": "meta_group_rank",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/meta_group_rank"
                },
                {
                  "text": "meta_group_size",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/meta_group_size"
                },
                {
                  "text": "shfl",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/shfl"
                },
                {
                  "text": "shfl_up",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/shfl_up"
                },
                {
                  "text": "shfl_down",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/shfl_down"
                },
                {
                  "text": "shfl_xor",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/shfl_xor"
                },
                {
                  "text": "ballot",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/ballot"
                },
                {
                  "text": "any",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/any"
                },
                {
                  "text": "all",
                  "link": "/api/SIMT-API/协作组/thread_block_tile/all"
                }
              ]
            }
          ]
        },
        {
          "text": "同步与内存栅栏",
          "collapsed": true,
          "items": [
            {
              "text": "同步接口",
              "collapsed": true,
              "items": [
                {
                  "text": "asc_syncthreads",
                  "link": "/api/SIMT-API/同步与内存栅栏/同步接口/asc_syncthreads"
                }
              ]
            },
            {
              "text": "内存栅栏接口",
              "collapsed": true,
              "items": [
                {
                  "text": "asc_threadfence",
                  "link": "/api/SIMT-API/同步与内存栅栏/内存栅栏接口/asc_threadfence"
                },
                {
                  "text": "asc_threadfence_block",
                  "link": "/api/SIMT-API/同步与内存栅栏/内存栅栏接口/asc_threadfence_block"
                }
              ]
            }
          ]
        },
        {
          "text": "原子操作",
          "collapsed": true,
          "items": [
            {
              "text": "asc_atomic_add",
              "link": "/api/SIMT-API/原子操作/asc_atomic_add"
            },
            {
              "text": "asc_atomic_sub",
              "link": "/api/SIMT-API/原子操作/asc_atomic_sub"
            },
            {
              "text": "asc_atomic_exch",
              "link": "/api/SIMT-API/原子操作/asc_atomic_exch"
            },
            {
              "text": "asc_atomic_max",
              "link": "/api/SIMT-API/原子操作/asc_atomic_max"
            },
            {
              "text": "asc_atomic_min",
              "link": "/api/SIMT-API/原子操作/asc_atomic_min"
            },
            {
              "text": "asc_atomic_inc",
              "link": "/api/SIMT-API/原子操作/asc_atomic_inc"
            },
            {
              "text": "asc_atomic_dec",
              "link": "/api/SIMT-API/原子操作/asc_atomic_dec"
            },
            {
              "text": "asc_atomic_cas",
              "link": "/api/SIMT-API/原子操作/asc_atomic_cas"
            },
            {
              "text": "asc_atomic_and",
              "link": "/api/SIMT-API/原子操作/asc_atomic_and"
            },
            {
              "text": "asc_atomic_or",
              "link": "/api/SIMT-API/原子操作/asc_atomic_or"
            },
            {
              "text": "asc_atomic_xor",
              "link": "/api/SIMT-API/原子操作/asc_atomic_xor"
            }
          ]
        },
        {
          "text": "Warp函数",
          "collapsed": true,
          "items": [
            {
              "text": "Warp Vote类函数",
              "collapsed": true,
              "items": [
                {
                  "text": "asc_all",
                  "link": "/api/SIMT-API/Warp函数/Warp-Vote类函数/asc_all"
                },
                {
                  "text": "asc_any",
                  "link": "/api/SIMT-API/Warp函数/Warp-Vote类函数/asc_any"
                },
                {
                  "text": "asc_ballot",
                  "link": "/api/SIMT-API/Warp函数/Warp-Vote类函数/asc_ballot"
                },
                {
                  "text": "asc_activemask",
                  "link": "/api/SIMT-API/Warp函数/Warp-Vote类函数/asc_activemask"
                }
              ]
            },
            {
              "text": "Warp Shfl类函数",
              "collapsed": true,
              "items": [
                {
                  "text": "asc_shfl",
                  "link": "/api/SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl"
                },
                {
                  "text": "asc_shfl_up",
                  "link": "/api/SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_up"
                },
                {
                  "text": "asc_shfl_down",
                  "link": "/api/SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_down"
                },
                {
                  "text": "asc_shfl_xor",
                  "link": "/api/SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_xor"
                }
              ]
            },
            {
              "text": "Warp Reduce类函数",
              "collapsed": true,
              "items": [
                {
                  "text": "asc_reduce_add",
                  "link": "/api/SIMT-API/Warp函数/Warp-Reduce类函数/asc_reduce_add"
                },
                {
                  "text": "asc_reduce_max",
                  "link": "/api/c_api/reg/reg_vector/asc_reduce_max"
                },
                {
                  "text": "asc_reduce_min",
                  "link": "/api/c_api/reg/reg_vector/asc_reduce_min"
                }
              ]
            }
          ]
        },
        {
          "text": "数学函数",
          "collapsed": true,
          "items": [
            {
              "text": "fp8类型",
              "collapsed": true,
              "items": [
                {
                  "text": "fp8数据类型简介",
                  "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据类型简介"
                },
                {
                  "text": "fp8数据转换函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__float22hif82_rna",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rna"
                    },
                    {
                      "text": "__float22hif82_rna_sat",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rna_sat"
                    },
                    {
                      "text": "__float22hif82_rh",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rh"
                    },
                    {
                      "text": "__float22hif82_rh_sat",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rh_sat"
                    },
                    {
                      "text": "__asc_cvt_float2_to_fp8x2",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__asc_cvt_float2_to_fp8x2"
                    },
                    {
                      "text": "__half22hif82_rna",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rna"
                    },
                    {
                      "text": "__half22hif82_rna_sat",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rna_sat"
                    },
                    {
                      "text": "__half22hif82_rh",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rh"
                    },
                    {
                      "text": "__half22hif82_rh_sat",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rh_sat"
                    },
                    {
                      "text": "__hif822float2",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__hif822float2"
                    },
                    {
                      "text": "__hif822half2",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__hif822half2"
                    },
                    {
                      "text": "__e4m3x22float2",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__e4m3x22float2"
                    },
                    {
                      "text": "__e5m2x22float2",
                      "link": "/api/SIMT-API/数学函数/fp8类型/fp8数据转换函数/__e5m2x22float2"
                    }
                  ]
                }
              ]
            },
            {
              "text": "half类型",
              "collapsed": true,
              "items": [
                {
                  "text": "half类型算术函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__habs",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__habs"
                    },
                    {
                      "text": "__hfma",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hfma"
                    },
                    {
                      "text": "__hadd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hadd"
                    },
                    {
                      "text": "__hsub",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hsub"
                    },
                    {
                      "text": "__hmul",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hmul"
                    },
                    {
                      "text": "__hdiv",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hdiv"
                    },
                    {
                      "text": "__hneg",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hneg"
                    },
                    {
                      "text": "__hfma_relu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型算术函数/__hfma_relu"
                    }
                  ]
                },
                {
                  "text": "half类型比较函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__hmax",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hmax"
                    },
                    {
                      "text": "__hmin",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hmin"
                    },
                    {
                      "text": "__hisnan",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hisnan"
                    },
                    {
                      "text": "__hisinf",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hisinf"
                    },
                    {
                      "text": "__heq",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__heq"
                    },
                    {
                      "text": "__hne",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hne"
                    },
                    {
                      "text": "__hle",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hle"
                    },
                    {
                      "text": "__hge",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hge"
                    },
                    {
                      "text": "__hlt",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hlt"
                    },
                    {
                      "text": "__hgt",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hgt"
                    },
                    {
                      "text": "__hequ",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hequ"
                    },
                    {
                      "text": "__hneu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hneu"
                    },
                    {
                      "text": "__hleu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hleu"
                    },
                    {
                      "text": "__hgeu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hgeu"
                    },
                    {
                      "text": "__hltu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hltu"
                    },
                    {
                      "text": "__hgtu",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hgtu"
                    },
                    {
                      "text": "__hmax_nan",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hmax_nan"
                    },
                    {
                      "text": "__hmin_nan",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型比较函数/__hmin_nan"
                    }
                  ]
                },
                {
                  "text": "half类型数学库函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "htanh",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/htanh"
                    },
                    {
                      "text": "hexp",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hexp"
                    },
                    {
                      "text": "hexp2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hexp2"
                    },
                    {
                      "text": "hexp10",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hexp10"
                    },
                    {
                      "text": "hlog",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hlog"
                    },
                    {
                      "text": "hlog2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hlog2"
                    },
                    {
                      "text": "hlog10",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hlog10"
                    },
                    {
                      "text": "hcos",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hcos"
                    },
                    {
                      "text": "hsin",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hsin"
                    },
                    {
                      "text": "hsqrt",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hsqrt"
                    },
                    {
                      "text": "hrsqrt",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hrsqrt"
                    },
                    {
                      "text": "hrcp",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hrcp"
                    },
                    {
                      "text": "hrint",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hrint"
                    },
                    {
                      "text": "hfloor",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hfloor"
                    },
                    {
                      "text": "hceil",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/hceil"
                    },
                    {
                      "text": "htrunc",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型数学库函数/htrunc"
                    }
                  ]
                },
                {
                  "text": "half类型精度转换函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__float2half",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half"
                    },
                    {
                      "text": "__float2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rn"
                    },
                    {
                      "text": "__float2half_rn_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rn_sat"
                    },
                    {
                      "text": "__float22half2_rn_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rn_sat"
                    },
                    {
                      "text": "__float2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rz"
                    },
                    {
                      "text": "__float2half_rz_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rz_sat"
                    },
                    {
                      "text": "__float22half2_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rz"
                    },
                    {
                      "text": "__float22half2_rz_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rz_sat"
                    },
                    {
                      "text": "__float2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rd"
                    },
                    {
                      "text": "__float2half_rd_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rd_sat"
                    },
                    {
                      "text": "__float22half2_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rd"
                    },
                    {
                      "text": "__float22half2_rd_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rd_sat"
                    },
                    {
                      "text": "__float2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ru"
                    },
                    {
                      "text": "__float2half_ru_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ru_sat"
                    },
                    {
                      "text": "__float22half2_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ru"
                    },
                    {
                      "text": "__float22half2_ru_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ru_sat"
                    },
                    {
                      "text": "__float2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rna"
                    },
                    {
                      "text": "__float2half_rna_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rna_sat"
                    },
                    {
                      "text": "__float22half2_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rna"
                    },
                    {
                      "text": "__float22half2_rna_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rna_sat"
                    },
                    {
                      "text": "__float2half_ro",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ro"
                    },
                    {
                      "text": "__float2half_ro_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ro_sat"
                    },
                    {
                      "text": "__float22half2_ro",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ro"
                    },
                    {
                      "text": "__float22half2_ro_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ro_sat"
                    },
                    {
                      "text": "__half2float",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2float"
                    },
                    {
                      "text": "__half2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rn"
                    },
                    {
                      "text": "__half2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rz"
                    },
                    {
                      "text": "__half2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rd"
                    },
                    {
                      "text": "__half2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_ru"
                    },
                    {
                      "text": "__half2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rna"
                    },
                    {
                      "text": "__half2uint_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rn"
                    },
                    {
                      "text": "__half2uint_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rz"
                    },
                    {
                      "text": "__half2uint_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rd"
                    },
                    {
                      "text": "__half2uint_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_ru"
                    },
                    {
                      "text": "__half2uint_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rna"
                    },
                    {
                      "text": "__half2int_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rn"
                    },
                    {
                      "text": "__half2int_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rz"
                    },
                    {
                      "text": "__half2int_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rd"
                    },
                    {
                      "text": "__half2int_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_ru"
                    },
                    {
                      "text": "__half2ull_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rn"
                    },
                    {
                      "text": "__half2int_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rna"
                    },
                    {
                      "text": "__half2ull_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rz"
                    },
                    {
                      "text": "__half2ull_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rd"
                    },
                    {
                      "text": "__half2ull_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_ru"
                    },
                    {
                      "text": "__half2ull_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rna"
                    },
                    {
                      "text": "__half2ll_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rn"
                    },
                    {
                      "text": "__half2ll_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rz"
                    },
                    {
                      "text": "__half2ll_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rd"
                    },
                    {
                      "text": "__half2ll_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_ru"
                    },
                    {
                      "text": "__half2ll_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rna"
                    },
                    {
                      "text": "__bfloat162half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rn"
                    },
                    {
                      "text": "__bfloat162half_rn_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rn_sat"
                    },
                    {
                      "text": "__bfloat162half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rz"
                    },
                    {
                      "text": "__bfloat162half_rz_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rz_sat"
                    },
                    {
                      "text": "__bfloat162half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rd"
                    },
                    {
                      "text": "__bfloat162half_rd_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rd_sat"
                    },
                    {
                      "text": "__bfloat162half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_ru"
                    },
                    {
                      "text": "__bfloat162half_ru_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_ru_sat"
                    },
                    {
                      "text": "__bfloat162half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rna"
                    },
                    {
                      "text": "__bfloat162half_rna_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rna_sat"
                    },
                    {
                      "text": "__uint2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rn"
                    },
                    {
                      "text": "__uint2half_rn_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rn_sat"
                    },
                    {
                      "text": "__uint2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rz"
                    },
                    {
                      "text": "__uint2half_rz_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rz_sat"
                    },
                    {
                      "text": "__uint2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rd"
                    },
                    {
                      "text": "__uint2half_rd_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rd_sat"
                    },
                    {
                      "text": "__uint2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_ru"
                    },
                    {
                      "text": "__uint2half_ru_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_ru_sat"
                    },
                    {
                      "text": "__uint2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rna"
                    },
                    {
                      "text": "__uint2half_rna_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rna_sat"
                    },
                    {
                      "text": "__int2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rn"
                    },
                    {
                      "text": "__int2half_rn_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rn_sat"
                    },
                    {
                      "text": "__int2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rz"
                    },
                    {
                      "text": "__int2half_rz_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rz_sat"
                    },
                    {
                      "text": "__int2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rd"
                    },
                    {
                      "text": "__int2half_rd_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rd_sat"
                    },
                    {
                      "text": "__int2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_ru"
                    },
                    {
                      "text": "__int2half_ru_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_ru_sat"
                    },
                    {
                      "text": "__int2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rna"
                    },
                    {
                      "text": "__int2half_rna_sat",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rna_sat"
                    },
                    {
                      "text": "__ull2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rn"
                    },
                    {
                      "text": "__ull2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rz"
                    },
                    {
                      "text": "__ull2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rd"
                    },
                    {
                      "text": "__ull2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_ru"
                    },
                    {
                      "text": "__ull2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rna"
                    },
                    {
                      "text": "__ll2half_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rn"
                    },
                    {
                      "text": "__ll2half_rz",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rz"
                    },
                    {
                      "text": "__ll2half_rd",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rd"
                    },
                    {
                      "text": "__ll2half_ru",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_ru"
                    },
                    {
                      "text": "__ll2half_rna",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rna"
                    },
                    {
                      "text": "__floats2half2_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__floats2half2_rn"
                    },
                    {
                      "text": "__float22half2_rn",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rn"
                    },
                    {
                      "text": "__low2half",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__low2half"
                    },
                    {
                      "text": "__low2half2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__low2half2"
                    },
                    {
                      "text": "__high2half",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__high2half"
                    },
                    {
                      "text": "__high2half2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__high2half2"
                    },
                    {
                      "text": "__highs2half2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__highs2half2"
                    },
                    {
                      "text": "__lows2half2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__lows2half2"
                    },
                    {
                      "text": "__halves2half2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__halves2half2"
                    },
                    {
                      "text": "__half22float2",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__half22float2"
                    },
                    {
                      "text": "__ushort_as_half",
                      "link": "/api/SIMT-API/数学函数/half类型/half类型精度转换函数/__ushort_as_half"
                    }
                  ]
                },
                {
                  "text": "half2类型算术函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__haddx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__haddx2"
                    },
                    {
                      "text": "__hsubx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hsubx2"
                    },
                    {
                      "text": "__hmulx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hmulx2"
                    },
                    {
                      "text": "__hdivx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hdivx2"
                    },
                    {
                      "text": "__habsx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__habsx2"
                    },
                    {
                      "text": "__hfmax2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hfmax2"
                    },
                    {
                      "text": "__hnegx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hnegx2"
                    },
                    {
                      "text": "__hfmax2_relu",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hfmax2_relu"
                    },
                    {
                      "text": "__hcmadd",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型算术函数/__hcmadd"
                    }
                  ]
                },
                {
                  "text": "half2类型比较函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__hbeqx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbeqx2"
                    },
                    {
                      "text": "__hbnex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbnex2"
                    },
                    {
                      "text": "__hblex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hblex2"
                    },
                    {
                      "text": "__hbgex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbgex2"
                    },
                    {
                      "text": "__hbltx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbltx2"
                    },
                    {
                      "text": "__hbgtx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbgtx2"
                    },
                    {
                      "text": "__hbequx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbequx2"
                    },
                    {
                      "text": "__hbneux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbneux2"
                    },
                    {
                      "text": "__hbleux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbleux2"
                    },
                    {
                      "text": "__hbgeux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbgeux2"
                    },
                    {
                      "text": "__hbltux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbltux2"
                    },
                    {
                      "text": "__hbgtux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hbgtux2"
                    },
                    {
                      "text": "__heqx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__heqx2"
                    },
                    {
                      "text": "__hnex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hnex2"
                    },
                    {
                      "text": "__hlex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hlex2"
                    },
                    {
                      "text": "__hgex2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgex2"
                    },
                    {
                      "text": "__hltx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hltx2"
                    },
                    {
                      "text": "__hgtx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgtx2"
                    },
                    {
                      "text": "__hequx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hequx2"
                    },
                    {
                      "text": "__hneux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hneux2"
                    },
                    {
                      "text": "__hleux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hleux2"
                    },
                    {
                      "text": "__hgeux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgeux2"
                    },
                    {
                      "text": "__hltux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hltux2"
                    },
                    {
                      "text": "__hgtux2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgtux2"
                    },
                    {
                      "text": "__heqx2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__heqx2_mask"
                    },
                    {
                      "text": "__hnex2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hnex2_mask"
                    },
                    {
                      "text": "__hlex2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hlex2_mask"
                    },
                    {
                      "text": "__hgex2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgex2_mask"
                    },
                    {
                      "text": "__hltx2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hltx2_mask"
                    },
                    {
                      "text": "__hgtx2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgtx2_mask"
                    },
                    {
                      "text": "__hequx2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hequx2_mask"
                    },
                    {
                      "text": "__hneux2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hneux2_mask"
                    },
                    {
                      "text": "__hleux2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hleux2_mask"
                    },
                    {
                      "text": "__hgeux2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgeux2_mask"
                    },
                    {
                      "text": "__hltux2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hltux2_mask"
                    },
                    {
                      "text": "__hgtux2_mask",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hgtux2_mask"
                    },
                    {
                      "text": "__isnanx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__isnanx2"
                    },
                    {
                      "text": "__hmaxx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hmaxx2"
                    },
                    {
                      "text": "__hmaxx2_nan",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hmaxx2_nan"
                    },
                    {
                      "text": "__hminx2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hminx2"
                    },
                    {
                      "text": "__hminx2_nan",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型比较函数/__hminx2_nan"
                    }
                  ]
                },
                {
                  "text": "half2类型数学库函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "h2tanh",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2tanh"
                    },
                    {
                      "text": "h2exp",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp"
                    },
                    {
                      "text": "h2exp2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp2"
                    },
                    {
                      "text": "h2exp10",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp10"
                    },
                    {
                      "text": "h2log",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2log"
                    },
                    {
                      "text": "h2log2",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2log2"
                    },
                    {
                      "text": "h2log10",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2log10"
                    },
                    {
                      "text": "h2cos",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2cos"
                    },
                    {
                      "text": "h2sin",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2sin"
                    },
                    {
                      "text": "h2sqrt",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2sqrt"
                    },
                    {
                      "text": "h2rsqrt",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2rsqrt"
                    },
                    {
                      "text": "h2rcp",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2rcp"
                    },
                    {
                      "text": "h2rint",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2rint"
                    },
                    {
                      "text": "h2floor",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2floor"
                    },
                    {
                      "text": "h2ceil",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2ceil"
                    },
                    {
                      "text": "h2trunc",
                      "link": "/api/SIMT-API/数学函数/half类型/half2类型数学库函数/h2trunc"
                    }
                  ]
                }
              ]
            },
            {
              "text": "bfloat16类型",
              "collapsed": true,
              "items": [
                {
                  "text": "bfloat16类型算术函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__habs",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__habs-150"
                    },
                    {
                      "text": "__hfma",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hfma-151"
                    },
                    {
                      "text": "__hadd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hadd-152"
                    },
                    {
                      "text": "__hsub",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hsub-153"
                    },
                    {
                      "text": "__hmul",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hmul-154"
                    },
                    {
                      "text": "__hdiv",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hdiv-155"
                    },
                    {
                      "text": "__hneg",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hneg-156"
                    },
                    {
                      "text": "__hfma_relu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hfma_relu-157"
                    }
                  ]
                },
                {
                  "text": "bfloat16类型比较函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__hmax",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmax-158"
                    },
                    {
                      "text": "__hmin",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmin-159"
                    },
                    {
                      "text": "__hisnan",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hisnan-160"
                    },
                    {
                      "text": "__hisinf",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hisinf-161"
                    },
                    {
                      "text": "__heq",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__heq-162"
                    },
                    {
                      "text": "__hne",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hne-163"
                    },
                    {
                      "text": "__hle",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hle-164"
                    },
                    {
                      "text": "__hge",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hge-165"
                    },
                    {
                      "text": "__hlt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hlt-166"
                    },
                    {
                      "text": "__hgt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgt-167"
                    },
                    {
                      "text": "__hequ",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hequ-168"
                    },
                    {
                      "text": "__hneu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hneu-169"
                    },
                    {
                      "text": "__hleu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hleu-170"
                    },
                    {
                      "text": "__hgeu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgeu-171"
                    },
                    {
                      "text": "__hltu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hltu-172"
                    },
                    {
                      "text": "__hgtu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgtu-173"
                    },
                    {
                      "text": "__hmax_nan",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmax_nan-174"
                    },
                    {
                      "text": "__hmin_nan",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmin_nan-175"
                    }
                  ]
                },
                {
                  "text": "bfloat16类型数学库函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "htanh",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/htanh-176"
                    },
                    {
                      "text": "hexp",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp-177"
                    },
                    {
                      "text": "hexp2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp2-178"
                    },
                    {
                      "text": "hexp10",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp10-179"
                    },
                    {
                      "text": "hlog",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog-180"
                    },
                    {
                      "text": "hlog2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog2-181"
                    },
                    {
                      "text": "hlog10",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog10-182"
                    },
                    {
                      "text": "hcos",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hcos-183"
                    },
                    {
                      "text": "hsin",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hsin-184"
                    },
                    {
                      "text": "hsqrt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hsqrt-185"
                    },
                    {
                      "text": "hrsqrt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrsqrt-186"
                    },
                    {
                      "text": "hrcp",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrcp-187"
                    },
                    {
                      "text": "hrint",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrint-188"
                    },
                    {
                      "text": "hfloor",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hfloor-189"
                    },
                    {
                      "text": "hceil",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hceil-190"
                    },
                    {
                      "text": "htrunc",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/htrunc-191"
                    }
                  ]
                },
                {
                  "text": "bfloat16类型精度转换函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__float2bfloat16",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16"
                    },
                    {
                      "text": "__float2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn"
                    },
                    {
                      "text": "__float2bfloat16_rn_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn_sat"
                    },
                    {
                      "text": "__float22bfloat162_rn_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn_sat"
                    },
                    {
                      "text": "__float2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz"
                    },
                    {
                      "text": "__float2bfloat16_rz_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz_sat"
                    },
                    {
                      "text": "__float22bfloat162_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz"
                    },
                    {
                      "text": "__float22bfloat162_rz_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz_sat"
                    },
                    {
                      "text": "__float2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd"
                    },
                    {
                      "text": "__float2bfloat16_rd_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd_sat"
                    },
                    {
                      "text": "__float22bfloat162_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd"
                    },
                    {
                      "text": "__float22bfloat162_rd_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd_sat"
                    },
                    {
                      "text": "__float2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru"
                    },
                    {
                      "text": "__float2bfloat16_ru_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru_sat"
                    },
                    {
                      "text": "__float22bfloat162_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru"
                    },
                    {
                      "text": "__float22bfloat162_ru_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru_sat"
                    },
                    {
                      "text": "__float2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna"
                    },
                    {
                      "text": "__float2bfloat16_rna_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna_sat"
                    },
                    {
                      "text": "__float22bfloat162_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna"
                    },
                    {
                      "text": "__float22bfloat162_rna_sat",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna_sat"
                    },
                    {
                      "text": "__half2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rn"
                    },
                    {
                      "text": "__half2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rz"
                    },
                    {
                      "text": "__half2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rd"
                    },
                    {
                      "text": "__half2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_ru"
                    },
                    {
                      "text": "__half2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rna"
                    },
                    {
                      "text": "__bfloat162float",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162float"
                    },
                    {
                      "text": "__bfloat162bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rn"
                    },
                    {
                      "text": "__bfloat162bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rz"
                    },
                    {
                      "text": "__bfloat162bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rd"
                    },
                    {
                      "text": "__bfloat162bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_ru"
                    },
                    {
                      "text": "__bfloat162bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rna"
                    },
                    {
                      "text": "__bfloat162uint_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rn"
                    },
                    {
                      "text": "__bfloat162uint_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rz"
                    },
                    {
                      "text": "__bfloat162uint_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rd"
                    },
                    {
                      "text": "__bfloat162uint_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_ru"
                    },
                    {
                      "text": "__bfloat162uint_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rna"
                    },
                    {
                      "text": "__bfloat162int_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rn"
                    },
                    {
                      "text": "__bfloat162int_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rz"
                    },
                    {
                      "text": "__bfloat162int_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rd"
                    },
                    {
                      "text": "__bfloat162int_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_ru"
                    },
                    {
                      "text": "__bfloat162int_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rna"
                    },
                    {
                      "text": "__bfloat162ull_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rn"
                    },
                    {
                      "text": "__bfloat162ull_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rz"
                    },
                    {
                      "text": "__bfloat162ull_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rd"
                    },
                    {
                      "text": "__bfloat162ull_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_ru"
                    },
                    {
                      "text": "__bfloat162ull_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rna"
                    },
                    {
                      "text": "__bfloat162ll_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rn"
                    },
                    {
                      "text": "__bfloat162ll_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rz"
                    },
                    {
                      "text": "__bfloat162ll_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rd"
                    },
                    {
                      "text": "__bfloat162ll_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_ru"
                    },
                    {
                      "text": "__bfloat162ll_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rna"
                    },
                    {
                      "text": "__uint2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rn"
                    },
                    {
                      "text": "__uint2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rz"
                    },
                    {
                      "text": "__uint2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rd"
                    },
                    {
                      "text": "__uint2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_ru"
                    },
                    {
                      "text": "__uint2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rna"
                    },
                    {
                      "text": "__int2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rn"
                    },
                    {
                      "text": "__int2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rz"
                    },
                    {
                      "text": "__int2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rd"
                    },
                    {
                      "text": "__int2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_ru"
                    },
                    {
                      "text": "__int2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rna"
                    },
                    {
                      "text": "__ull2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rn"
                    },
                    {
                      "text": "__ull2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rz"
                    },
                    {
                      "text": "__ull2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rd"
                    },
                    {
                      "text": "__ull2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_ru"
                    },
                    {
                      "text": "__ull2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rna"
                    },
                    {
                      "text": "__ll2bfloat16_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rn"
                    },
                    {
                      "text": "__ll2bfloat16_rz",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rz"
                    },
                    {
                      "text": "__ll2bfloat16_rd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rd"
                    },
                    {
                      "text": "__ll2bfloat16_ru",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_ru"
                    },
                    {
                      "text": "__ll2bfloat16_rna",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rna"
                    },
                    {
                      "text": "__float2bfloat162_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat162_rn"
                    },
                    {
                      "text": "__floats2bfloat162_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__floats2bfloat162_rn"
                    },
                    {
                      "text": "__float22bfloat162_rn",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn"
                    },
                    {
                      "text": "__bfloat162bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat162"
                    },
                    {
                      "text": "__halves2bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__halves2bfloat162"
                    },
                    {
                      "text": "__high2bfloat16",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat16"
                    },
                    {
                      "text": "__high2bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat162"
                    },
                    {
                      "text": "__high2float",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2float"
                    },
                    {
                      "text": "__highs2bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__highs2bfloat162"
                    },
                    {
                      "text": "__low2bfloat16",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat16"
                    },
                    {
                      "text": "__low2bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat162"
                    },
                    {
                      "text": "__low2float",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2float"
                    },
                    {
                      "text": "__lowhigh2highlow",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__lowhigh2highlow"
                    },
                    {
                      "text": "__lows2bfloat162",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__lows2bfloat162"
                    },
                    {
                      "text": "__bfloat1622float2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat1622float2"
                    },
                    {
                      "text": "__ushort_as_bfloat16",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ushort_as_bfloat16"
                    }
                  ]
                },
                {
                  "text": "bfloat16x2类型算术函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__haddx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__haddx2-192"
                    },
                    {
                      "text": "__hsubx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hsubx2-193"
                    },
                    {
                      "text": "__hmulx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hmulx2-194"
                    },
                    {
                      "text": "__hdivx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hdivx2-195"
                    },
                    {
                      "text": "__habsx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__habsx2-196"
                    },
                    {
                      "text": "__hfmax2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2-197"
                    },
                    {
                      "text": "__hnegx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hnegx2-198"
                    },
                    {
                      "text": "__hfmax2_relu",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2_relu-199"
                    },
                    {
                      "text": "__hcmadd",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hcmadd-200"
                    }
                  ]
                },
                {
                  "text": "bfloat16x2类型比较函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__hbeqx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbeqx2-201"
                    },
                    {
                      "text": "__hbnex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbnex2-202"
                    },
                    {
                      "text": "__hblex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hblex2-203"
                    },
                    {
                      "text": "__hbgex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgex2-204"
                    },
                    {
                      "text": "__hbltx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltx2-205"
                    },
                    {
                      "text": "__hbgtx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtx2-206"
                    },
                    {
                      "text": "__hbequx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbequx2-207"
                    },
                    {
                      "text": "__hbneux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbneux2-208"
                    },
                    {
                      "text": "__hbleux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbleux2-209"
                    },
                    {
                      "text": "__hbgeux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgeux2-210"
                    },
                    {
                      "text": "__hbltux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltux2-211"
                    },
                    {
                      "text": "__hbgtux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtux2-212"
                    },
                    {
                      "text": "__heqx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2-213"
                    },
                    {
                      "text": "__hnex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2-214"
                    },
                    {
                      "text": "__hlex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2-215"
                    },
                    {
                      "text": "__hgex2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2-216"
                    },
                    {
                      "text": "__hltx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2-217"
                    },
                    {
                      "text": "__hgtx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2-218"
                    },
                    {
                      "text": "__hequx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2-219"
                    },
                    {
                      "text": "__hneux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2-220"
                    },
                    {
                      "text": "__hleux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2-221"
                    },
                    {
                      "text": "__hgeux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2-222"
                    },
                    {
                      "text": "__hltux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2-223"
                    },
                    {
                      "text": "__hgtux2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2-224"
                    },
                    {
                      "text": "__heqx2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2_mask-225"
                    },
                    {
                      "text": "__hnex2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2_mask-226"
                    },
                    {
                      "text": "__hlex2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2_mask-227"
                    },
                    {
                      "text": "__hgex2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2_mask-228"
                    },
                    {
                      "text": "__hltx2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2_mask-229"
                    },
                    {
                      "text": "__hgtx2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2_mask-230"
                    },
                    {
                      "text": "__hequx2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2_mask-231"
                    },
                    {
                      "text": "__hneux2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2_mask-232"
                    },
                    {
                      "text": "__hleux2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2_mask-233"
                    },
                    {
                      "text": "__hgeux2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2_mask-234"
                    },
                    {
                      "text": "__hltux2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2_mask-235"
                    },
                    {
                      "text": "__hgtux2_mask",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2_mask-236"
                    },
                    {
                      "text": "__isnanx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__isnanx2-237"
                    },
                    {
                      "text": "__hmaxx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2-238"
                    },
                    {
                      "text": "__hmaxx2_nan",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2_nan-239"
                    },
                    {
                      "text": "__hminx2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2-240"
                    },
                    {
                      "text": "__hminx2_nan",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2_nan-241"
                    }
                  ]
                },
                {
                  "text": "bfloat16x2类型数学库函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "h2tanh",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2tanh-242"
                    },
                    {
                      "text": "h2exp",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp-243"
                    },
                    {
                      "text": "h2exp2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp2-244"
                    },
                    {
                      "text": "h2exp10",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp10-245"
                    },
                    {
                      "text": "h2log",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log-246"
                    },
                    {
                      "text": "h2log2",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log2-247"
                    },
                    {
                      "text": "h2log10",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log10-248"
                    },
                    {
                      "text": "h2cos",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2cos-249"
                    },
                    {
                      "text": "h2sin",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sin-250"
                    },
                    {
                      "text": "h2sqrt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sqrt-251"
                    },
                    {
                      "text": "h2rsqrt",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rsqrt-252"
                    },
                    {
                      "text": "h2rcp",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rcp-253"
                    },
                    {
                      "text": "h2rint",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rint-254"
                    },
                    {
                      "text": "h2floor",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2floor-255"
                    },
                    {
                      "text": "h2ceil",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2ceil-256"
                    },
                    {
                      "text": "h2trunc",
                      "link": "/api/SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2trunc-257"
                    }
                  ]
                }
              ]
            },
            {
              "text": "float类型数学库函数",
              "collapsed": true,
              "items": [
                {
                  "text": "tanf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/tanf"
                },
                {
                  "text": "tanhf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/tanhf"
                },
                {
                  "text": "tanpif",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/tanpif"
                },
                {
                  "text": "atanf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/atanf"
                },
                {
                  "text": "atan2f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/atan2f"
                },
                {
                  "text": "atanhf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/atanhf"
                },
                {
                  "text": "expf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/expf"
                },
                {
                  "text": "exp2f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/exp2f"
                },
                {
                  "text": "exp10f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/exp10f"
                },
                {
                  "text": "expm1f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/expm1f"
                },
                {
                  "text": "logf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/logf"
                },
                {
                  "text": "log2f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/log2f"
                },
                {
                  "text": "log10f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/log10f"
                },
                {
                  "text": "log1pf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/log1pf"
                },
                {
                  "text": "logbf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/logbf"
                },
                {
                  "text": "ilogbf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/ilogbf"
                },
                {
                  "text": "cosf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/cosf"
                },
                {
                  "text": "coshf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/coshf"
                },
                {
                  "text": "cospif",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/cospif"
                },
                {
                  "text": "acosf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/acosf"
                },
                {
                  "text": "acoshf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/acoshf"
                },
                {
                  "text": "sinf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sinf"
                },
                {
                  "text": "sinhf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sinhf"
                },
                {
                  "text": "sinpif",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sinpif"
                },
                {
                  "text": "asinf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/asinf"
                },
                {
                  "text": "asinhf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/asinhf"
                },
                {
                  "text": "sincosf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sincosf"
                },
                {
                  "text": "sincospif",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sincospif"
                },
                {
                  "text": "frexpf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/frexpf"
                },
                {
                  "text": "ldexpf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/ldexpf"
                },
                {
                  "text": "sqrtf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/sqrtf"
                },
                {
                  "text": "rsqrtf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rsqrtf"
                },
                {
                  "text": "hypotf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/hypotf"
                },
                {
                  "text": "rhypotf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rhypotf"
                },
                {
                  "text": "powf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/powf"
                },
                {
                  "text": "norm3df",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/norm3df"
                },
                {
                  "text": "rnorm3df",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rnorm3df"
                },
                {
                  "text": "norm4df",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/norm4df"
                },
                {
                  "text": "rnorm4df",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rnorm4df"
                },
                {
                  "text": "normf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/normf"
                },
                {
                  "text": "rnormf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rnormf"
                },
                {
                  "text": "cbrtf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/cbrtf"
                },
                {
                  "text": "rcbrtf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rcbrtf"
                },
                {
                  "text": "erff",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/erff"
                },
                {
                  "text": "erfcf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/erfcf"
                },
                {
                  "text": "erfinvf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/erfinvf"
                },
                {
                  "text": "erfcinvf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/erfcinvf"
                },
                {
                  "text": "erfcxf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/erfcxf"
                },
                {
                  "text": "tgammaf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/tgammaf"
                },
                {
                  "text": "lgammaf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/lgammaf"
                },
                {
                  "text": "cyl_bessel_i0f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/cyl_bessel_i0f"
                },
                {
                  "text": "cyl_bessel_i1f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/cyl_bessel_i1f"
                },
                {
                  "text": "normcdff",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/normcdff"
                },
                {
                  "text": "normcdfinvf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/normcdfinvf"
                },
                {
                  "text": "j0f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/j0f"
                },
                {
                  "text": "j1f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/j1f"
                },
                {
                  "text": "jnf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/jnf"
                },
                {
                  "text": "y0f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/y0f"
                },
                {
                  "text": "y1f",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/y1f"
                },
                {
                  "text": "ynf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/ynf"
                },
                {
                  "text": "fabsf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fabsf"
                },
                {
                  "text": "fmaf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fmaf"
                },
                {
                  "text": "fmaxf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fmaxf"
                },
                {
                  "text": "fminf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fminf"
                },
                {
                  "text": "fdimf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fdimf"
                },
                {
                  "text": "remquof",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/remquof"
                },
                {
                  "text": "fmodf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fmodf"
                },
                {
                  "text": "remainderf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/remainderf"
                },
                {
                  "text": "copysignf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/copysignf"
                },
                {
                  "text": "nearbyintf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/nearbyintf"
                },
                {
                  "text": "nextafterf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/nextafterf"
                },
                {
                  "text": "scalbnf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/scalbnf"
                },
                {
                  "text": "scalblnf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/scalblnf"
                },
                {
                  "text": "modff",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/modff"
                },
                {
                  "text": "fdividef",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/fdividef"
                },
                {
                  "text": "signbit",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/signbit"
                },
                {
                  "text": "__saturatef",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/__saturatef"
                },
                {
                  "text": "__fdividef",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/__fdividef"
                },
                {
                  "text": "rintf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/rintf"
                },
                {
                  "text": "lrintf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/lrintf"
                },
                {
                  "text": "llrintf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/llrintf"
                },
                {
                  "text": "roundf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/roundf"
                },
                {
                  "text": "lroundf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/lroundf"
                },
                {
                  "text": "llroundf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/llroundf"
                },
                {
                  "text": "floorf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/floorf"
                },
                {
                  "text": "ceilf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/ceilf"
                },
                {
                  "text": "truncf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/truncf"
                },
                {
                  "text": "isfinite",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/isfinite1"
                },
                {
                  "text": "isnan",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/isnan1"
                },
                {
                  "text": "isinf",
                  "link": "/api/SIMT-API/数学函数/float类型数学库函数/isinf1"
                }
              ]
            },
            {
              "text": "数据类型转换",
              "collapsed": true,
              "items": [
                {
                  "text": "概述",
                  "link": "/api/SIMT-API/数学函数/数据类型转换/概述-258"
                },
                {
                  "text": "类型转换函数",
                  "collapsed": true,
                  "items": [
                    {
                      "text": "__float2float_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rn"
                    },
                    {
                      "text": "__float2float_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rz"
                    },
                    {
                      "text": "__float2float_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rd"
                    },
                    {
                      "text": "__float2float_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_ru"
                    },
                    {
                      "text": "__float2float_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rna"
                    },
                    {
                      "text": "__float2uint_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rn"
                    },
                    {
                      "text": "__float2uint_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rz"
                    },
                    {
                      "text": "__float2uint_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rd"
                    },
                    {
                      "text": "__float2uint_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_ru"
                    },
                    {
                      "text": "__float2uint_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rna"
                    },
                    {
                      "text": "__float2int_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rn"
                    },
                    {
                      "text": "__float2int_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rz"
                    },
                    {
                      "text": "__float2int_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rd"
                    },
                    {
                      "text": "__float2int_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_ru"
                    },
                    {
                      "text": "__float2int_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rna"
                    },
                    {
                      "text": "__float2ull_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rn"
                    },
                    {
                      "text": "__float2ull_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rz"
                    },
                    {
                      "text": "__float2ull_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rd"
                    },
                    {
                      "text": "__float2ull_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_ru"
                    },
                    {
                      "text": "__float2ull_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rna"
                    },
                    {
                      "text": "__float2ll_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rn"
                    },
                    {
                      "text": "__float2ll_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rz"
                    },
                    {
                      "text": "__float2ll_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rd"
                    },
                    {
                      "text": "__float2ll_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_ru"
                    },
                    {
                      "text": "__float2ll_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rna"
                    },
                    {
                      "text": "__uint2float_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rn"
                    },
                    {
                      "text": "__uint2float_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rz"
                    },
                    {
                      "text": "__uint2float_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rd"
                    },
                    {
                      "text": "__uint2float_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_ru"
                    },
                    {
                      "text": "__uint2float_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rna"
                    },
                    {
                      "text": "__int2float_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rn"
                    },
                    {
                      "text": "__int2float_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rz"
                    },
                    {
                      "text": "__int2float_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rd"
                    },
                    {
                      "text": "__int2float_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_ru"
                    },
                    {
                      "text": "__int2float_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rna"
                    },
                    {
                      "text": "__ull2float_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rn"
                    },
                    {
                      "text": "__ull2float_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rz"
                    },
                    {
                      "text": "__ull2float_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rd"
                    },
                    {
                      "text": "__ull2float_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_ru"
                    },
                    {
                      "text": "__ull2float_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rna"
                    },
                    {
                      "text": "__ll2float_rn",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rn"
                    },
                    {
                      "text": "__ll2float_rz",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rz"
                    },
                    {
                      "text": "__ll2float_rd",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rd"
                    },
                    {
                      "text": "__ll2float_ru",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_ru"
                    },
                    {
                      "text": "__ll2float_rna",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rna"
                    },
                    {
                      "text": "__int_as_float",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__int_as_float"
                    },
                    {
                      "text": "__uint_as_float",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__uint_as_float"
                    },
                    {
                      "text": "__float_as_int",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float_as_int"
                    },
                    {
                      "text": "__float_as_uint",
                      "link": "/api/SIMT-API/数学函数/数据类型转换/类型转换函数/__float_as_uint"
                    }
                  ]
                }
              ]
            },
            {
              "text": "整型数学库函数",
              "collapsed": true,
              "items": [
                {
                  "text": "labs",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/labs"
                },
                {
                  "text": "llabs",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/llabs"
                },
                {
                  "text": "llmax",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/llmax"
                },
                {
                  "text": "ullmax",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/ullmax"
                },
                {
                  "text": "umax",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/umax"
                },
                {
                  "text": "llmin",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/llmin"
                },
                {
                  "text": "ullmin",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/ullmin"
                },
                {
                  "text": "umin",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/umin"
                },
                {
                  "text": "__mulhi",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__mulhi"
                },
                {
                  "text": "__umulhi",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__umulhi"
                },
                {
                  "text": "__mul64hi",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__mul64hi"
                },
                {
                  "text": "__umul64hi",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__umul64hi"
                },
                {
                  "text": "__mul_i32toi64",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__mul_i32toi64"
                },
                {
                  "text": "__brev",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__brev"
                },
                {
                  "text": "__clz",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__clz"
                },
                {
                  "text": "__ffs",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__ffs"
                },
                {
                  "text": "__popc",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__popc"
                },
                {
                  "text": "__byte_perm",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__byte_perm"
                },
                {
                  "text": "__sad",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__sad"
                },
                {
                  "text": "__usad",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__usad"
                },
                {
                  "text": "__mul24",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__mul24"
                },
                {
                  "text": "__umul24",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__umul24"
                },
                {
                  "text": "__hadd",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__hadd-259"
                },
                {
                  "text": "__rhadd",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__rhadd"
                },
                {
                  "text": "__uhadd",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__uhadd"
                },
                {
                  "text": "__urhadd",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/__urhadd"
                },
                {
                  "text": "max",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/max1"
                },
                {
                  "text": "min",
                  "link": "/api/SIMT-API/数学函数/整型数学库函数/min1"
                }
              ]
            }
          ]
        },
        {
          "text": "地址空间谓词函数",
          "collapsed": true,
          "items": [
            {
              "text": "__isGlobal",
              "link": "/api/SIMT-API/地址空间谓词函数/__isGlobal"
            },
            {
              "text": "__isUbuf",
              "link": "/api/SIMT-API/地址空间谓词函数/__isUbuf"
            },
            {
              "text": "__isLocal",
              "link": "/api/SIMT-API/地址空间谓词函数/__isLocal"
            }
          ]
        },
        {
          "text": "地址空间转换函数",
          "collapsed": true,
          "items": [
            {
              "text": "__cvta_generic_to_global",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_generic_to_global"
            },
            {
              "text": "__cvta_generic_to_ubuf",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_generic_to_ubuf"
            },
            {
              "text": "__cvta_generic_to_local",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_generic_to_local"
            },
            {
              "text": "__cvta_global_to_generic",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_global_to_generic"
            },
            {
              "text": "__cvta_ubuf_to_generic",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_ubuf_to_generic"
            },
            {
              "text": "__cvta_local_to_generic",
              "link": "/api/SIMT-API/地址空间转换函数/__cvta_local_to_generic"
            }
          ]
        },
        {
          "text": "访存函数",
          "collapsed": true,
          "items": [
            {
              "text": "asc_ldcg",
              "link": "/api/SIMT-API/访存函数/asc_ldcg"
            },
            {
              "text": "asc_ldca",
              "link": "/api/SIMT-API/访存函数/asc_ldca"
            },
            {
              "text": "asc_stcg",
              "link": "/api/SIMT-API/访存函数/asc_stcg"
            },
            {
              "text": "asc_stwt",
              "link": "/api/SIMT-API/访存函数/asc_stwt"
            }
          ]
        }
      ]
    },
    {
      "text": "Utils API",
      "collapsed": true,
      "items": [
        {
          "text": "Utils API列表",
          "link": "/api/Utils-API/Utils-API列表"
        },
        {
          "text": "C++标准库",
          "collapsed": true,
          "items": [
            {
              "text": "算法",
              "collapsed": true,
              "items": [
                {
                  "text": "max",
                  "link": "/api/Utils-API/CPP标准库/算法/max-267"
                },
                {
                  "text": "min",
                  "link": "/api/Utils-API/CPP标准库/算法/min-268"
                }
              ]
            },
            {
              "text": "数学函数",
              "collapsed": true,
              "items": [
                {
                  "text": "abs",
                  "link": "/api/Utils-API/CPP标准库/数学函数/abs1"
                },
                {
                  "text": "sqrt",
                  "link": "/api/Utils-API/CPP标准库/数学函数/sqrt1"
                }
              ]
            },
            {
              "text": "容器函数",
              "collapsed": true,
              "items": [
                {
                  "text": "tuple",
                  "link": "/api/Utils-API/CPP标准库/容器函数/tuple"
                },
                {
                  "text": "get",
                  "link": "/api/Utils-API/CPP标准库/容器函数/get1"
                },
                {
                  "text": "make_tuple",
                  "link": "/api/Utils-API/CPP标准库/容器函数/make_tuple"
                }
              ]
            },
            {
              "text": "类型特性",
              "collapsed": true,
              "items": [
                {
                  "text": "is_convertible",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_convertible"
                },
                {
                  "text": "is_base_of",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_base_of"
                },
                {
                  "text": "is_same",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_same"
                },
                {
                  "text": "is_void",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_void"
                },
                {
                  "text": "is_integral",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_integral"
                },
                {
                  "text": "is_floating_point",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_floating_point"
                },
                {
                  "text": "is_array",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_array"
                },
                {
                  "text": "is_pointer",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_pointer"
                },
                {
                  "text": "is_reference",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_reference"
                },
                {
                  "text": "is_const",
                  "link": "/api/Utils-API/CPP标准库/类型特性/is_const"
                },
                {
                  "text": "remove_const",
                  "link": "/api/Utils-API/CPP标准库/类型特性/remove_const"
                },
                {
                  "text": "remove_volatile",
                  "link": "/api/Utils-API/CPP标准库/类型特性/remove_volatile"
                },
                {
                  "text": "remove_cv",
                  "link": "/api/Utils-API/CPP标准库/类型特性/remove_cv"
                },
                {
                  "text": "remove_reference",
                  "link": "/api/Utils-API/CPP标准库/类型特性/remove_reference"
                },
                {
                  "text": "remove_pointer",
                  "link": "/api/Utils-API/CPP标准库/类型特性/remove_pointer"
                },
                {
                  "text": "add_const",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_const"
                },
                {
                  "text": "add_volatile",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_volatile"
                },
                {
                  "text": "add_cv",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_cv"
                },
                {
                  "text": "add_pointer",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_pointer"
                },
                {
                  "text": "add_lvalue_reference",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_lvalue_reference"
                },
                {
                  "text": "add_rvalue_reference",
                  "link": "/api/Utils-API/CPP标准库/类型特性/add_rvalue_reference"
                },
                {
                  "text": "enable_if",
                  "link": "/api/Utils-API/CPP标准库/类型特性/enable_if"
                },
                {
                  "text": "conditional",
                  "link": "/api/Utils-API/CPP标准库/类型特性/conditional"
                },
                {
                  "text": "integral_constant",
                  "link": "/api/Utils-API/CPP标准库/类型特性/integral_constant"
                }
              ]
            },
            {
              "text": "通用工具",
              "collapsed": true,
              "items": [
                {
                  "text": "integer_sequence",
                  "link": "/api/Utils-API/CPP标准库/通用工具/integer_sequence"
                }
              ]
            }
          ]
        },
        {
          "text": "平台信息获取",
          "collapsed": true,
          "items": [
            {
              "text": "PlatformAscendC",
              "collapsed": true,
              "items": [
                {
                  "text": "PlatformAscendC简介",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/PlatformAscendC简介"
                },
                {
                  "text": "构造及析构函数",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/构造及析构函数"
                },
                {
                  "text": "GetCoreNum",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreNum-270"
                },
                {
                  "text": "GetSocVersion",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetSocVersion"
                },
                {
                  "text": "GetCurNpuArch",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCurNpuArch"
                },
                {
                  "text": "GetVecRegLen",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetVecRegLen"
                },
                {
                  "text": "GetCoreNumAic",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreNumAic"
                },
                {
                  "text": "GetCoreNumAiv",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreNumAiv"
                },
                {
                  "text": "GetCoreNumVector",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreNumVector"
                },
                {
                  "text": "CalcTschNumBlocks",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/CalcTschNumBlocks"
                },
                {
                  "text": "GetCoreMemSize",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreMemSize"
                },
                {
                  "text": "GetCoreMemBw",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetCoreMemBw"
                },
                {
                  "text": "GetLibApiWorkSpaceSize",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetLibApiWorkSpaceSize"
                },
                {
                  "text": "GetResGroupBarrierWorkSpaceSize",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetResGroupBarrierWorkSpaceSize"
                },
                {
                  "text": "GetResCubeGroupWorkSpaceSize",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/GetResCubeGroupWorkSpaceSize"
                },
                {
                  "text": "ReserveLocalMemory",
                  "link": "/api/Utils-API/平台信息获取/PlatformAscendC/ReserveLocalMemory"
                }
              ]
            },
            {
              "text": "PlatformAscendCManager",
              "link": "/api/Utils-API/平台信息获取/PlatformAscendCManager"
            }
          ]
        },
        {
          "text": "原型注册与管理",
          "collapsed": true,
          "items": [
            {
              "text": "原型注册接口（OP_ADD）",
              "link": "/api/Utils-API/原型注册与管理/原型注册接口（OP_ADD）"
            },
            {
              "text": "OpAICoreConfig注册接口（REGISTER_OP_AICORE_CONFIG）",
              "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig注册接口（REGISTER_OP_AICORE_CONFIG）"
            },
            {
              "text": "OpDef",
              "collapsed": true,
              "items": [
                {
                  "text": "Input",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/Input"
                },
                {
                  "text": "Output",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/Output"
                },
                {
                  "text": "Attr",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/Attr"
                },
                {
                  "text": "SetInferShape",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/SetInferShape"
                },
                {
                  "text": "SetInferShapeRange",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/SetInferShapeRange"
                },
                {
                  "text": "SetInferDataType",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/SetInferDataType"
                },
                {
                  "text": "AICore",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/AICore"
                },
                {
                  "text": "AICPU",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/AICPU"
                },
                {
                  "text": "HostCPU",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/HostCPU"
                },
                {
                  "text": "MC2",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/MC2"
                },
                {
                  "text": "EnableFallBack",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/EnableFallBack"
                },
                {
                  "text": "Comment",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/Comment"
                },
                {
                  "text": "FormatMatchMode",
                  "link": "/api/Utils-API/原型注册与管理/OpDef/FormatMatchMode"
                }
              ]
            },
            {
              "text": "OpParamDef",
              "collapsed": true,
              "items": [
                {
                  "text": "构造&拷贝构造函数",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/构造-拷贝构造函数"
                },
                {
                  "text": "ParamType",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/ParamType"
                },
                {
                  "text": "DataType",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/DataType"
                },
                {
                  "text": "DataTypeForBinQuery",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/DataTypeForBinQuery"
                },
                {
                  "text": "DataTypeList",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/DataTypeList"
                },
                {
                  "text": "Format",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/Format"
                },
                {
                  "text": "FormatForBinQuery",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/FormatForBinQuery"
                },
                {
                  "text": "FormatList",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/FormatList"
                },
                {
                  "text": "UnknownShapeFormat（废弃）",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/UnknownShapeFormat（废弃）"
                },
                {
                  "text": "ValueDepend",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/ValueDepend"
                },
                {
                  "text": "IgnoreContiguous",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/IgnoreContiguous"
                },
                {
                  "text": "AutoContiguous",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/AutoContiguous"
                },
                {
                  "text": "Scalar",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/Scalar"
                },
                {
                  "text": "ScalarList",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/ScalarList"
                },
                {
                  "text": "To",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/To"
                },
                {
                  "text": "Version",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/Version"
                },
                {
                  "text": "InitValue",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/InitValue"
                },
                {
                  "text": "OutputShapeDependOnCompute",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/OutputShapeDependOnCompute"
                },
                {
                  "text": "Follow",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/Follow"
                },
                {
                  "text": "Comment",
                  "link": "/api/Utils-API/原型注册与管理/OpParamDef/Comment-271"
                }
              ]
            },
            {
              "text": "OpAttrDef",
              "collapsed": true,
              "items": [
                {
                  "text": "OpAttrDef",
                  "link": "/api/Utils-API/原型注册与管理/OpAttrDef/OpAttrDef-272"
                },
                {
                  "text": "Comment",
                  "link": "/api/Utils-API/原型注册与管理/OpAttrDef/Comment-273"
                }
              ]
            },
            {
              "text": "OpAICoreDef",
              "collapsed": true,
              "items": [
                {
                  "text": "SetTiling",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetTiling"
                },
                {
                  "text": "SetCheckSupport",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetCheckSupport"
                },
                {
                  "text": "SetOpSelectFormat",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetOpSelectFormat"
                },
                {
                  "text": "SetOpSupportInfo",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetOpSupportInfo"
                },
                {
                  "text": "SetOpSpecInfo",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetOpSpecInfo"
                },
                {
                  "text": "SetParamGeneralize",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/SetParamGeneralize"
                },
                {
                  "text": "AddConfig",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/AddConfig"
                },
                {
                  "text": "LaunchWithZeroEleOutputTensors",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreDef/LaunchWithZeroEleOutputTensors"
                }
              ]
            },
            {
              "text": "OpAICPUDef",
              "collapsed": true,
              "items": [
                {
                  "text": "Engine",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/Engine"
                },
                {
                  "text": "FlagPartial",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/FlagPartial"
                },
                {
                  "text": "ComputeCost",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/ComputeCost"
                },
                {
                  "text": "FlagAsync",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/FlagAsync"
                },
                {
                  "text": "OpkernelLib",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/OpkernelLib1"
                },
                {
                  "text": "KernelSo",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/KernelSo"
                },
                {
                  "text": "FunctionName",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/FunctionName"
                },
                {
                  "text": "UserDefined",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/UserDefined"
                },
                {
                  "text": "ExtendCfgInfo",
                  "link": "/api/Utils-API/原型注册与管理/OpAICPUDef/ExtendCfgInfo"
                }
              ]
            },
            {
              "text": "OpHostCPUDef",
              "collapsed": true,
              "items": [
                {
                  "text": "Engine",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/Engine-274"
                },
                {
                  "text": "FlagPartial",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/FlagPartial-275"
                },
                {
                  "text": "ComputeCost",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/ComputeCost-276"
                },
                {
                  "text": "FlagAsync",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/FlagAsync-277"
                },
                {
                  "text": "OpKernelLib",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/OpKernelLib"
                },
                {
                  "text": "KernelSo",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/KernelSo-278"
                },
                {
                  "text": "FunctionName",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/FunctionName-279"
                },
                {
                  "text": "UserDefined",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/UserDefined-280"
                },
                {
                  "text": "ExtendCfgInfo",
                  "link": "/api/Utils-API/原型注册与管理/OpHostCPUDef/ExtendCfgInfo-281"
                }
              ]
            },
            {
              "text": "OpAICoreConfig",
              "collapsed": true,
              "items": [
                {
                  "text": "OpAICoreConfig构造函数",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/OpAICoreConfig构造函数"
                },
                {
                  "text": "Input",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/Input-282"
                },
                {
                  "text": "Output",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/Output-283"
                },
                {
                  "text": "DynamicCompileStaticFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/DynamicCompileStaticFlag"
                },
                {
                  "text": "DynamicFormatFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/DynamicFormatFlag"
                },
                {
                  "text": "DynamicRankSupportFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/DynamicRankSupportFlag"
                },
                {
                  "text": "DynamicShapeSupportFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/DynamicShapeSupportFlag"
                },
                {
                  "text": "NeedCheckSupportFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/NeedCheckSupportFlag"
                },
                {
                  "text": "PrecisionReduceFlag",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/PrecisionReduceFlag"
                },
                {
                  "text": "ExtendCfgInfo",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/ExtendCfgInfo-284"
                },
                {
                  "text": "operator =",
                  "link": "/api/Utils-API/原型注册与管理/OpAICoreConfig/operator-285"
                }
              ]
            },
            {
              "text": "OpMC2Def",
              "collapsed": true,
              "items": [
                {
                  "text": "OpMC2Def简介",
                  "link": "/api/Utils-API/原型注册与管理/OpMC2Def/OpMC2Def简介"
                },
                {
                  "text": "OpMC2Def构造函数",
                  "link": "/api/Utils-API/原型注册与管理/OpMC2Def/OpMC2Def构造函数"
                },
                {
                  "text": "HcclGroup",
                  "link": "/api/Utils-API/原型注册与管理/OpMC2Def/HcclGroup"
                },
                {
                  "text": "HcclServerType",
                  "link": "/api/Utils-API/原型注册与管理/OpMC2Def/HcclServerType"
                },
                {
                  "text": "operator=",
                  "link": "/api/Utils-API/原型注册与管理/OpMC2Def/operator-286"
                }
              ]
            }
          ]
        },
        {
          "text": "Tiling数据结构注册",
          "collapsed": true,
          "items": [
            {
              "text": "TilingData结构定义",
              "link": "/api/Utils-API/Tiling数据结构注册/TilingData结构定义"
            },
            {
              "text": "TilingData结构注册",
              "link": "/api/Utils-API/Tiling数据结构注册/TilingData结构注册"
            }
          ]
        },
        {
          "text": "Tiling调测",
          "collapsed": true,
          "items": [
            {
              "text": "OpTilingRegistry",
              "collapsed": true,
              "items": [
                {
                  "text": "简介",
                  "link": "/api/Utils-API/Tiling调测/OpTilingRegistry/简介-287"
                },
                {
                  "text": "构造和析构函数",
                  "link": "/api/Utils-API/Tiling调测/OpTilingRegistry/构造和析构函数-288"
                },
                {
                  "text": "GetTilingFunc",
                  "link": "/api/Utils-API/Tiling调测/OpTilingRegistry/GetTilingFunc"
                },
                {
                  "text": "LoadTilingLibrary",
                  "link": "/api/Utils-API/Tiling调测/OpTilingRegistry/LoadTilingLibrary"
                }
              ]
            },
            {
              "text": "ContextBuilder",
              "collapsed": true,
              "items": [
                {
                  "text": "简介",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/简介-289"
                },
                {
                  "text": "ContextBuilder构造函数",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/ContextBuilder构造函数"
                },
                {
                  "text": "KernelRunContextHolder结构定义",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/KernelRunContextHolder结构定义"
                },
                {
                  "text": "Inputs",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/Inputs"
                },
                {
                  "text": "Outputs",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/Outputs"
                },
                {
                  "text": "BuildKernelRunContext",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/BuildKernelRunContext"
                },
                {
                  "text": "NodeIoNum",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/NodeIoNum"
                },
                {
                  "text": "SetOpNameType",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/SetOpNameType"
                },
                {
                  "text": "IrInstanceNum",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/IrInstanceNum"
                },
                {
                  "text": "AddInputTd",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/AddInputTd"
                },
                {
                  "text": "AddOutputTd",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/AddOutputTd"
                },
                {
                  "text": "AddAttr",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/AddAttr"
                },
                {
                  "text": "CompileInfo",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/CompileInfo"
                },
                {
                  "text": "PlatformInfo",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/PlatformInfo"
                },
                {
                  "text": "AddPlatformInfo",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/AddPlatformInfo"
                },
                {
                  "text": "TilingData",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/TilingData"
                },
                {
                  "text": "Workspace",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/Workspace"
                },
                {
                  "text": "BuildTilingContext",
                  "link": "/api/Utils-API/Tiling调测/ContextBuilder/BuildTilingContext"
                }
              ]
            }
          ]
        },
        {
          "text": "Tiling模板编程",
          "collapsed": true,
          "items": [
            {
              "text": "模板参数定义",
              "link": "/api/Utils-API/Tiling模板编程/模板参数定义"
            },
            {
              "text": "GET_TPL_TILING_KEY",
              "link": "/api/Utils-API/Tiling模板编程/GET_TPL_TILING_KEY"
            },
            {
              "text": "ASCENDC_TPL_SEL_PARAM",
              "link": "/api/Utils-API/Tiling模板编程/ASCENDC_TPL_SEL_PARAM"
            }
          ]
        },
        {
          "text": "Tiling下沉",
          "collapsed": true,
          "items": [
            {
              "text": "DEVICE_IMPL_OP_OPTILING",
              "link": "/api/Utils-API/Tiling下沉/DEVICE_IMPL_OP_OPTILING"
            }
          ]
        },
        {
          "text": "RTC",
          "collapsed": true,
          "items": [
            {
              "text": "RTC简介",
              "link": "/api/Utils-API/RTC/RTC简介"
            },
            {
              "text": "aclrtcCompileProg",
              "link": "/api/Utils-API/RTC/aclrtcCompileProg"
            },
            {
              "text": "aclrtcCreateProg",
              "link": "/api/Utils-API/RTC/aclrtcCreateProg"
            },
            {
              "text": "aclrtcDestroyProg",
              "link": "/api/Utils-API/RTC/aclrtcDestroyProg"
            },
            {
              "text": "aclrtcGetBinData",
              "link": "/api/Utils-API/RTC/aclrtcGetBinData"
            },
            {
              "text": "aclrtcGetBinDataSize",
              "link": "/api/Utils-API/RTC/aclrtcGetBinDataSize"
            },
            {
              "text": "aclrtcAddNameExpr",
              "link": "/api/Utils-API/RTC/aclrtcAddNameExpr"
            },
            {
              "text": "aclrtcGetLoweredName",
              "link": "/api/Utils-API/RTC/aclrtcGetLoweredName"
            },
            {
              "text": "aclrtcGetCompileLogSize",
              "link": "/api/Utils-API/RTC/aclrtcGetCompileLogSize"
            },
            {
              "text": "aclrtcGetCompileLog",
              "link": "/api/Utils-API/RTC/aclrtcGetCompileLog"
            },
            {
              "text": "RTC错误码",
              "link": "/api/Utils-API/RTC/RTC错误码"
            }
          ]
        },
        {
          "text": "log",
          "collapsed": true,
          "items": [
            {
              "text": "ASC_CPU_LOG",
              "link": "/api/Utils-API/log/ASC_CPU_LOG"
            }
          ]
        },
        {
          "text": "调测接口",
          "collapsed": true,
          "items": [
            {
              "text": "printf",
              "link": "/api/Utils-API/调测接口/printf-290"
            },
            {
              "text": "assert",
              "link": "/api/Utils-API/调测接口/assert-291"
            },
            {
              "text": "__trap",
              "link": "/api/Utils-API/调测接口/__trap"
            },
            {
              "text": "asc_dump",
              "link": "/api/Utils-API/调测接口/asc_dump"
            },
            {
              "text": "clock",
              "link": "/api/Utils-API/调测接口/clock"
            },
            {
              "text": "asc_time_stamp",
              "link": "/api/Utils-API/调测接口/asc_time_stamp"
            },
            {
              "text": "asc_prof_start",
              "link": "/api/Utils-API/调测接口/asc_prof_start"
            },
            {
              "text": "asc_prof_stop",
              "link": "/api/Utils-API/调测接口/asc_prof_stop"
            },
            {
              "text": "asc_mark_stamp",
              "link": "/api/Utils-API/调测接口/asc_mark_stamp"
            },
            {
              "text": "TRACE_START",
              "link": "/api/Utils-API/调测接口/TRACE_START"
            },
            {
              "text": "TRACE_STOP",
              "link": "/api/Utils-API/调测接口/TRACE_STOP"
            }
          ]
        },
        {
          "text": "SuperKernel",
          "collapsed": true,
          "items": [
            {
              "text": "SK_BIND",
              "link": "/api/Utils-API/SuperKernel/SK_BIND"
            }
          ]
        }
      ]
    },
    {
      "text": "AI CPU API",
      "collapsed": true,
      "items": [
        {
          "text": "AI CPU API列表",
          "link": "/api/AI-CPU-API/AI-CPU-API列表"
        },
        {
          "text": "printf",
          "link": "/api/AI-CPU-API/printf-292"
        },
        {
          "text": "assert",
          "link": "/api/AI-CPU-API/assert-293"
        },
        {
          "text": "DataStoreBarrier",
          "link": "/api/AI-CPU-API/DataStoreBarrier"
        }
      ]
    },
    {
      "text": "附录",
      "collapsed": true,
      "items": [
        {
          "text": "预留接口",
          "link": "/api/附录/预留接口"
        },
        {
          "text": "接口变更说明",
          "link": "/api/附录/接口变更说明"
        }
      ]
    },
    {
      "text": "README",
      "link": "/api/README"
    }
  ]))
}
