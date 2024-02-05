import Lister from "../../components/lister/lister"
import React from "react"
import Fade from 'react-reveal/Fade'
import './webanimation.css'

class AniItem extends React.Component{
  render(){
    const className = this.props.className ? this.props.className : ''
    return(
      <div className = {className} />
    )
  }
}

export default class Animation extends React.Component{
  render(){
    const {size, className} = this.props
    const fixClassName  = className ? className : ''
    const array = Array(size).fill("")
    return (
      <Lister
        array     = {array}
        className = {fixClassName}
        render    = {<AniItem className = {fixClassName}/>}
      />
    )
  }
}


export class WaveAnimation extends React.Component{
  render(){
    return (
      <Animation
        size      = {50}
        className = 'wave-ani-item'
      />
    )
  }
}

export class GraphAnimation extends React.Component{
  render(){
    return(
      <Fade bottom>
        <div className = 'item-graph'>
          <Animation
            size      = {8}
            className = 'item-graph-img'
          />
        </div>
      </Fade>
    )
  }
}

interface FadeTSProps extends React.PropsWithChildren{
  bottom? : boolean, 
  cascade?: boolean
}
export class FadeTS extends React.Component<FadeTSProps>{
  render(){
    return(
      <Fade {...this.props} />
    )
  }
}