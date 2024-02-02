import React from 'react'
import './loading.css'

export default class Loading extends React.Component{
  render(){
    return(
      <div className="loading-box">
        <div className="loading"></div>
        <p className="loading-text">Loading</p>
      </div>
    )
  }
}
