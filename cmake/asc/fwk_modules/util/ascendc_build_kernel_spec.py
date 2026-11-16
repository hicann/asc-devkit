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

"""Collect Kernel Spec resources and bind source installation paths for RUN delivery."""

import argparse
from dataclasses import dataclass, field
import fcntl
import hashlib
import json
import logging
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import tempfile
from kernel_spec_resources import BundleError, KernelSpecSourceMapper, _checked_path, _relative


def task_directory(root, current_op_type, soc):
    """Return the resource directory for one operator and chip in this build."""
    if not root or not os.path.isabs(root):
        raise ValueError("kernel-spec-dir must be an absolute path")
    return Path(root) / soc.lower() / current_op_type


def load_build_context(config_path):
    """Load the package's single kernel build configuration."""
    context = json.loads(Path(config_path).read_text(encoding="utf-8"))
    if not isinstance(context, dict):
        raise BundleError("build configuration must be a JSON object")
    return context


def _files(root, include_directories=False):
    root = _checked_path(root)
    if not root.is_dir():
        raise BundleError(f"directory is missing: {root}")

    def walk_error(error):
        raise error

    for directory, dirs, files in os.walk(root, onerror=walk_error):
        if len(Path(directory).relative_to(root).parts) > 64:
            raise BundleError(f"directory nesting exceeds 64: {directory}")
        dirs.sort()
        for name in sorted(dirs + files):
            path = _checked_path(Path(directory) / name)
            if name in dirs and include_directories:
                yield path
            elif name in files:
                if not path.is_file():
                    raise BundleError(f"non-regular file: {path}")
                yield path


def _hash_file(path):
    digest = hashlib.sha256()
    with open(path, "rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _tool(value):
    found = shutil.which(value)
    if not found:
        raise BundleError(f"executable tool is missing: {value}")
    return Path(found).resolve()


def _keep_temp(config):
    if not config:
        return False
    path = _checked_path(config)
    if not path.is_file():
        raise BundleError(f"OPC config is missing: {path}")
    for line in path.read_text(encoding="utf-8").splitlines():
        for option in line.split("@")[2:]:
            if option.startswith("--op_debug_config=") and "dump_cce" in option.split("=", 1)[1].replace(
                ",", ";"
            ).split(";"):
                return True
    return False


def _atomic_write(output, write):
    descriptor, name = tempfile.mkstemp(prefix="." + output.name + ".", dir=output.parent)
    try:
        with os.fdopen(descriptor, "wb") as stream:
            write(stream)
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(name, output)
        directory_fd = os.open(output.parent, os.O_RDONLY | os.O_DIRECTORY)
        try:
            os.fsync(directory_fd)
        finally:
            os.close(directory_fd)
    finally:
        if os.path.exists(name):
            os.unlink(name)


@dataclass
class ResourceUnit:
    manifest_path: Path
    manifest_json: bytes
    resource_root: Path
    installed_source: str
    source_hash: str


@dataclass
class KernelSpecBundleBuilder:
    """Collect one delivery's resources and export its shared library."""

    input_dir: list
    output: str
    work_dir: str
    exporter: str
    cxx: str
    make: str
    source_map: list = field(default_factory=list)
    source_file: list = field(default_factory=list)
    opc_config: str = None
    jobs: int = 8
    source_bindings: dict = field(default_factory=dict)

    def _collect(self, mapper):
        units = []
        source_hashes = {}
        bindings = self.source_bindings
        for directory in sorted(set(self.input_dir)):
            manifests = [p for p in _files(directory) if p.name.endswith("_manifest.json")]
            if not manifests:
                logging.warning("Kernel Spec: no manifests in input directory, skipping: %s", directory)
                continue
            for path in manifests:
                manifest = json.loads(path.read_text(encoding="utf-8"))
                resource_id = manifest.get("resource_id")
                source, package_source = mapper.resolve(
                    manifest.get("source_file"), bindings.get(str(_checked_path(directory)))
                )
                resource_root = _checked_path(path.parent / _relative(manifest.get("resource_path")))
                manifest["source_file"] = package_source
                encoded = json.dumps(manifest, sort_keys=True, separators=(",", ":"), allow_nan=False).encode()
                if source not in source_hashes:
                    source_hashes[source] = _hash_file(source)
                unit = ResourceUnit(path, encoded, resource_root, package_source, source_hashes[source])
                # Forward every candidate so exporter owns duplicate-ID selection.
                units.append((resource_id, unit))
        # Preserve valid-ID ordering and let exporter diagnose missing or malformed IDs.
        return sorted(units, key=lambda item: (item[0] if isinstance(item[0], str) else "", str(item[1].manifest_path)))

    @staticmethod
    def write_install_entry(install_script, output=None):
        if output is None:
            command = 'file(REMOVE "$ENV{DESTDIR}${spec_destination}/libcust_compile_resource.so")\n'
        else:
            output_string = str(output).replace("\\", "\\\\").replace('"', '\\"').replace("$", "\\$")
            # CMake's timestamp comparison can skip changed resources generated within the same second.
            command = (
                "unset(_spec_install_always)\n"
                "if(DEFINED ENV{CMAKE_INSTALL_ALWAYS})\n"
                '  set(_spec_install_always "$ENV{CMAKE_INSTALL_ALWAYS}")\n'
                "endif()\n"
                "set(ENV{CMAKE_INSTALL_ALWAYS} 1)\n"
                'file(INSTALL DESTINATION "${spec_destination}" TYPE FILE FILES "' + output_string + '")\n'
                "unset(ENV{CMAKE_INSTALL_ALWAYS})\n"
                "if(DEFINED _spec_install_always)\n"
                '  set(ENV{CMAKE_INSTALL_ALWAYS} "${_spec_install_always}")\n'
                "  unset(_spec_install_always)\n"
                "endif()\n"
            )
        install_script.parent.mkdir(parents=True, exist_ok=True)
        _atomic_write(install_script, lambda stream: stream.write(command.encode("utf-8")))

    def build(self, install_script=None):
        """Export resources and publish the optional installation entry only on success."""
        install = _checked_path(install_script) if install_script else None
        if install:
            install.unlink(missing_ok=True)
        output = _checked_path(self.output)
        if output.suffix != ".so":
            raise BundleError("RUN resource output must have the .so suffix")
        work = _checked_path(self.work_dir)
        for value in self.input_dir:
            root = _checked_path(value)
            if root == work or root in work.parents or root in output.parents:
                raise BundleError(f"work/output overlap manifest input directory: {root}")
        output.parent.mkdir(parents=True, exist_ok=True)
        work.mkdir(parents=True, exist_ok=True)
        # Output ownership, rather than work-dir ownership, serializes separate callers.
        lock_root = output.parent
        lock_root.mkdir(parents=True, exist_ok=True)
        with open(_checked_path(lock_root / ("." + output.name + ".lock")), "a+") as lock:
            fcntl.flock(lock.fileno(), fcntl.LOCK_EX)
            exported = self._export(output, work)
            if install:
                self.write_install_entry(install, output if exported else None)
            return exported

    @staticmethod
    def _resource_fingerprint(units, tools):
        fingerprint = hashlib.sha256()
        for name, tool in sorted(tools.items()):
            fingerprint.update((name + str(tool) + _hash_file(tool)).encode())
        for resource_id, unit in units:
            fingerprint.update(json.dumps([resource_id, unit.installed_source, unit.source_hash]).encode())
            fingerprint.update(unit.manifest_json)
            for path in _files(unit.resource_root, include_directories=True):
                relative = str(path.relative_to(unit.resource_root))
                if path.is_dir():
                    fingerprint.update(json.dumps(["directory", relative]).encode())
                else:
                    fingerprint.update(json.dumps(["file", relative, _hash_file(path)]).encode())
        return fingerprint.hexdigest()

    @staticmethod
    def _stage_resource_units(units, bundle_dir):
        collected = bundle_dir / "collected"
        for index, (_resource_id, unit) in enumerate(units):
            # Use private ordinal directories; resource IDs are data, never filesystem paths.
            destination = collected / f"{index:08d}"
            destination.mkdir(parents=True)
            resource_path = unit.resource_root.relative_to(unit.manifest_path.parent)
            shutil.copytree(unit.resource_root, destination / resource_path, symlinks=True)
            (destination / "resource_manifest.json").write_bytes(unit.manifest_json)
        return collected

    def _run_exporter(self, tools, bundle_dir, collected, output, keep):
        argv = [
            str(tools["exporter"]),
            "--input-dir",
            str(collected),
            "--output",
            str(output),
            "--cxx",
            str(tools["cxx"]),
            "--make",
            str(tools["make"]),
            "--jobs",
            str(self.jobs),
        ]
        if keep:
            argv.append("--save-temp-files")
        env = dict(os.environ, TMPDIR=str(bundle_dir))
        with open(bundle_dir / "build.log", "wb") as log:
            result = subprocess.run(argv, stdout=log, stderr=subprocess.STDOUT, env=env, check=False)
        if result.returncode:
            with open(bundle_dir / "build.log", "rb") as log:
                log.seek(max(0, log.seek(0, os.SEEK_END) - 8192))
                detail = log.read().decode("utf-8", errors="replace")
            raise BundleError(f"exporter failed ({result.returncode}): {detail}")

    def _export(self, output, work):
        mapper = KernelSpecSourceMapper(self.source_map, self.source_file)
        units = self._collect(mapper)
        if not units:
            output.unlink(missing_ok=True)
            output.with_suffix(output.suffix + ".cache.json").unlink(missing_ok=True)
            return False
        tools = {name: _tool(getattr(self, name)) for name in ("exporter", "cxx", "make")}
        keep = _keep_temp(self.opc_config)
        if self.jobs < 1:
            raise BundleError("jobs must be positive")
        signature = self._resource_fingerprint(units, tools)
        cache = _checked_path(output.with_suffix(output.suffix + ".cache.json"))
        if not keep and cache.is_file() and output.is_file():
            try:
                saved = json.loads(cache.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                saved = None
            if saved == {"input": signature, "output": _hash_file(output)}:
                return True
        bundle_dir = Path(tempfile.mkdtemp(prefix=".bundle-", dir=work))
        try:
            collected = self._stage_resource_units(units, bundle_dir)
            self._run_exporter(tools, bundle_dir, collected, output, keep)
            # asc_compile_exporter owns atomic publication and cross-filesystem handling.
            if not output.is_file():
                raise BundleError(f"exporter succeeded without publishing output: {output}")
            contents = json.dumps({"input": signature, "output": _hash_file(output)}).encode()
            _atomic_write(cache, lambda stream: stream.write(contents))
            return True
        finally:
            if keep:
                print(f"Kernel Spec diagnostics retained: {bundle_dir}")
            else:
                shutil.rmtree(bundle_dir)


def bundle_resources(build_config, work_dir, exporter, cxx, make):
    context = load_build_context(build_config)
    work = _checked_path(work_dir)
    install_script = work / "install.cmake"
    install_script.unlink(missing_ok=True)
    directory = work / "input"
    output = work / "op_spec/libcust_compile_resource.so"
    inputs = []
    source_bindings = {}
    source_root = Path(context["source_root"]).resolve()
    copied_root = Path(context["copied_source_root"])
    for op_type, soc in context["tasks"]:
        task_root = task_directory(str(directory), op_type, soc)
        receipt = task_root / ".producer-result.json"
        if not receipt.is_file():
            raise BundleError(f"Kernel Spec producer result is missing: {op_type}/{soc}")
        result = json.loads(receipt.read_text(encoding="utf-8"))
        status = result.get("status")
        if status == "filtered" or (status == "failed" and context.get("allow_failed") is True):
            logging.warning("Kernel Spec producer %s, skipping %s/%s: %s", status, op_type, soc, result["reason"])
            continue
        if status != "completed":
            raise BundleError(f"Kernel Spec producer {op_type}/{soc}: {status}: {result.get('reason', '')}")
        if not any(task_root.rglob("*_manifest.json")):
            raise BundleError(f"Kernel Spec producer completed without manifests: {op_type}/{soc}")
        source = Path(result["source"])
        relative = source.relative_to(source_root)
        # The source map describes the existing source installation, not a guessed new layout.
        if not (copied_root / relative).is_file():
            raise ValueError("Kernel Spec source is missing from the install input: " + str(source))
        input_root = str(_checked_path(task_root))
        source_bindings[input_root] = str(source)
        inputs.append(input_root)
    KernelSpecBundleBuilder(
        input_dir=inputs,
        source_map=context.get("source_maps") or [(context["source_root"], context["source_destination"])],
        output=str(output),
        work_dir=str(work / "work"),
        exporter=exporter,
        cxx=cxx,
        make=make,
        opc_config=context.get("opc_config"),
        source_bindings=source_bindings,
    ).build(install_script)


def main():
    """Build the resources produced by the package's kernel targets."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-config", required=True)
    parser.add_argument("--work-dir", required=True)
    for name in ("exporter", "cxx", "make"):
        parser.add_argument("--" + name, required=True)
    request = parser.parse_args()
    try:
        bundle_resources(request.build_config, request.work_dir, request.exporter, request.cxx, request.make)
    except (BundleError, OSError, ValueError, KeyError, TypeError, AttributeError) as error:
        parser.exit(1, f"Kernel Spec build failed: {error}\n")


if __name__ == "__main__":
    main()
