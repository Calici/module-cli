import React from "react";
import Lister from "../lister/lister";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faAngleLeft,
  faAngleRight,
  faAngleDoubleLeft,
  faAngleDoubleRight
} from "@fortawesome/free-solid-svg-icons"

const DEFAULT_PAGE_NEIGHBOURS = 2
const LEFT_PAGE = "LEFT";
const RIGHT_PAGE = "RIGHT";
const LEFT = "BEGINNING";
const RIGHT = "LAST";

const range = (from : number, to : number, step : number = 1) => {
  const size = Math.ceil((to - from) / step) + 1
  return Array(size)
    .fill(null)
    .map((_, id) => id * step + from)
}

type PaginationButtonP = {
  data : string | number
  id : number
  advanceLeft : () => void
  advanceRight : () => void
  advanceLast : () => void
  advanceFirst : () => void
  advanceToPage : (page : number) => void
  className : string
  currentPage : number
}
const PaginationButton = (props : PaginationButtonP) => {
  const className = props.className
  if (props.data === LEFT)
    return (
      <li className={`${className} pagination-item`}>
        <button
          className={`${className} page-left-button pagination-item-button`}
          onClick={props.advanceFirst}
        >
          <span className={`${className} pagination-item-prev`}>
            <FontAwesomeIcon icon={faAngleDoubleLeft}/>
            </span>
        </button>
      </li>
    )
  else if (props.data === LEFT_PAGE)
    return (
      <li className={`${className} pagination-item`}>
        <button
          className={
            `${className} page-left-button pagination-item-button`
          }
          onClick={props.advanceLeft}
        >
          <span className={`${className} pagination-item-prev`}>
            <FontAwesomeIcon icon={faAngleLeft}/>
            </span>
        </button>
      </li>
    )
  else if (props.data === RIGHT)
    return (
      <li className={`${className} pagination-item`}>
        <button
          className={`${className} pagination-item-button`}
          onClick={props.advanceLast}
        >
          <span className={`${className} pagination-item-next`}>
            <FontAwesomeIcon icon={faAngleDoubleRight}/>
            </span>
        </button>
      </li>
    )
  else if (props.data === RIGHT_PAGE)
    return (
      <li className={`${className} pagination-item`}>
        <button
          className={`${className} pagination-item-button`}
          onClick={props.advanceRight}
        >
          <span className={`${className} pagination-item-next`}>
            <FontAwesomeIcon icon={faAngleRight}/>
          </span>
        </button>
      </li>
    )
  else if (typeof props.data === 'number')
    return (
      <li
        className={`${className} pagination-item ${
          props.currentPage === props.data ? "active" : ""
        }`}
      >
        <button
          className = {`${className} pagination-item-button`}
          onClick = {() => props.advanceToPage(props.data as number)}
          // Type check in the if expression
        >
          {props.data}
        </button>
      </li>
    )
  else
    return <></>
}

type PaginationPanelP = {
  totalRecords : number
  totalPage : number
  currentPage : number
  setPage : (page : number) => void
  pageNeighbours? : number
  className? : string
}

const PaginationPanel = (props : PaginationPanelP) => {
  const onPageClick = React.useCallback(
    (page : number) => props.setPage(page), [props.setPage]
  )
  const onAdvanceLeftClick = React.useCallback(
    () => props.setPage(props.currentPage - 1),
    [props.setPage, props.currentPage]
  )
  const onAdvancedRightClick = React.useCallback(
    () => props.setPage(props.currentPage + 1),
    [props.setPage, props.currentPage]
  )
  const advanceToFirst = React.useCallback(
    () => props.setPage(1), [props.setPage]
  )
  const advanceToLast = React.useCallback(
    () => props.setPage(props.totalPage), [props.setPage, props.totalPage]
  )
  const pageNumbers = React.useMemo(
    () => {
      // THIS ALGORITHM NEED REFACTORING, IT IS UNCLEAR
      const pageNeighbours = Math.max(
        0, Math.min(
          props.pageNeighbours ? props.pageNeighbours : 2,
          DEFAULT_PAGE_NEIGHBOURS
        )
      )
      const totalPage = props.totalPage
      const currentPage = props.currentPage
      const totalNumbers = pageNeighbours * 2 + 3
      const totalBlocks = totalNumbers + 2
      if (totalPage < totalBlocks)
        return range(1, props.totalPage)
      let pages = [];
      const leftBound = currentPage - pageNeighbours;
      const rightBound = currentPage + pageNeighbours;
      const beforeLastPage = totalPage;
      const startPage = leftBound > 1 ? leftBound : 1;
      const endPage = rightBound < beforeLastPage ? rightBound : beforeLastPage;
      pages = range(startPage, endPage)
      const pagesCount = pages.length;
      const singleSpillOffset = totalNumbers - pagesCount - 2;
      const leftSpill = startPage > 2;
      const rightSpill = endPage < beforeLastPage;
      const leftSpillPage = LEFT_PAGE;
      const rightSpillPage = RIGHT_PAGE;
      if (leftSpill && !rightSpill) {
        const extraPages = range(startPage - singleSpillOffset, startPage - 1);
        pages = [...extraPages, ...pages];
      } else if (!leftSpill && rightSpill) {
        const extraPages = range(endPage + 1, endPage + singleSpillOffset);
        pages = [...pages, ...extraPages];
      } else if (leftSpill && rightSpill) {
        pages = [...pages];
      }
      return [LEFT, leftSpillPage, ...pages,rightSpillPage, RIGHT];

    }, [props.pageNeighbours, props.totalPage, props.currentPage]
  )
  if (props.totalRecords === 0) return null
  else if (props.totalPage === 1) return null
  const currentPage = props.currentPage
  const {className = ''} = props
  return (
    <nav className = {`${className} pagination-nav`}>
      <ul className = {`${className} pagination-list`}>
        <Lister
          array = {pageNumbers}
          render = {PaginationButton}
          props = {{
            advanceLeft : onAdvanceLeftClick,
            advanceRight : onAdvancedRightClick,
            advanceLast : advanceToLast,
            advanceFirst : advanceToFirst,
            currentPage : currentPage,
            className : className,
            advanceToPage : onPageClick
          }}
        />
      </ul>
    </nav>
  )
}

export default PaginationPanel
