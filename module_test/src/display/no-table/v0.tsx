import React from "react";
import { NoTableDisplayI as NoTableDisplayI_v0 } from "interfaces/noTableDisplay/v0";
import { NoTableDisplayI as NoTableDisplayI_v1 } from "interfaces/noTableDisplay/v1";
import { SocketControl } from "interfaces/component";
import NoTableDisplay from "./v1";
// CSS
import "./main.css";


interface NoTableDisplayProps{
  component : NoTableDisplayI_v0
  controls : SocketControl
}

 export default class NoTableDisplay_v0_Adapter extends React.Component<NoTableDisplayProps> {
  static PROGRESS_BG_COLOR = "#8DA6FF";
  static PROGRESS_TICK = 50;

  render() {
    const { component, controls } = this.props;
    const { progress, messages, start_time, status } = component;
    const v1_component : NoTableDisplayI_v1 = {
      version : "1.0",
      progress : {
        bouncy:true,
        value:progress
      },
      messages : messages,
      time : {
        startTime: start_time,
        timeDelta : 0
      },
      status : status
    }
    return (
      <NoTableDisplay
        controls = {controls}
        component = {v1_component}
      />
    )

  }
}

