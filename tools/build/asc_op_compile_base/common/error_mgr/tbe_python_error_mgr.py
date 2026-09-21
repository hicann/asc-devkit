#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

TBE_DEFAULT_PYTHON_ERROR_CODE = "EB0500"


class TBEPythonError(RuntimeError):
    """
    Create new Error CompileError when cce compile error
    """

    def __init__(self, errorinfo):
        super().__init__()
        self.args = (
            errorinfo["errCode"],
            errorinfo["errClass"],
            errorinfo["errPcause"],
            errorinfo["errSolution"],
            (errorinfo["message"]),
        )
        self.errorinfo = errorinfo

    def __str__(self):
        if not isinstance(self.errorinfo, dict):
            return self.errorinfo
        out_str = ""
        if self.errorinfo.get("errClass") not in ("", " ", "N/A", None):
            out_str += "\n[errClass:{}]".format(self.errorinfo.get("errClass"))
        else:
            out_str += "\n[errClass:{}]".format("N/A")
        if self.errorinfo.get("errCode") not in ("", " ", "N/A", None):
            out_str += "\n[errCode:{}]".format(self.errorinfo.get("errCode"))
        else:
            out_str += "\n[errCode:{}]".format("N/A")
        if self.errorinfo.get("message") not in ("", " ", "N/A", None):
            out_str += "\n[message:{}]".format(self.errorinfo.get("message"))
        else:
            out_str += "\n[message:{}]".format("N/A")
        if self.errorinfo.get("errPcause") not in ("", " ", "N/A", None):
            out_str += "\n[errPcause:{}]".format(self.errorinfo.get("errPcause"))
        else:
            out_str += "\n[errPcause:{}]".format("N/A")
        if self.errorinfo.get("errSolution") not in ("", " ", "N/A", None):
            out_str += "\n[errSolution:{}]".format(self.errorinfo.get("errSolution"))
        else:
            out_str += "\n[errSolution:{}]".format("N/A")
        return out_str


def raise_tbe_python_err(err_code, msg):
    """
    packed tvm python error msg with errCode
    """
    if isinstance(msg, tuple) and len(msg) >= 2:
        if hasattr(msg[1], "errorinfo") and isinstance(msg[1].errorinfo, dict):
            msg = "{}\n{}".format(msg[0], msg[1].errorinfo.get("message"))
    args_dict = {"errCode": err_code, "message": msg, "errClass": "", "errPcause": "", "errSolution": ""}
    raise TBEPythonError(args_dict)
