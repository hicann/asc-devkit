/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

const COMPACT_COLUMN_MAX_UNITS = 22
const HEADER_NOWRAP_MAX_UNITS = 32
const COMPLEX_CONTENT_SELECTOR = 'pre, ul, ol, dl, blockquote, details, figure, table, br, hr'

export function measureTextUnits(text) {
  let units = 0
  for (const character of String(text).trim()) {
    if (/\s/.test(character)) units += 0.35
    else if (/^[\x00-\x7f]$/.test(character)) units += 0.58
    else units += 1
  }
  return units
}

export function placeTableCells(rows) {
  const occupiedRows = []

  return rows.map(row => {
    const placements = []
    let column = 0

    for (const cell of row) {
      while (occupiedRows[column] > 0) column += 1

      const colSpan = Math.max(1, Number(cell.colSpan) || 1)
      const rowSpan = Math.max(1, Number(cell.rowSpan) || 1)
      placements.push({ cell, column, colSpan })

      if (rowSpan > 1) {
        for (let index = column; index < column + colSpan; index += 1) {
          occupiedRows[index] = Math.max(occupiedRows[index] || 0, rowSpan)
        }
      }
      column += colSpan
    }

    for (let index = 0; index < occupiedRows.length; index += 1) {
      if (occupiedRows[index] > 0) occupiedRows[index] -= 1
    }

    return placements
  })
}

export function classifyTableColumns(rows, maxUnits = COMPACT_COLUMN_MAX_UNITS) {
  const placedRows = placeTableCells(rows)
  const stats = []

  for (const placements of placedRows) {
    for (const { cell, column, colSpan } of placements) {
      for (let index = column; index < column + colSpan; index += 1) {
        stats[index] ||= { compact: true, seen: 0, preferredUnits: 0 }
      }

      // A spanning cell describes several columns and must not dictate any
      // individual column's width.
      if (colSpan !== 1) continue

      const stat = stats[column]
      const textUnits = measureTextUnits(cell.text)
      stat.seen += 1
      stat.preferredUnits = Math.max(stat.preferredUnits, textUnits)
      if (cell.hasComplexContent || textUnits > maxUnits) stat.compact = false
    }
  }

  return stats.map(stat => ({
    ...stat,
    compact: stat.compact && stat.seen > 0,
  }))
}

function describeTable(table) {
  return Array.from(table.rows, row => Array.from(row.cells, cell => ({
    element: cell,
    text: cell.textContent.replace(/\s+/g, ' ').trim(),
    colSpan: cell.colSpan,
    rowSpan: cell.rowSpan,
    hasComplexContent: Boolean(cell.querySelector(COMPLEX_CONTENT_SELECTOR)),
  })))
}

function addGeneratedColgroup(table, columns) {
  if (table.querySelector(':scope > colgroup')) return

  const colgroup = document.createElement('colgroup')
  colgroup.dataset.vpTableLayout = 'true'

  for (const column of columns) {
    const col = document.createElement('col')
    if (column.compact) {
      const width = Math.min(24, Math.max(4, Math.ceil(column.preferredUnits + 2)))
      col.className = 'vp-table-compact-column'
      col.style.setProperty('--vp-table-column-width', `${width}em`)
    }
    colgroup.append(col)
  }

  table.prepend(colgroup)
}

function wrapTable(table) {
  if (table.parentElement?.classList.contains('vp-table-scroll')) return

  const wrapper = document.createElement('div')
  wrapper.className = 'vp-table-scroll'
  table.before(wrapper)
  wrapper.append(table)
}

export function optimizeTable(table) {
  if (!(table instanceof HTMLTableElement) || table.dataset.vpTableOptimized) return
  table.dataset.vpTableOptimized = 'true'

  const rows = describeTable(table)
  const placedRows = placeTableCells(rows)
  const columns = classifyTableColumns(rows)
  const hasCompactColumn = columns.some(column => column.compact)
  const hasFlexibleColumn = columns.some(column => !column.compact)

  table.classList.add('vp-table-optimized')
  if (hasCompactColumn && hasFlexibleColumn) {
    table.classList.add('vp-table-mixed-layout')
    addGeneratedColgroup(table, columns)
  }

  for (const placements of placedRows) {
    for (const { cell, column, colSpan } of placements) {
      const element = cell.element
      if (colSpan === 1 && columns[column]?.compact) {
        element.classList.add('vp-table-compact-cell')
      }
      if (
        element.tagName === 'TH' &&
        !cell.hasComplexContent &&
        measureTextUnits(cell.text) <= HEADER_NOWRAP_MAX_UNITS
      ) {
        element.classList.add('vp-table-nowrap')
      }
    }
  }

  wrapTable(table)
}

export function optimizeDocumentTables(root = document) {
  const tables = Array.from(root.querySelectorAll('.vp-doc table')).reverse()
  for (const table of tables) optimizeTable(table)
}

export function installAutomaticTableLayout() {
  if (typeof document === 'undefined' || typeof MutationObserver === 'undefined') return

  let frame = null
  const schedule = () => {
    if (frame !== null) return
    const requestFrame = globalThis.requestAnimationFrame || (callback => setTimeout(callback, 0))
    frame = requestFrame(() => {
      frame = null
      optimizeDocumentTables()
    })
  }

  const start = () => {
    schedule()
    const observer = new MutationObserver(records => {
      const hasNewTable = records.some(record => Array.from(record.addedNodes).some(node =>
        node.nodeType === 1 && (node.matches?.('.vp-doc table') || node.querySelector?.('.vp-doc table'))
      ))
      if (hasNewTable) schedule()
    })
    observer.observe(document.body, { childList: true, subtree: true })
  }

  if (document.body) start()
  else document.addEventListener('DOMContentLoaded', start, { once: true })
}
