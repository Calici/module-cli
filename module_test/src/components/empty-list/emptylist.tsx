import React from 'react'
import './empty-list.css'

interface EmptyListProps{
  text      : React.ReactNode,
  className?: string,
  img       : string,
}
export default class EmptyList extends React.Component<EmptyListProps, any>{
  render(){
    const className =  this.props.className ? this.props.className : ''
    const img       =  this.props.img
    const text      =  this.props.text
    return(
      <div className={`${className}-container empty-list-container`}>
        <div className={`${className} empty-list-desc`} >
          <img src = {img} alt = ""></img>
          <p>{text}</p>
        </div>
      </div>
    )
  }
}
