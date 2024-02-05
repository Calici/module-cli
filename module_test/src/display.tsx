import React from "react";
import Display from "./interfaces/display";
import MessageBox from "./components/messagebox/messagebox";
import TitleText from "./components/titletext/titletext";
import { Link } from "react-router-dom";
import { DisplayProps, SocketControl } from "./interfaces/component";
import { Socket } from "./components/json_socket/json_socket";

const OneTableDisplay_v0 = React.lazy(
  () => import("./display/one-table/v0")
)
const OneTableDisplay_v1 = React.lazy(
  () => import("./display/one-table/v1")
)
const NoTableDisplay_v0 = React.lazy(
  () => import("./display/no-table/v0")
)
const NoTableDisplay_v1 = React.lazy(
  () => import("./display/no-table/v1")
)
const PDFDisplay = React.lazy(() => import('./display/pdf-view/main'))

interface DisplayRendererP{
  display : Display
  socket : Socket
}

interface DisplayRendererS{

}

export class ErrorMessageBox extends React.Component{
  render(){
    return (
      <MessageBox
          state = {true} className = 'backend-err-msgbox'
        >
          <TitleText
            titleTextFirst  = 'Backend Error'
            titleTextLast = 'Contact Developer of Calici'
          />
          <Link to = '../'>Go back to Module Page</Link>
      </MessageBox>
    )
  }
}

type VersionT = string
interface VersionSwitchP{
  version : VersionT,
  versionMap : Record<
    VersionT, React.LazyExoticComponent<React.ComponentType<any>>>
  props : DisplayProps
}

export class VersionSwitch extends React.Component<VersionSwitchP>{
  render(): React.ReactNode {
    const version = this.props.version
    const Comp = this.props.versionMap[version]
    const props = this.props.props
    return (
      <Comp {...props} />
    )
  }
}

export default class DisplayRenderer extends React.Component<
  DisplayRendererP, DisplayRendererS
>{
  controls : SocketControl
  constructor(props : DisplayRendererP){
    super(props)
    this.state = {}
    this.controls = {
      start : this.startModule,
      stop : this.stopModule
    }
  }
  startModule = () => {
    this.props.socket.send({
      type : 'stat', msg : 'play'
    })
    window.location.reload()
  }
  stopModule = () => {
    this.props.socket.send({
      type : 'stat', msg : 'stop'
    })
  }
  render(): React.ReactNode {
    const dtype = this.props.display.dtype()
    const version = this.props.display.version()
    const controls = this.controls
    const displayProps : DisplayProps = {
      component : this.props.display.get(),
      controls : controls,
    }
    // Implement
    if (dtype === 0)
      return (
        <VersionSwitch
          version = {version}
          versionMap = {{
            "0.0" : NoTableDisplay_v0,
            "1.0" : NoTableDisplay_v1,
          }}
          props = {displayProps}
        />
      )
    // Implement
    else if (dtype === 1)
      return(
        <VersionSwitch
          version = {version}
          versionMap = {{
            "0.0" : OneTableDisplay_v0,
            "1.0" : OneTableDisplay_v1
          }}
          props = {displayProps}
        />
      )
    else if (dtype == 2)
      return (
        <VersionSwitch
          version = {version}
          versionMap = {{
            "0.0" : PDFDisplay
          }}
          props = {displayProps}
        />
      )
    else
      return <ErrorMessageBox />
  }
}
