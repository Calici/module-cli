import React from "react";
import './run_timer.css'
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faClock } from "@fortawesome/free-solid-svg-icons"

interface RunTimerProps{
  startTime : string,
  status    : string
}
interface RunTimerState{
  currentDelta : number
}

export default class RunTimer extends React.Component<
  RunTimerProps, RunTimerState
>{
  static REFRESH_INTERVAL = 1000
  static DAY_TO_MS        = 86400000
  static HOUR_TO_MS       = 3600000
  static MINUTE_TO_MS     = 60000
  static SECOND_TO_MS     = 1000
  timerId   : NodeJS.Timer | null
  constructor(props : RunTimerProps){
    super(props)
    this.state  = {
      currentDelta  : Date.now() - Date.parse(props.startTime)
    }
    this.timerId = null
  }
  componentDidMount(): void {
    this.timerId = setInterval(this.setTimeDelta, RunTimer.REFRESH_INTERVAL)
  }
  componentDidUpdate(
    prevProps: Readonly<RunTimerProps>, prevState: Readonly<RunTimerState>
  ){
    if(prevProps.startTime !== this.props.startTime)
      this.setTimeDelta()
    if(!this.isRunning(this.props.status) && this.timerId){
      clearInterval(this.timerId)
      this.timerId = null
    }
    if(
      this.timerId === null &&  
      this.isRunning(this.props.status) &&
      this.timerId == null
    )
      this.timerId = setInterval(this.setTimeDelta, RunTimer.REFRESH_INTERVAL)
  }
  isRunning = (status : string) : boolean => {
    if (status === 'COMPLETE') return false
    else if (status === 'ERROR') return false
    else if (status === 'STOP') return false
    else return true
  }
  setTimeDelta = () => {
    const startTime   = Date.parse(this.props.startTime)
    const endTime     = Date.now()
    const timeDelta   = endTime - startTime
    this.setState({
      currentDelta : timeDelta
    })
  }
  reLU(value : number) : number{
    return value < 0 ? 0 : value
  }
  displayTime(value : number) : string{
    const stringVal  = value.toString()
    return stringVal.length < 2 ? '0' + stringVal : stringVal
  }
  renderTimePassed() : React.ReactNode{
    const delta = this.state.currentDelta
    const days  = this.reLU(
      Math.floor(delta / RunTimer.DAY_TO_MS)
    )
    const numH  = this.reLU(Math.floor(
      (delta - days * RunTimer.DAY_TO_MS) /
      RunTimer.HOUR_TO_MS
    ))
    const hours = this.displayTime(numH)
    const numM  = this.reLU(Math.floor(
      (delta - days * RunTimer.DAY_TO_MS - numH * RunTimer.HOUR_TO_MS) /
      RunTimer.MINUTE_TO_MS
    ))
    const minutes = this.displayTime(numM)
    const numS  = this.reLU(Math.floor(
      (delta - days * RunTimer.DAY_TO_MS -
        numH * RunTimer.HOUR_TO_MS -
        numM * RunTimer.MINUTE_TO_MS
      ) /
      RunTimer.SECOND_TO_MS
    ))
    const seconds = this.displayTime(numS)
    return (
      <div>
        <p>{hours}:{minutes}:{seconds}</p>
        <p className={numH < 24 ? 'hide' : ''}>&#40; {days} days &#41; </p>
      </div>
    )
  }
  componentWillUnmount(): void {
    if(this.timerId) clearInterval(this.timerId)
  }
  render() : React.ReactNode{
    return(
      <div className = 'socket-timer'>
        <p className = 'socket-timer-title'>
          <FontAwesomeIcon icon={faClock}/>Running Time
        </p>
        <div className = 'socket-timer-content'>
          {this.renderTimePassed()}
        </div>
      </div>
    )
  }
}
