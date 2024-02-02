import React from "react"
import Lister from "../../components/lister/lister"
import PropTypes from 'prop-types'
import './socket-message.css'

class SocketMessage extends React.Component{
  render(){
    return (
      <p>{this.props.data}</p>
    )
  }
}

export default class SocketMessages extends React.Component{
  static propTypes = {
    messages      : PropTypes.arrayOf(PropTypes.string),
    mainLineCount : PropTypes.number,
    className     : PropTypes.string,
    expandable    : PropTypes.bool
  }
  constructor(props){
    super(props)
    this.state  = {
      showFullMessage : true,
      doAutoScroll    : true
    }
    this.scrollRef  = React.createRef()
  }
  showHide = () => {
    this.setState({
      showFullMessage : !this.state.showFullMessage
    })
  }
  autoScroll = () => {
    if (this.scrollRef.current && this.state.doAutoScroll){
      const scrollHeight  = this.scrollRef?.current?.scrollHeight
      this.scrollRef.current.scrollTop  = scrollHeight
    }
  }
  componentDidMount(){
    this.autoScroll()
  }
  componentDidUpdate(){
    this.autoScroll()
  }
  render(){
    const messages  = this.props.messages
    const className = this.props.className ? this.props.className : ''
    const hide      = {display : 'none'}
    const noHide    = {display : 'inherit'}
    const hideMain  = this.state.showFullMessage ? hide : noHide
    const hideFull  = this.state.showFullMessage ? noHide : hide
    const partCount = this.props.mainLineCount ? this.props.mainLineCount : 1;
    const partMsg   = messages.slice(-partCount)
    return(
      <div className = {`socket-message ${className}`}>
        {/* Conditionally render the button */}
        {this.props.expandable && <div
          className = 'show-more-socket-message'
        >
          <button onClick = {this.showHide}>
            {this.state.showFullMessage ? 'Hide' : 'Show'}
          </button>
        </div>}
        <div
          className = 'brief-socket-message'
          style     = {hideMain}
        >
          <Lister
            array     = {partMsg}
            render    = {<SocketMessage />}
          />
        </div>
        <div ref = {this.scrollRef} onScroll = {this.handleScroll}
          className = 'all-socket-message'
          style     = {hideFull}
        >
          <Lister
            array     = {messages}
            render    = {<SocketMessage />}
          />
        </div>
      </div>
    )
  }
}
