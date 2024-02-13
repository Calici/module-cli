import {
  TableCellT,
  TableType_ZoomableSortableT,
  TableT,
  TableCell_ZoomableI,
} from "interfaces/oneTableDisplay/v1";

class Cell{
  static isNumber = (
    cell : TableCellT, type : TableType_ZoomableSortableT
  ) : boolean => {
    return (
      ( typeof cell === 'string' && !isNaN(parseFloat(cell)) ) ||
      ( typeof cell === 'number' ) ||
      ( type.zoomable === 'normal' &&
        !isNaN(parseFloat((cell as TableCell_ZoomableI).shortText))
      )
    )
  }
}

type OpHistory = { name : string, object : ModuleTable }

export default class ModuleTable{
  table : TableT
  history : OpHistory[]
  index : number
  indexes : number[]
  hooks : (() => any)[]
  constructor(
    table : TableT,
    history : OpHistory[] = [],
    index : number = 0,
    indexes? : number[],
    hooks : (() => any)[] = []
  ){
    this.table = table
    this.history = history.length === 0 ?
      [{ name : "base", object : this}] : history
    this.index = index
    this.indexes = indexes === undefined ?
      table.rows.map((_, id) => id) : indexes
    this.hooks = hooks
  }
  createNext = (
    table : TableT,
    opName : string,
    indexes? : number[],
    hooks : typeof this.hooks = []
  ) : ModuleTable => {
    const moduleTable = new ModuleTable(
      table, this.history, this.index + 1,
      indexes === undefined ? this.indexes : indexes,
      [...this.hooks, ...hooks]
    )
    const history = { name : opName, object : moduleTable }
    if (this.history.length <= moduleTable.index)
      this.history.push(history)
    else
      this.history[moduleTable.index] = history
    moduleTable.hooks.forEach((hook) => hook())
    return moduleTable
  }
  autoFloatingPoint = () : ModuleTable => {
    const headers = [ ...this.table.headers ]
    const rows = [ ...this.table.rows ]
    if (rows.length === 0)
      return new ModuleTable({headers, rows}, this.history)
    const headersWithAutoFP = headers
      .map((header, id) => {
        if (
          (header.type.type !== 'string' && header.type.type !== 'number') ||
          header.type.fpAccuracy !== undefined
        )
          return header
        const cell = rows[0][id]
        if ( !Cell.isNumber(cell, header.type) ) return header
        const maxFloatingPoint = Math.max(
          ...rows.map((row) => {
            const cell = row[id]
            if (typeof cell === 'string'){
              const fp = cell.split('.')
              return fp.length > 1 ? fp[1].length : 0
            }
            else if (typeof cell === 'number'){
              const fp = cell.toString().split('.')
              return fp.length > 1 ? fp[1].length : 0
            }
            else if (typeof cell === 'object'){
              const fp = (cell as TableCell_ZoomableI).shortText.split('.')
              return fp.length > 1 ? fp[1].length : 0
            }
            else return 0
          })
        )
        return {
          ...header, type : { ...header.type, fpAccuracy : maxFloatingPoint}
        }
      })
    const table = { headers : headersWithAutoFP, rows }
    return this.createNext(table, "autoFP")
  }
  sort = (column : number, ascending : boolean) : ModuleTable => {
    const opName = `sort ${column} ${ascending}`
    const rowsWithId = this.table.rows.map((row, id) => ({row, id}))
    const headers = [ ...this.table.headers ]
    if (this.table.headers[column].type.type === 'id')
      return ascending ?
        this.createNext({headers, rows : this.table.rows.slice()}, opName) :
        this.createNext(
          {headers, rows : this.table.rows.slice().reverse()},
          opName,
          this.indexes.slice().reverse()
        )
    rowsWithId.sort(({row : a}, {row : b}) => {
      const valA = a[column];
      const valB = b[column];
      if (typeof valA === 'object' && typeof valB === 'object'){
        const convA = valA as TableCell_ZoomableI
        const convB = valB as TableCell_ZoomableI
        if (
          typeof convA.shortText === 'string' &&
          typeof convB.shortText === 'string'
        ){
          const numA = parseFloat(convA.shortText)
          const numB = parseFloat(convB.shortText)
          if (isNaN(numA) || isNaN(numB))
            return convA.shortText.localeCompare(convB.shortText)
          else
            return numA - numB
        }
        else if (
          typeof convA.shortText === 'number' &&
          typeof convB.shortText === 'number'
        )
          return convA.shortText - convB.shortText
        else return 0
      }
      else if (typeof valA === 'string' && typeof valB === 'string'){
        const numA = parseFloat(valA)
        const numB = parseFloat(valB)
        if (isNaN(numA) || isNaN(numB))
          return valA.localeCompare(valB);
        else
          return numA - numB
      }
      else if (typeof valA === 'number' && typeof valB === 'number'){
        return valA - valB
      }
      //TODO: THINK ABOUT REST OF THE CASE
      else return 0
    });
    const sortedRowsWithId = ascending ? rowsWithId : rowsWithId.reverse()
    const rows = sortedRowsWithId.map((row) => row.row)
    const ids = sortedRowsWithId.map((row) => row.id)
    return this.createNext( {headers, rows}, opName, ids )
  }
  apply = (
    func : ((table : TableT) => TableT),
    opName : string,
    hooks : typeof this.hooks = []
  ) => {
    const headers = this.table.headers.slice()
    const rows = this.table.rows.slice()
    return this.createNext( func({headers, rows}), opName, undefined, hooks)
  }
};
