/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

export const KERNEL_LAUNCH_TOKEN = '<<<>>>'

const KERNEL_LAUNCH_PATTERN = '<<<(?:[^<>\\r\\n]{0,256})>>>'
const KERNEL_LAUNCH_QUERY_PREFIX_PATTERN = /^<<<[^<>\r\n]{0,256}>{0,2}$/

function kernelLaunchRegex(flags = '') {
  return new RegExp(KERNEL_LAUNCH_PATTERN, flags)
}

export function kernelLaunchMatches(value) {
  return [...String(value || '').matchAll(kernelLaunchRegex('g'))]
    .map(match => ({ index: match.index, value: match[0] }))
}

export function findKernelLaunch(value) {
  const text = String(value || '')
  const exactIndex = text.indexOf(KERNEL_LAUNCH_TOKEN)
  if (exactIndex >= 0) return { index: exactIndex, value: KERNEL_LAUNCH_TOKEN }
  return kernelLaunchMatches(text)[0] || null
}

export function normalizeKernelLaunchQuery(value) {
  const normalized = String(value || '').normalize('NFKC').trim()
  if (KERNEL_LAUNCH_QUERY_PREFIX_PATTERN.test(normalized)) return KERNEL_LAUNCH_TOKEN
  const match = kernelLaunchMatches(normalized)[0]
  return match?.index === 0 && match.value.length === normalized.length
    ? KERNEL_LAUNCH_TOKEN
    : ''
}
