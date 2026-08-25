/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { load as loadHtml } from 'cheerio'

const INLINE_CONTAINERS = new Set([
  'a', 'abbr', 'b', 'bdi', 'bdo', 'caption', 'cite', 'code', 'data', 'del',
  'dfn', 'em', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'i', 'ins', 'kbd',
  'label', 'li', 'mark', 'p', 'q', 's', 'samp', 'small', 'span', 'strong',
  'sub', 'sup', 'td', 'th', 'time', 'u', 'var',
])

const BLOCK_CONTENT_SELECTOR = [
  'address', 'article', 'aside', 'blockquote', 'details', 'dialog', 'div', 'dl',
  'dt', 'dd', 'fieldset', 'figcaption', 'figure', 'footer', 'form', 'h1', 'h2',
  'h3', 'h4', 'h5', 'h6', 'header', 'hr', 'li', 'main', 'menu', 'nav', 'ol',
  'p', 'pre', 'section', 'summary', 'table', 'tbody', 'td', 'tfoot', 'th',
  'thead', 'tr', 'ul',
].join(',')

function closestLayoutParent(element) {
  let parent = element.parent
  while (parent?.name === 'cann-filter') parent = parent.parent
  return parent?.name?.toLowerCase() || ''
}

export function renderCannFilterTags(html) {
  if (!/<cann-filter\b/i.test(html)) return html

  const $ = loadHtml(html, {
    xml: {
      xmlMode: false,
      decodeEntities: false,
      withStartIndices: false,
      withEndIndices: false,
    },
  }, false)

  const filters = $('cann-filter').toArray().reverse()
  for (const element of filters) {
    const npuType = element.attribs?.['npu-type'] || element.attribs?.npu_type
    if (!npuType) continue

    const parent = closestLayoutParent(element)
    const hasBlockContent = $(element).find(BLOCK_CONTENT_SELECTOR).length > 0
    element.name = INLINE_CONTAINERS.has(parent) && !hasBlockContent ? 'span' : 'div'
    element.attribs = { 'data-filter': npuType }
  }

  return $.html() || ''
}
