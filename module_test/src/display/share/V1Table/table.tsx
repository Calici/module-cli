import React from "react"
import {
  faCaretUp,
  faCaretDown,
  faSort
} from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import Table from "components/table/table";
import {
  TableCellT,
  TableTypesT,
  TableT,
  TableTypeT,
} from "interfaces/oneTableDisplay/v1";
import AutoAdjustRow, { AutoAdjustCell } from "components/table/auto-adjust-row";
import {
  TableCellChildP,
} from "components/table/auto-adjust-row";
import { PaginationP } from "components/pagination/pagination-component";
// CSS
import "./table.css"
import ModuleTable from "./table_manager";

const TextCell = React.lazy(() => import('./Text'))
const ButtonCell = React.lazy(() => import('./Button'))
const ImageCell = React.lazy(() => import('./Image'))
const InputCell = React.lazy(() => import('./input-cell'))

type TableCellP = TableCellChildP<
  TableCellT, {
    types : TableTypesT,
    onChange? : (rowId : number, colId : number, value : any) => void
  }
>
const TableCell = (props : TableCellP) => {
  const type = props.types[props.colId]
  const onChange = React.useCallback((value : any) => {
    if (props.onChange)
      props.onChange(props.rowId, props.colId, value)
  }, [ props ])
  if (type.type === 'string' || type.type === 'number')
    return (
      <TextCell data = {props.data as any} type = {type} />
    )
  else if (type.type === 'id')
    return (
      <TextCell data = {props.rowId + 1} type = {type} />
    )
  else if (type.type === 'button')
    return (
      <ButtonCell data = {props.data as any} type = {type} />
    )
  else if (type.type === 'img')
    return (
      <ImageCell data = {props.data as any} type = {type} />
    )
  else if (type.type === 'input')
    return (
      <InputCell
        data = {props.data as any} type = {type} onChange = {onChange}
      />
    )
  else
    return <p>Error Type : {type.type}</p>
}

type HeaderT = {
  displayName : React.ReactNode
  sortOnHeader : boolean,
  sortStatus : boolean,
  sortable : boolean
  setSortParams : () => void
}

type SocketTableHeaderP = {
  data : HeaderT
}
const SocketTableHeader = (props : SocketTableHeaderP) => {
  const displayName = React.useMemo(() => {
    const displayName = props.data.displayName
    if (typeof displayName === 'string' || typeof displayName === 'number')
      return <p>{displayName}</p>
    else
      return <>{displayName}</>
  }, [ props ])
  if (! props.data.sortable )
    return <>{displayName}</>
  else
    return (
      <div
        className='component-table-cell-sort'
        onClick = {props.data.setSortParams}
      >
        {displayName}
        <button className='sort-btn'>
          {!props.data.sortOnHeader ?
            <FontAwesomeIcon icon = {faSort} /> :
            !props.data.sortStatus ?
            <FontAwesomeIcon icon={faCaretUp}/>
            :
            <FontAwesomeIcon icon={faCaretDown}/>
          }
        </button>
      </div>
    )
}

type SocketTableWithRenderP = {
  tableConfig : ModuleTable | TableT,
  className?: string,
  paginate?: Omit<PaginationP<any, any>, "data" | "render" | "props">,
  emptyRender?: React.ReactNode
  // Additional
  cellProps? : Omit<
    TableCellP,
    keyof TableCellChildP<TableCellT, {types : TableTypeT}>
  >
}

export const SocketTableWithRender = (props : SocketTableWithRenderP) => {
  // TODO: Fix this Clutch, Use a boolean to do this.
  // Also update node js a lot of features are unsupproted
  const table = React.useMemo(() => {
    if (props.tableConfig instanceof ModuleTable)
      return props.tableConfig
    else
      return new ModuleTable(props.tableConfig)
  }, [ props.tableConfig ])
  const lastSortIndex = table.table.headers
    .slice()
    .reverse()
    .findIndex(
      (val) => {
        if (
          val.type.type === 'string' ||
          val.type.type === 'id' ||
          val.type.type === 'number'
        )
          return val.type.sortable
        else
          return false
      }
    )
  const [state, setState] = React.useState({
    sorted : true,
    sortColumn : lastSortIndex !== -1 ?
      table.table.headers.length - 1 - lastSortIndex : 0
  })
  const processedTable = React.useMemo(
    () => table.sort(state.sortColumn, state.sorted).autoFloatingPoint(),
    [ table, state ]
  )
  const setSortParams = React.useCallback((id : number) => {
    setState((state) => {
      return {
        sortColumn : id,
        sorted : id === state.sortColumn ? !state.sorted : false
      }
    })
  }, [ setState ])
  const headers = React.useMemo(() => {
    return processedTable.table.headers.map((item, id) => {
      const setSort = () => setSortParams(id)
      const type = item.type.type
      const sortable =
        (type === 'string' || type === 'number' || type === 'id') &&
        item.type.sortable
      return {
        displayName : item.displayName,
        sortStatus : state.sorted,
        sortOnHeader : state.sortColumn === id,
        setSortParams : setSort,
        sortable : sortable
      }
    })
  }, [ props, state, setSortParams, processedTable ])
  const types = React.useMemo(() => {
    return processedTable.table.headers.map((header) => header.type)
  }, [ processedTable ])
  const {
    className = '', paginate, emptyRender, cellProps = {}
  } = props
  return (
    <Table
      className = {className}
      headers = {headers}
      headerRender = {AutoAdjustCell}
      headerProps = {{
        render : SocketTableHeader,
        props : {},
        className : "table-header"
      }}
      rows = {processedTable.table.rows}
      rowRender = {AutoAdjustRow}
      rowProps = {{
        render : TableCell,
        props : {
          types,
          ...cellProps
        },
        className : 'table-content'
      }}
      paginate = {paginate}
      emptyRender = {emptyRender}
    />
  )
}
const SocketTable = SocketTableWithRender
export default SocketTable
