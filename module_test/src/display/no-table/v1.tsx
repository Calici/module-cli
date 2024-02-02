import React from "react";
import { NoTableDisplayI} from "../../interfaces/noTableDisplay/v1";
import { SocketControl } from "../../interfaces/component";

import { WaveAnimation, FadeTS } from "./animation";
import ProgressBar from "../../components/progressbar/progressbar";
import SocketMessages from "../share/socket-message";

// Graphics Import
import MainCircle from "assets/simulation/main-circle.svg";
import MainCircleBorder from "assets/simulation/main-circle-border.svg";
import MainCircleDot from "assets/simulation/main-circle-dot.svg";
import MainCircleDash from "assets/simulation/main-circle-dash.svg";

// CSS
import "./main.css";
import RunTimer from "../share/run_timer";

interface NoTableDisplayProps {
  component: NoTableDisplayI;
  controls: SocketControl;
}

class NoTableDisplay extends React.Component<NoTableDisplayProps> {
  static PROGRESS_BG_COLOR = "#8DA6FF";
  static PROGRESS_TICK = 50;
  
  render() {
    const { component } = this.props;
    const { progress, messages, time, status } = component;
  
    // Null check for time and startTime
    const startTime = time && time.startTime ? time.startTime : 0;
  
    return (
      <div
        className={
          status === "RUNNING"
            ? "loading-wrapper"
            : "loading-done loading-wrapper"
        }
      >
        <div className="wave-animation">
          <WaveAnimation />
        </div>
        <FadeTS bottom cascade>
          <div className="websocket-loading-box">
            <div className="loading-item-progress">
              <div className="item-main-circle">
                <div>
                  <img src={MainCircleDot} alt="fancy-loading-circle-part-4" />
                </div>
                <div>
                  <img src={MainCircle} alt="fancy-loading-circle-part-5" />
                </div>
                <div>
                  <img
                    src={MainCircleBorder}
                    alt="fancy-loading-circle-part-6"
                  />
                </div>
                <div>
                  <img src={MainCircleDash} alt="fancy-loading-circle-part-7" />
                </div>
                <div className="item-circle-num">
                  <ProgressBar
                    completed={progress.value}
                    showNumber={true}
                    showBar={false}
                    bouncy={progress.bouncy}
                    tick={NoTableDisplay.PROGRESS_TICK}
                  />
                </div>
              </div>
              <ProgressBar
                completed={progress.value}
                showNumber={false}
                showBar={true}
                bouncy={progress.bouncy}
                tick={NoTableDisplay.PROGRESS_TICK}
              />
              <RunTimer startTime={time.startTime} status={status} />
            </div>
            <div className="loading-item-msg">
              <p className="websocket-title">Messages from the Pharmaco-Net</p>
              <SocketMessages
                className="websocket-desc"
                mainLineCount={1}
                messages={messages}
                expandable={true}
              />
            </div>
          </div>
        </FadeTS>
      </div>
    );
  }
}

export default NoTableDisplay;
