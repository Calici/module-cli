import React from "react"
import Lister from "../lister/lister"

export type TableCellChildP <T, U> = {
  data : T, rowId : number, colId : number
} & U

export type AutoAdjustCellP<T, U> = {
  data : T,
  id : number,
  className : string,
  length : number,
  render : React.ComponentType<TableCellChildP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<TableCellChildP<T, U>>>,
  props : U,
  rowId : number
}
export const AutoAdjustCell = <T, U>(props : AutoAdjustCellP<T, U>) => {
  const style = {
    minWidth : `calc(100% / ${props.length})`,
    width : `calc(100% / ${props.length - 1})`
  }
  const Component = props.render
  const newProps = {
    data : props.data,
    rowId : props.rowId,
    colId : props.id,
    ...props.props
  } as (
    TableCellChildP<T, U> &
    JSX.IntrinsicAttributes &
    React.PropsWithRef<TableCellChildP<T, U>>
  )
  return (
    <div
      className = {`${props.className} component-table-cell`}
      style = {style}
    >
      <Component {...newProps} />
    </div>
  )
}

export type TableRowChildP<T, U> = {
  data : T, id : number, length : number, className : string, rowId : number
} & U

export type GenericAutoAdjustRowP<T, U> = {
  mobile : boolean
  data : Array<T>,
  id : number,
  className : string,
  render : React.ComponentType<TableRowChildP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<TableRowChildP<T, U>>>
  props : U
}

export const GenericAutoAdjustRow =
<T, U>(props : GenericAutoAdjustRowP<T, U>) => {
  const {mobile, data, className, id} = props
  const mobileClass = mobile ? 'mobile' : 'desktop'
  const combine = `${className} ${mobileClass}`
  const totalLength = data.length
  return (
    <div className = {`component-table-row ${combine}`}>
      <Lister
        array = {data}
        render = {props.render}
        props = {{
          className : mobileClass,
          length : totalLength,
          rowId : id,
          ...props.props
        }}
      />
    </div>
  )
}

export type AutoAdjustRowP<T, U> = {
  mobile : boolean
  data : Array<T>,
  id : number,
  className : string,
  render : React.ComponentType<TableCellChildP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<TableCellChildP<T, U>>>,
  props : U
}

export const AutoAdjustRow = <T, U>(props : AutoAdjustRowP<T, U>) => {
  return (
    <GenericAutoAdjustRow
      mobile = {props.mobile}
      data = {props.data}
      id = {props.id}
      className = {props.className}
      render = {AutoAdjustCell}
      props = {{
        render : props.render,
        props : props.props
      }}
    />
  )
}

export default AutoAdjustRow
