import React from 'react'
import './titletext.css'

interface TitleTextProps{
  titleTextFirst  : string,
  titleTextLast   : string,
  className      ?: string
}

export default class TitleText extends React.Component<TitleTextProps>{
  render(){
    const className = this.props.className ? this.props.className : ''
    return(
      <div className = {`title-text-conatiner ${className}`}>
        <p className = 'title-text-first'> {this.props.titleTextFirst} </p>
        <p className = 'title-text-last'> {this.props.titleTextLast} </p>
      </div>
    )
  }
}
