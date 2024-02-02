import React from 'react';
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faAngleLeft,
  faAngleRight,
  faAngleDoubleLeft,
  faAngleDoubleRight
} from "@fortawesome/free-solid-svg-icons"
import NewPagination, { PaginationP } from './pagination-component';
import './paginationlist.css'

const LEFT_PAGE = "LEFT";
const RIGHT_PAGE = "RIGHT";
const LEFT = "BEGINNING";
const RIGHT = "LAST";

const range = (from: number, to: number, step = 1) => {
  const range = [];
  for (let i = from; i <= to; i+=step) {
    range.push(i);
  }
  return range;
};

interface PaginationPanelProps{
  totalRecords    : number,
  pageLimit?      : number,
  pageNeighbours? : number,
  currentPage     : number,
  className?      : string,
  goToPage        : string,
  totalPages      : number,
  onPageChanged   : (page: number) => any
}

class PaginationPanel extends React.Component<PaginationPanelProps> {
  componentDidUpdate(prevProps: PaginationPanelProps) {
    if (this.props.goToPage !== prevProps.goToPage) {
      this.gotoPage(parseInt(this.props.goToPage));
    }
  }
  gotoPage = (page: number) => {
    const { onPageChanged = (f: number) => f } = this.props;
    const currentPage = Math.max(1, Math.min(page, this.props.totalPages));
    const paginationData = {
      currentPage,
      totalPages: this.props.totalPages,
      pageLimit: this.props.pageLimit,
      totalRecords: this.props.totalRecords
    };
    onPageChanged(paginationData.currentPage);
  };
  handleClick = (page: number, evt: React.MouseEvent<HTMLButtonElement>) => {
    evt.preventDefault();
    this.gotoPage(page)
  };
  handleMoveLeft = (evt: React.MouseEvent<HTMLButtonElement>) => {
    evt.preventDefault();
    this.gotoPage(this.props.currentPage - 1);
  };
  handleMoveRight = (evt: React.MouseEvent<HTMLButtonElement>) => {
    evt.preventDefault();
    this.gotoPage(this.props.currentPage + 1);
  };
  handleFirst = (evt: React.MouseEvent<HTMLButtonElement>) => {
    evt.preventDefault();
    this.gotoPage(1);
  }
  handleLast = (evt: React.MouseEvent<HTMLButtonElement>) => {
    evt.preventDefault();
    this.gotoPage(this.props.totalPages);
  }
  fetchPageNumbers = () => {
    let {pageNeighbours = 2 } = this.props;
    const totalPages = this.props.totalPages;
    const currentPage = this.props.currentPage;
    pageNeighbours = Math.max(0, Math.min(pageNeighbours, 2))
    const totalNumbers = pageNeighbours * 2 + 3;
    const totalBlocks = totalNumbers + 2;
    if (totalPages > totalBlocks) {
      let pages = [];
      const leftBound = currentPage - pageNeighbours;
      const rightBound = currentPage + pageNeighbours;
      const beforeLastPage = totalPages;
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
    }
    return range(1, totalPages);
  };
  render() {
    if (!this.props.totalRecords) return null;
    if (this.props.totalPages === 1) return null;
    const { currentPage } = this.props;
    const className = this.props.className ? this.props.className : ''
    const pages = this.fetchPageNumbers()
    return (
      <div className={`${className} pagination-wrapper`}>
        <nav className={`${className} pagination-nav`}>
          <ul className={`${className} pagination-list`}>
            {pages.map((page, index) => {
              if (page === LEFT)
                return (
                  <li key={index} className={`${className} pagination-item`}>
                    <button
                      className={`${className} page-left-button pagination-item-button`}
                      onClick={this.handleFirst}
                    >
                      <span className={`${className} pagination-item-prev`}>
                        <FontAwesomeIcon icon={faAngleDoubleLeft}/>
                        </span>
                    </button>
                  </li>
                );
              if (page === LEFT_PAGE)
                return (
                  <li key={index} className={`${className} pagination-item`}>
                    <button
                      className={
                        `${className} page-left-button pagination-item-button`
                      }
                      onClick={this.handleMoveLeft}
                    >
                      <span className={`${className} pagination-item-prev`}>
                        <FontAwesomeIcon icon={faAngleLeft}/>
                        </span>
                    </button>
                  </li>
                );
              if (page === RIGHT)
                return (
                  <li key={index} className={`${className} pagination-item`}>
                    <button
                      className={`${className} pagination-item-button`}
                      onClick={this.handleLast}
                    >
                      <span className={`${className} pagination-item-next`}>
                        <FontAwesomeIcon icon={faAngleDoubleRight}/>
                        </span>
                    </button>
                  </li>
                );
              if (page === RIGHT_PAGE)
                return (
                  <li key={index} className={`${className} pagination-item`}>
                    <button
                      className={`${className} pagination-item-button`}
                      onClick={this.handleMoveRight}
                    >
                      <span className={`${className} pagination-item-next`}>
                        <FontAwesomeIcon icon={faAngleRight}/>
                      </span>
                    </button>
                  </li>
                );
              return (
                <li
                  key={index}
                  className={`${className} pagination-item${
                    currentPage === page ? " active" : ""
                  }`}
                >
                  <button
                    className={`${className} pagination-item-button`}
                    onClick={
                      (e) => this.handleClick(
                        typeof page === 'string' ? parseInt(page, 10) : page, e
                      )
                    }
                  >
                    {page}
                  </button>
                </li>
              );
            })}
          </ul>
        </nav>
      </div>
    );
  }
}

interface PaginationProps<T>{
  data            : Array<T>,
  pageSize?       : number,
  className?      : string,
  render          : React.ReactElement<{dataItem?: T, dataId?: number}>,
  currentPage?    : (page: number) => number,
  page?           : number,
  showPanel?      : boolean,
  pageNeighbours  : number,
  goToPage        : string
}

interface PaginationState{
  currentPage: number
}

class LegacyPagination extends React.Component<PaginationProps<any>, PaginationState> {
  constructor(props: PaginationProps<any>) {
    super(props)
    this.state = {
      currentPage: 1,
    }
  }
  renderTable(page: number) {
    let dataList = this.props.data
    let pageSize = this.props.pageSize ? this.props.pageSize : 10
    const list = this.paginate(dataList, page, pageSize)
    const className = this.props.className ? this.props.className : ''
    return (
      <div className={`${className} pagination-render-item`}>
        {list.map((item, id) =>
          <React.Fragment key = {id}>
            {React.cloneElement(this.props.render, {
              dataItem: item,
              dataId: (page - 1) * (pageSize) + id,
              ...this.props.render.props
            })}
          </React.Fragment>
        )}
      </div>
    )
  }
  paginate = (items: Array<any>, pageNumber: number, pageSize = 10) => {
    let startIndex = (pageNumber - 1) * pageSize;
    let array = items.slice(startIndex, startIndex + pageSize)
    // fix bug delete last item of last page
    if (array.length === 0 && pageNumber>1) {
      pageNumber = pageNumber - 1
      startIndex = (pageNumber - 1) * pageSize;
      array = items.slice(startIndex, startIndex + pageSize)
    }
    return array;
  }
  onPageChanged = (pageNumber: number) => {
    this.setState({ currentPage: pageNumber });
    if (this.props.currentPage) this.props.currentPage(pageNumber)
  };
  render() {
    const page  = this.props.page ? this.props.page : this.state.currentPage
    const totalRecords = this.props.data ? this.props.data.length : 0
    const table   = this.renderTable(page)
    const className = this.props.className ? this.props.className : ''
    const showPanel = this.props.showPanel !== undefined ?
      this.props.showPanel : true
    const pageSize = this.props.pageSize ? this.props.pageSize : 10
    const totalPages = Math.ceil(totalRecords / pageSize)
    return (
      <div className={`${className} pagination-container`}>
        {table}
        {showPanel && <PaginationPanel
          goToPage={this.props.goToPage}
          totalRecords={totalRecords}
          pageLimit={this.props.pageSize}
          pageNeighbours={this.props.pageNeighbours}
          onPageChanged={this.onPageChanged}
          currentPage={page}
          className={className}
          totalPages={totalPages}
          // key={totalRecords}
        />}
      </div>
    );
  }
}

const Pagination =
<T, U>(props : PaginationProps<T> | PaginationP<T, U>) => {
  return "props" in props ?
    <NewPagination {...props} /> : <LegacyPagination {...props} />
}

export default Pagination
