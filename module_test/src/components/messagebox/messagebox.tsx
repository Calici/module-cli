import React from 'react'
import './messagebox.css'

interface MessageBoxProps extends React.PropsWithChildren{
  state           : Boolean,
  onBgClick?      : () => any | undefined
  className?      : string,
}

export default class MessageBox extends React.Component<MessageBoxProps, any>{
  renderMessageBox = () => {
    const {state, className, children} = this.props
    const fixClassName = className ? className : ''
    return(
      <div
        className = "message-box-background"
        style = {{display : state ? 'flex' : 'none'}}
        onClick = {this.props.onBgClick}
      >
        <div className = {"message-box " + fixClassName}>
          {children}
        </div>
      </div>
    )
  }
  render(){
    return this.renderMessageBox()
  }
}
