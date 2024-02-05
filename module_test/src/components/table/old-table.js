import React from "react"
import Lister from "../lister/lister"
import Pagination from "../pagination/paginationcomponent"
import MessageBox from "../messagebox/messagebox"

// CSS
import './table.css'
import NoProcess from '../../assets/noresult/no-simul.png'

// NB : If you wanna use different layout for mobile and desktop, make a new
// component. Otherwise, it is only going to use css for switching.
class TableCell extends React.Component{
  renderData(data){
    if(typeof(data) === 'string')
      return <p>{data}</p>
    else if(React.isValidElement(data))
      return data
  }

  render(){
    const {data, className, length} = this.props
    const style = {
      'minWidth': `calc(100% / ${length})`,
      'width': `calc(100% / ${length - 1})`
    }
    return (
      <div className={`${className} component-table-cell`} style={style}>
        {this.renderData(data)}
      </div>
    )
  }
}

interface ZoomRowProps {
  rowTypes: Array<TableTypes>,
  // data      : any,
  // id        : Number | number
}
interface TableTypes {
  type?: string,
  zoomable?: string,
  sortable?: boolean
}

export class ZoomableCell extends React.Component<ZoomRowProps, any>{
  constructor(props) {
    super(props)
    this.state = {
      zoomMsgBoxState: false
    }
  }

  renderData(type: String, data: any) {
    if (typeof (data) === 'string')
      return <p>{data}</p>
    else if (React.isValidElement(data))
      return data
  }

  toggleZoom = () => {
    this.setState({ zoomMsgBoxState: !this.state.zoomMsgBoxState })
  }

  renderRow(type: TableTypes, data : any){
    let zoomable = type?.zoomable

    if (zoomable===true) {
      zoomable = 'normal'
    } else if (zoomable===false) {
      zoomable = 'none'
    }


    if (zoomable==='normal') {
      const content   = this.renderData(type?.type, data)
      return (
        <React.Fragment>
          <MessageBox state={this.state.zoomMsgBoxState} className="message-box-normal-zoom">
            <div className='zoom-cell-content normal-zoom'>
              {content}
            </div>
            <div className='zoom-close-button'>
              <button onClick={this.toggleZoom} className='popup-btn-close'>Close</button>
            </div>
          </MessageBox>
          <div
            className='table-cell-content zoomable'
            onClick={this.toggleZoom}
          >
            {content}
          </div>
        </React.Fragment>
      )
    } else{
      const content   = this.renderData(type?.type, data)
      return (
        <div className = 'table-cell-content'>
          {content}
        </div>
      )
    }
  }

  render() {
    const { rowTypes, data, id, className } = this.props
    const currentRow = rowTypes[id]
    const style = {
      'minWidth': `calc(100% / ${rowTypes.length})`,
      'width': `calc(100% / ${rowTypes.length - 1})`
    }
    return (
      <div className={`${className} component-table-cell`} style={style}>
        {this.renderRow(currentRow, data)}
      </div>
    )
  }
}

export class StandardRow extends React.Component{
  render(){
    const {mobile, dataItem, className, render} = this.props
    const mobileClass   = mobile ? 'mobile' : 'desktop'
    const combine       = `${className} ${mobileClass}`
    const totalLength   = dataItem.length
    return (
      <div className = {`component-table-row ${combine} `}>
        <Lister
          array   = {dataItem}
          render  = {
            render ? React.cloneElement(render, {
              ...render.props, className : mobileClass, length : totalLength
            }) : <TableCell
              className = {mobileClass} length = {totalLength}
            />
          }
        />
      </div>
    )
  }
}
export class AutoInsertIdRow extends React.Component{
  render(){
    const {dataItem, dataId, render, ...props}  = this.props
    const data  = [dataId + 1, ...dataItem]
    return <StandardRow
      render = {render} dataItem = {data} {...props}
    />
  }
}

class TableHeader extends React.Component{
  render(){
    const {className, dataItem, mobile}= this.props
    return (
      <StandardRow
        className = {className} dataItem = {dataItem} mobile = {mobile}
      />
    )
  }
}

export interface TableProps{
  rows        : Array<Array<any>>,
  headers     : Array<React.ReactNode>,
  className?  : String,
  paginate?   : Object,
  render      : React.ReactNode,
  emptyRender?: React.ReactNode
}

export default class OldTable extends React.Component<TableProps, any>{
  constructor(props){
    super(props)
    this.state  = {
      isMobile  : false
    }
  }
  componentDidMount(){
    this.setIsMobile()
    window.addEventListener('resize', this.setIsMobile, false)
  }

  setIsMobile = () => {
    this.setState({isMobile : window.innerWidth < 1280})
  }

  render(){
    const {
      rows, headers, className, paginate, render, emptyRender
    } = this.props
    const fixClassName = className ? className : ''
    if(rows.length === 0 && emptyRender)
      return React.cloneElement(emptyRender, emptyRender.props)
    else return(
      <div className = {`component-table ${fixClassName}`}>
        <TableHeader
          className = {`table-header ${fixClassName}`}
          dataItem  = {headers}
          mobile    = {this.state.isMobile}
        />
        <Pagination
          data        = {rows}
          render      = {
            React.cloneElement(render, {
              ...render.props,
              mobile : this.state.isMobile,
              className : `table-content ${fixClassName}`
            })
          }
          {...paginate}
        />
      </div>
    )
  }
}

export function renderNoItemTable(headers = [], text = "No data.", image = NoProcess) {
  return <></>
}

export const columnNormalType = { "sortable": false, "type": "string", "zoomable": "none" }

export const columnZoomType = { "sortable": false, "type": "string", "zoomable": "normal" }
