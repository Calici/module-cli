import React from "react";
import
  Pagination, { PaginationP }
from "../pagination/pagination-component";
import {
  GenericAutoAdjustRow,
  GenericAutoAdjustRowP,
  TableRowChildP
} from "./auto-adjust-row";
import "./table.css"

export type TableChildRenderP<T, U> = Omit<
  GenericAutoAdjustRowP<T, U>, "render" | "props"
> & U

export type TableP<T, U, V, W> = {
  rows : Array<Array<T>>
  headers : Array<V>

  rowRender :
    React.ComponentType<TableChildRenderP<T, U>> |
    React.LazyExoticComponent<
      React.ComponentType<TableChildRenderP<T, U>>
    >
  rowProps : U

  headerRender :
    React.ComponentType<TableRowChildP<V, W>> |
    React.LazyExoticComponent<
      React.ComponentType<TableRowChildP<V, W>>
    >
  headerProps : W

  className? : string
  paginate? : Omit<PaginationP<Array<T>, U>, "data" | "render" | "props">
  emptyRender? : React.ReactNode
}

const  NewTable =
<T, U, V, W>(props : TableP<T, U, V, W>) => {
  const [state, setState] = React.useState({isMobile : false})
  const setMobile = React.useCallback(() => {
    setState({isMobile : window.innerWidth < 1280})
  }, [ setState ])
  React.useEffect(() => {
    setMobile()
    window.addEventListener('resize', setMobile, false)
  }, [])
  const {
    rows, headers, className = '', paginate = {},rowRender, emptyRender = <></>,
    rowProps, headerRender, headerProps
  } = props
  if (rows.length === 0 && emptyRender)
    return <>{emptyRender}</>
  return (
    <div className = {`component-table ${className}`}>
      <GenericAutoAdjustRow
        data = {headers}
        render = {headerRender}
        props = {headerProps}
        id = {0}
        className = {className + ' table-header'}
        mobile = {state.isMobile}
      />
      <Pagination
        data = {rows}
        render = {rowRender}
        props = {{
          mobile : state.isMobile,
          className : className,
          ...rowProps
        }}
        {...paginate}
      />
    </div>
  )
}

export default NewTable
