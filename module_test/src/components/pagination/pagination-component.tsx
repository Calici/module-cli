  import React from 'react';
import Lister from '../lister/lister';
import PaginationPanel from './pagination-panel';
import './paginationlist.css'



const PAGINATION_PAGE_SIZE = 10

type PaginationChildRenderP<T, U> = {
  data : T, id : number
} & U

type PaginationChildP<T, U> = {
  render :
    React.ComponentType<PaginationChildRenderP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<PaginationChildRenderP<T, U>>>
  props : U
  idStart : number,
  data : T,
  id : number
}

const PaginationChild =
<T, U>(props : PaginationChildP<T, U>) => {
  const {idStart, id, data} = props
  const Component = props.render
  const renderProps = {
    data : data, id : idStart + id, ...props.props
  } as (
    JSX.IntrinsicAttributes &
    PaginationChildRenderP<T, U> &
    React.PropsWithRef<PaginationChildRenderP<T, U>>
  )
  return (
    <Component {...renderProps} />
  )
}


export type PaginationP<T, U> = {
  data : Array<T>
  render :
    React.ComponentType<PaginationChildRenderP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<PaginationChildRenderP<T, U>>>
  props : U
  pageNeighbours? : number
  onPageChange? : (page : number) => void
  page? : number
  showPanel? : boolean
  className? : string
  pageSize? : number
}

const NewPagination = <T, U>(props : PaginationP<T, U>) => {
  const [state, setState] = React.useState({ currentPage : 1})
  const numPages = React.useMemo(() => {
    const pageSize = props.pageSize ? props.pageSize : PAGINATION_PAGE_SIZE
    return Math.ceil(props.data.length / pageSize)
  }, [ props ])
  const getCurrentPage = React.useCallback(() => {
    if (props.page && props.page < 1)
      throw Error("Page number should never be less than zero")
    return props.page ? props.page : state.currentPage
  }, [state.currentPage, props.page])
  const setPage = React.useCallback((page : number) => {
    if (page < 1 || page > numPages)
      return
    setState({currentPage : page})
    if (props.onPageChange) props.onPageChange(page)
  }, [setState, props.onPageChange, numPages])
  const table = React.useMemo(() => {
    const pageSize = props.pageSize ? props.pageSize : PAGINATION_PAGE_SIZE
    const pageNum = getCurrentPage()
    const startId = (pageNum - 1) * pageSize
    const expectedEnd = pageNum * pageSize
    const endId = expectedEnd > props.data.length ?
      props.data.length : expectedEnd
    const paginatedData = props.data.slice(startId, endId)
    return (
      <Lister
        array = {paginatedData}
        render = {PaginationChild}
        props = {{
          idStart : startId,
          render : props.render,
          props : props.props
        }}
      />
    )
  }, [
    getCurrentPage, props
  ])
  const {className = '', showPanel = true} = props
  return (
    <div className = {`${className} pagination-container`}>
      <div className = {`${className} pagination-render-item`}>
        {table}
      </div>
      <div className = {`${className} pagination-wrapper`}>
        {showPanel  && numPages > 1 &&
          <PaginationPanel
            totalRecords = {props.data.length}
            currentPage = {getCurrentPage()}
            setPage = {setPage}
            pageNeighbours = {props.pageNeighbours}
            className = {props.className}
            totalPage = {numPages}
          />
        }
      </div>
    </div>
  )
}

export default NewPagination
