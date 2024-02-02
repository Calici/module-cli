import React from "react";
import { OneTableDisplayT } from "../../interfaces/oneTableDisplay/v1";
import { SocketControl } from "../../interfaces/component";
import ProgressBar from "../../components/progressbar/progressbar";
import Controls from "../share/controls";
import SocketMessages from "../share/socket-message";
import SocketTable from "../share/V1Table/table";
import NoResult from 'assets/noresult/no-simulation-result.png';
import EmptyList from "../../components/empty-list/emptylist";
import RunTimer from "../share/run_timer";
import './main.css';
interface OneTableDisplayP{
  component : OneTableDisplayT
  controls : SocketControl
}

export default class NoTableDisplay extends React.Component<OneTableDisplayP>{
  static PROGRESS_TICK = 50;
  static controls_Status=true;
  render(): React.ReactNode {

      const { component , controls} = this.props;

      const { messages, status, table, time}= component
      const start_time = time.startTime;
      const progress=component.progress.value;


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
            tableConfig = {{...table}}
            className   = 'page-dark'
            paginate    = {{
              pageSize        : 10,
              pageNeighbours  : 2,
            }}

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
