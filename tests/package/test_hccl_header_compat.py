#!/usr/bin/env python3
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Linux script regression; no CANN installation or NPU is required.
Run: python3 tests/package/test_hccl_header_compat.py
"""

from pathlib import Path
import os
import re
import sys
import shlex
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET

REPO = Path(__file__).resolve().parents[2]
SCRIPTS = REPO / "scripts/package/scripts"
FUNCTIONS = (SCRIPTS / "asc-devkit_func.sh").read_text().split("get_pkg_arch_name() {")[0]


class HeaderCompatibility(unittest.TestCase):
    def run_case(self, shell, case):
        with tempfile.TemporaryDirectory() as temp:
            base = Path(temp)
            root = base / "cann"
            if case == "heterogeneous":
                root = root / "aarch64-linux/hetero-arch"
            root.mkdir(parents=True)
            arch = "aarch64-linux"
            rel = ("../../" if case == "heterogeneous" else "") + arch + "/include/hccl/hccl.h"
            if case in ("bare_record", "same_arch_link", "cross_arch_link", "outside_link"):
                rel = "include/hccl/hccl.h"
            header = root / rel
            header.parent.mkdir(parents=True, exist_ok=True)
            if case in ("same_arch_link", "cross_arch_link", "outside_link"):
                import shutil

                shutil.rmtree(root / "include")
                destination = root / (arch if case == "same_arch_link" else "x86_64-linux") / "include"
                if case == "outside_link":
                    destination = base / "outside/include"
                (destination / "hccl").mkdir(parents=True, exist_ok=True)
                (root / "include").symlink_to(destination)
            payload = base / "extracted/.asc-devkit-compatibility/hccl/hccl.h"
            payload.parent.mkdir(parents=True)
            payload.write_text("repair payload\n")
            hccl = root / "share/info/hccl"
            if case not in ("no_hccl", "no_hccl_existing"):
                (hccl / "script").mkdir(parents=True)
                install_type = "run" if case == "runtime" else "devel" if case == "devel" else "full"
                (hccl / "ascend_install.info").write_text("Hccl_Install_Type=" + install_type + "\n")
                path = rel if case != "other_arch" else "x86_64-linux/include/hccl/hccl.h"
                rows = "include,copy,source," + path + ",FALSE,440,user:group,devel,NA,all,Y,FALSE,NA,HcclLib,NA,all\n"
                rows += (
                    "NA,mkdir,NA,"
                    + str(Path(path).parent)
                    + ",FALSE,550,user:group,devel,NA,all,Y,FALSE,NA,HcclLib,NA,all\n"
                )
                if case == "no_record":
                    rows = rows.replace("hccl.h,", "hccl_mc2.h,")
                if case == "directory_record":
                    rows = rows.replace(",copy,", ",mkdir,")
                (hccl / "script/filelist.csv").write_text(rows)
            if case in ("existing", "no_hccl_existing"):
                header.write_text("existing customer content\n")
                header.chmod(0o444)
            if case == "dangling_link":
                header.symlink_to("missing-target")
            if case in ("directory_mode", "legacy_directory"):
                manifest = hccl / "script/filelist.csv"
                rows = manifest.read_text()
                if case == "directory_mode":
                    rows = rows.replace(",550,", ",750,")
                else:
                    rows = rows.splitlines()[0] + "\n"
                manifest.write_text(rows)
            if case == "unknown_type":
                (hccl / "ascend_install.info").write_text("Hccl_Install_Type=unknown\n")
            if case == "missing_metadata":
                (hccl / "ascend_install.info").unlink()
            if case == "invalid_mode":
                manifest = hccl / "script/filelist.csv"
                manifest.write_text(manifest.read_text().replace(",440,", ",NA,"))
            if case == "missing_payload":
                payload.unlink()
            if case in ("missing_directory", "directory_mode", "legacy_directory"):
                header.parent.rmdir()
            else:
                header.parent.chmod(0o550)
            q = shlex.quote
            env = 'log() { echo "$*"; }\n'
            inject = "cp() { return 1; }\n" if case == "copy_failure" else ""
            hetero = "y" if case == "heterogeneous" else "n"
            cmd = "repair_missing_hccl_header " + " ".join(map(q, [str(root), str(payload), arch, hetero]))
            p = subprocess.run([shell, "-c", env + FUNCTIONS + inject + cmd], capture_output=True, text=True)
            failure = case in (
                "copy_failure",
                "missing_payload",
                "dangling_link",
                "cross_arch_link",
                "outside_link",
                "unknown_type",
                "missing_metadata",
                "invalid_mode",
            )
            self.assertEqual(p.returncode != 0, failure, p.stdout + p.stderr)
            expected = case not in ("no_hccl", "runtime", "no_record", "other_arch", "directory_record") and not failure
            self.assertEqual(header.is_file(), expected)
            if expected:
                old = case in ("existing", "no_hccl_existing")
                self.assertEqual(header.read_text(), "existing customer content\n" if old else "repair payload\n")
                self.assertEqual(header.stat().st_mode & 0o777, 0o444 if old else 0o440)
                # Repeated install must not rewrite an existing header.
                before = header.stat()
                p2 = subprocess.run([shell, "-c", env + FUNCTIONS + cmd], capture_output=True, text=True)
                self.assertEqual(p2.returncode, 0, p2.stderr)
                self.assertEqual(header.stat().st_ino, before.st_ino)
                self.assertEqual(header.stat().st_mtime_ns, before.st_mtime_ns)
            self.assertFalse(list(root.rglob(".hccl.h.*")))
            if header.parent.exists():
                self.assertEqual(header.parent.stat().st_mode & 0o777, 0o750 if case == "directory_mode" else 0o550)
            if failure:
                return
            # Simulate state change before uninstall, without source/ownership tracking.
            if case == "hccl_removed":
                import shutil

                shutil.rmtree(hccl)
            uninstall = "remove_hccl_header_without_hccl " + " ".join(map(q, [str(root), arch, hetero]))
            p = subprocess.run([shell, "-c", env + FUNCTIONS + uninstall], capture_output=True, text=True)
            self.assertEqual(p.returncode, 0, p.stdout + p.stderr)
            self.assertEqual(header.is_file(), expected and hccl.is_dir())
            # No repair payload is ever installed into a private directory.
            self.assertFalse((root / ".asc-devkit-compatibility").exists())
            self.assertFalse((root / "share/info/asc-devkit/compatibility").exists())

    def test_states(self):
        for shell in ("sh", "bash"):
            for case in (
                "full",
                "devel",
                "runtime",
                "bare_record",
                "no_hccl",
                "existing",
                "no_hccl_existing",
                "no_record",
                "other_arch",
                "directory_record",
                "heterogeneous",
                "missing_payload",
                "dangling_link",
                "copy_failure",
                "missing_directory",
                "hccl_removed",
                "same_arch_link",
                "cross_arch_link",
                "outside_link",
                "unknown_type",
                "missing_metadata",
                "invalid_mode",
                "directory_mode",
                "legacy_directory",
            ):
                with self.subTest(shell=shell, case=case):
                    self.run_case(shell, case)

    def test_legacy_manifest(self):
        for shell in ("sh", "bash"):
            for installed in (False, True):
                with self.subTest(shell=shell, hccl=installed), tempfile.TemporaryDirectory() as temp:
                    info = Path(temp) / "share/info/asc-devkit"
                    (info / "script").mkdir(parents=True)
                    if installed:
                        (info.parent / "hccl").mkdir()
                    manifest = info / "script/filelist.csv"
                    bad = "include,copy,src,aarch64-linux/include/hccl/hccl.h,FALSE,440\n"
                    good = "include,copy,src,aarch64-linux/include/hccl/hccl_mc2.h,FALSE,440\n"
                    manifest.write_text(bad + good)
                    manifest.chmod(0o440)
                    manifest.parent.chmod(0o550)
                    command = (
                        "log() { :; };\n"
                        + FUNCTIONS
                        + "\nprotect_hccl_header_from_legacy_filelist "
                        + shlex.quote(str(info))
                    )
                    p = subprocess.run([shell, "-c", command], capture_output=True, text=True)
                    self.assertEqual(p.returncode, 0, p.stderr)
                    self.assertEqual(manifest.read_text(), good if installed else bad + good)
                    self.assertEqual(manifest.parent.stat().st_mode & 0o777, 0o550)

    @unittest.skipUnless(os.environ.get("CANN_CMAKE_DIR"), "set CANN_CMAKE_DIR to test the real package installer")
    def test_common_installer_payload_exclusion(self):
        cmake = Path(os.environ["CANN_CMAKE_DIR"])
        sys.path.insert(0, str(cmake / "scripts/package"))
        import package

        xml = ET.parse(REPO / "scripts/package/module/ascend/AscHcclLib.xml").getroot()
        holder = next(node for node in xml.findall("file_info") if node.get("value") == "hccl_header_repair")
        config = dict(xml.attrib)
        config.update(holder.attrib)
        config.update(value="hccl.h", feature={"all"}, chip={"all"})
        rows = list(package.parse_install_info("/unused", [config], "copy", ["docker"]))
        # Old packers honor pkg_only; master-054 instead relies on install_type=NA.
        self.assertTrue(all(row.install_type == "NA" for row in rows))
        parser = (cmake / "scripts/install/install_common_parser.sh").read_text()
        copy_function = re.search(r"^do_copy_files\(\) \{.*?^\}", parser, re.M | re.S).group()
        common = (cmake / "scripts/install/common_func_v2.inc").read_text()
        filter_function = re.search(r"^filter_install_type\(\) \{.*?^\}", common, re.M | re.S).group()
        for shell in ("sh", "bash"):
            with self.subTest(shell=shell), tempfile.TemporaryDirectory() as temp:
                base = Path(temp)
                source, target = base / "source", base / "installed"
                payload = source / holder.get("dst_path") / "hccl.h"
                payload.parent.mkdir(parents=True)
                payload.write_text("repair payload\n")
                (source / "ordinary").write_text("ordinary package file\n")
                target.mkdir()
                command = "COPY_ALL=y; SET_CANN_UNINSTALL=n; PKG_ARCH=x86_64;\n" + copy_function
                command += "\ndo_copy_files full " + shlex.quote(str(target)) + " unused asc-devkit all"
                result = subprocess.run([shell, "-c", command], cwd=source, capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual([item.name for item in target.iterdir()], ["ordinary"])
                self.assertTrue(payload.is_file())
                (target / "ordinary").unlink()  # Manifest uninstalls the ordinary fixture file.
                self.assertEqual(list(target.iterdir()), [])
                for mode in ("full", "debug", "run", "devel", "docker"):
                    result = subprocess.run(
                        [shell, "-c", filter_function + "\nfilter_install_type " + mode],
                        input="repair,copy,src,dst,FALSE,440,owner,NA\n",
                        capture_output=True,
                        text=True,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertEqual(result.stdout, "")

    def test_package_only_and_hooks(self):
        root = ET.parse(REPO / "scripts/package/module/ascend/AscHcclLib.xml").getroot()
        holders = [node for node in root.findall("file_info") if node.find("file[@value='hccl.h']") is not None]
        self.assertEqual(len(holders), 1)
        self.assertEqual(holders[0].get("pkg_only"), "true")
        self.assertEqual(holders[0].get("install_type"), "NA")
        self.assertEqual(holders[0].get("install_path"), holders[0].get("dst_path"))
        self.assertEqual(holders[0].get("dst_path"), ".asc-devkit-compatibility/hccl")
        cmake = (REPO / "impl/adv_api/detail/hccl/cc/CMakeLists.txt").read_text()
        self.assertIn("DESTINATION .asc-devkit-compatibility/hccl", cmake)
        self.assertNotIn("DESTINATION ${CMAKE_SYSTEM_PROCESSOR}-linux/include/hccl", cmake)
        self.assertIn(
            'repair_missing_hccl_header "$common_parse_dir"', (SCRIPTS / "asc-devkit_custom_install.sh").read_text()
        )
        self.assertIn(
            'remove_hccl_header_without_hccl "$common_parse_dir"',
            (SCRIPTS / "asc-devkit_custom_uninstall.sh").read_text(),
        )
        install = (SCRIPTS / "install.sh").read_text()
        self.assertEqual(install.count("if ! protect_hccl_header_from_legacy_filelist"), 2)


if __name__ == "__main__":
    unittest.main(verbosity=2)
