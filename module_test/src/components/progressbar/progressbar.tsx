import React from 'react';
import './progressbar.css'

interface ProgressBarProps{
  completed   : string | number,
  bouncy      : boolean,
  showBar     : boolean,
  showNumber  : boolean,
  tick?       : number,
  className?  : string
}

interface ProgressBarState{
  progressQueue : Array<number>,
  currentProg   : number
}

export default class ProgressBar extends React.Component<
  ProgressBarProps, ProgressBarState
>{
  static PROGRESS_TICK   = 100;
  funcStack     : Array<{funcId : NodeJS.Timer, targetProgress : number}>
  progressTick  : number
  constructor(props : ProgressBarProps){
    super(props)
    this.state        = {
      progressQueue : [],
      currentProg   : this.convertFromString(props.completed),
    }
    this.funcStack    = []
    this.progressTick = props.tick ? props.tick : ProgressBar.PROGRESS_TICK
  }
  convertFromString(progress : string | number) : number{
    if(typeof(progress) === 'string') return parseInt(progress)
    else return progress
  }
  componentDidUpdate(
    prevProps : Readonly<ProgressBarProps>,
    prevState : Readonly<ProgressBarState>
  ){
    if(this.props.bouncy) this.bouncyUpdate(prevProps, prevState)
  }
  bouncyUpdate(
    prevProps : Readonly<ProgressBarProps>,
    prevState : Readonly<ProgressBarState>
  ){
    if(this.props.completed !== prevProps.completed){
      const progress  = this.convertFromString(this.props.completed)
      const queue     = this.state.progressQueue
      queue.push(progress)
      this.setState({progressQueue : queue})
    }
    if(this.state.progressQueue.length !== 0){
      const targetProgress  = this.state.progressQueue.splice(0, 1)[0]
      const delta           = targetProgress - this.state.currentProg
      const gradientSign    = delta > 0 ? 1 : -1;
      const funcId          = setInterval(() => {
        const progress  = this.state.currentProg
        if(progress < targetProgress)
          this.setState({
            currentProg : progress + gradientSign
          })
      }, this.progressTick)
      this.funcStack.push({funcId, targetProgress})
    }
    this.funcStack  = this.funcStack.filter((val) => {
      if(this.state.currentProg >= val.targetProgress){
        clearInterval(val.funcId)
        return false
      }
      return true
    })
  }
  render(){
    const progress    = this.props.bouncy ?
      this.state.currentProg : this.props.completed
    const className   = this.props.className ? this.props.className : ''
    if(this.props.showBar){
      const fillerStyles= {width: `${progress}%`}
      return (
        <div className={`progressbar ${className}`}>
          <div className={`progressbar-per ${className}`} style={fillerStyles}>
          </div>
          {this.props.showNumber &&
            <p className={`progressbar-num ${className}`} >
              {progress}<span>%</span>
            </p>}
        </div>
      )
    }
    else if(this.props.showNumber){
      return(
        <div className = {`progressbar-num-only ${className}`}>
          <p>{progress}<span>%</span></p>
        </div>
      )
    }
    else{
      return (
        <React.Fragment></React.Fragment>
      )
    }
  }
}
