import React from "react";
import { OneTableDisplayI as OneTableDisplayI_v0 } from "interfaces/oneTableDisplay/v0";
import { SocketControl } from "interfaces/component";
import ProgressBar from "components/progressbar/progressbar";
import Controls from "../share/controls";
import SocketMessages from "../share/socket-message";
import SocketTable from "../share/V1Table/table";
import NoResult from 'assets/noresult/no-simulation-result.png';
import EmptyList from "components/empty-list/emptylist";
import RunTimer from "../share/run_timer";

import './main.css';
import { adaptV0TableToV1 } from "interfaces/adapters";

interface OneTableDisplayP{
  component : OneTableDisplayI_v0
  controls : SocketControl
}

export default class NoTableDisplay extends React.Component<OneTableDisplayP>{
  static PROGRESS_TICK = 50;
  static controls_Status=true;
  render(): React.ReactNode {
    const { component , controls} = this.props;
    const { progress, messages, status, start_time }= component
      return (
        <div className="run-container">
          <div className="simul-status">
            <SocketMessages
              messages={messages}
              mainLineCount={5}
              className="one-table-message result-table-message"
              expandable={false}
            />
            <div className={`
              simul-progress
              ${progress === 100 ? "done" : ""}
              ${status === "STOP" ? "pause" : ""}
              `}
            >
              <ProgressBar
                  bouncy={true}
                  showBar={true}
                  showNumber={true}
                  completed={progress}
                  tick={NoTableDisplay.PROGRESS_TICK}
              />
              <RunTimer
                  startTime={start_time}
                  status={status}
              />
            </div>
            <Controls
              progress={progress}
              controls={controls}
              status={status}
              config = {component.controls}
              className="one-table"
            />
          </div>
          <SocketTable
            tableConfig = {adaptV0TableToV1(this.props.component.table)}
            className   = 'page-dark'
            paginate    = {{
              pageSize        : 10,
              pageNeighbours  : 2
            }}
            // removed when refactoring since it is not defined amongst SocketTable interface parameters
            emptyRender = {
              <EmptyList
                text = "No Result satisfying the parameters yet"
                img  = {NoResult}
                // picture="no-result"
                className = 'no-result'
              />
            }
          />
        </div>
      )

  }
}
