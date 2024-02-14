import Display from "./interfaces/display"
import React from "react"
import MessageBox from './components/messagebox/messagebox'
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSpinner, faUsers, faExclamationTriangle } from "@fortawesome/free-solid-svg-icons"
import DisplayRenderer from "./display";
import JSONSocket from "./components/json_socket/json_socket";
import "./socket.css"

interface CaliciSocketProps{
  moduleId : number,
  jwt : string,
  url?: string
}
interface CaliciSocketState_Init{
  wsConnectCount : number,
  wsDisplay : Display,
  wsData : {type : string, msg : string}
  wsInit : true
  wsQueue: number
  socket : JSONSocket
  wsState : "auth" | "queue" | "run" | "err"
}
interface CaliciSocketState_UnInit{
  wsInit : false
}
type CaliciSocketState = CaliciSocketState_Init | CaliciSocketState_UnInit

export default class CaliciSocket extends React.Component<
  CaliciSocketProps, CaliciSocketState
>{
  static SOCKET_BACKEND_URL  = `${process.env.REACT_APP_WS_ENDPOINT}`
  constructor(props : CaliciSocketProps){
    super(props)
    this.state  = {
      wsInit : false
    }
  }
  componentDidMount(): void {
    const newState : CaliciSocketState_Init = {
      wsInit : true,
      wsData : {type : "", msg : ""},
      socket : this.createSocket(),
      wsDisplay : new Display(),
      wsConnectCount : -1,
      wsState : "auth",
      wsQueue : 0
    }
    this.setState(newState)
  }
  componentWillUnmount(): void {
    if (this.state.wsInit)
      this.state.socket.close()
  }
  displayMessageHandler = (msg : any) => {
    if (!this.state.wsInit)
      throw Error("Something very wrong happened")
    const data = msg.msg
    const disp = this.state.wsDisplay
    if (disp.isInitialized())
      disp.set(data.component, data.dtype)
    else
      disp.init(data.component, data.dtype)
    this.setState({wsInit : true, wsDisplay : disp})
  }
  otherMessageHandler = (msg : any) => {
    if (msg.type === "err")
      this.setState({wsState : "err", wsInit : true, wsData : msg})
    else if (msg.type === "ws_queue")
      this.setState({
        wsState : "queue", wsInit : true, wsData : msg, wsQueue : msg.queue_num}
      )
    else if (msg.type === "auth")
      this.setState({
        wsState : "auth", wsInit : true, wsData : msg
      })
  }
  messageEventHandler = (msg : any) => {
    if (msg.type === 'display')
      this.displayMessageHandler(msg)
    else
      this.otherMessageHandler(msg)
  }
  createSocket = () : JSONSocket => {
    const onError = (ev : Event) => console.error(ev)
    const onOpen = (socket : JSONSocket) => {
      const payload = {
        module_id : this.props.moduleId, jwt : this.props.jwt
      }
      socket.send(payload)
    }
    const onMsg = this.messageEventHandler
    const onClose = () => {}
    const endpoint = this.props.url ?
      this.props.url : CaliciSocket.SOCKET_BACKEND_URL
    const socket = new JSONSocket({
      onOpen, onClose, onError, onMsg, endpoint
    })
    return socket
  }
  msgBoxIcon = () : React.ReactNode => {
    if (!this.state.wsInit) throw Error("ws not started yet")
    const state = this.state.wsState
    if (state === 'err')
      return (
        <p className='socket-load-msg-title warning'>
          <FontAwesomeIcon icon={faExclamationTriangle}/>
        </p>
      )
    else
      return (
        <p className='socket-load-msg-title spin'>
          <FontAwesomeIcon icon={faSpinner}/>
        </p>
      )
  }
  queueMessage = () : React.ReactNode => {
    if (!this.state.wsInit) throw Error("WS not started yet")
    return (
      <div className = 'queue-box'>
        <p className = 'queue-title'> Queue </p>
        <p className = 'queue-icon'>
          <FontAwesomeIcon icon = {faUsers} />
          {" " + this.state.wsQueue}
        </p>
      </div>
    )
  }

  dedicatedServerMessage = () : React.ReactNode => {
    return(
      <div className='socket-load-msg-server'>
        <p>If you want to execute the module without waiting, <br/> you can require the dedicated server.</p>
      </div>
    )
  }

  renderMsgBoxButtons = () => {
    if (!this.state.wsInit) throw Error("WS not started yet")
    const state = this.state.wsState
    if (state === 'err')
      return (
        <p className='socket-load-msg-desc2'>
          *You can go back by click Close button.
        </p>
      )
    else
      return (
        <p className='socket-load-msg-desc2'>
          *You can go back by click Close button. <br/>
          Your process will be run in background.
        </p>
      )
  }

  render(): React.ReactNode {
    if (!this.state.wsInit)
      return <></>
    const isDisplayInitialized = this.state.wsDisplay.isInitialized()
    const wsState = this.state.wsState
    const msg = this.state.wsData.msg
    return (
      <div className = 'socket-display'>
        <MessageBox
          className = 'socket-load-msgbox' state = {!isDisplayInitialized}
        >
          <div className = 'socket-load-msg'>
            {this.msgBoxIcon()}
            <p className = 'socket-load-msg-desc'>{msg}</p>
            {wsState === 'queue' && this.state.wsQueue > 0 &&
              <>
                {this.queueMessage()}
                {this.dedicatedServerMessage()}
              </>
            }
            {this.renderMsgBoxButtons()}
          </div>
        </MessageBox>
        {
          isDisplayInitialized &&
          <DisplayRenderer
            display = {this.state.wsDisplay}
            socket = {this.state.socket}
          />
        }
      </div>
    )
  }
}
