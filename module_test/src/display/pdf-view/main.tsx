import { SocketControl } from "interfaces/component"
import { PDFDisplayT } from "interfaces/pdfDisplay/v0"
import SocketMessages from "../share/socket-message";
import ProgressBar from "components/progressbar/progressbar";
import RunTimer from "../share/run_timer";
import Controls from "../share/controls";
import PDFViewer from "../share/pdf-viewer";
import "../one-table/main.css"

type PDFDisplayP = {
  component : PDFDisplayT,
  controls : SocketControl
}

const PROGRESS_TICK = 50

const PDFDisplay = ({ component, controls } : PDFDisplayP) => {
  return (
    <div className = 'run-container'>
      <div className = 'simul-status'>
        <SocketMessages
          messages={component.messages}
          mainLineCount={5}
          className="one-table-message result-table-message"
          expandable={false}
        />
        <div className = {
          'simul-progress ' +
          (component.progress.value === 100 ? 'done ' : '') +
          (component.status === 'STOP' ? 'pause' : "")
        }>
          <ProgressBar
            bouncy = {component.progress.bouncy}
            showBar = {true}
            showNumber = {true}
            completed = {component.progress.value}
            tick = {PROGRESS_TICK}
          />
          <RunTimer
            startTime = {component.time.startTime}
            status = {component.status}
          />
        </div>
        <Controls
          progress = {component.progress.value}
          controls = {controls}
          status = {component.status}
          config = {component.controls}
          className = 'one-table'
        />
      </div>
      <PDFViewer
        file = {component.pdf_file.url}
        version = {component.pdf_file.version}
        params = {component.pdf_file.params}
      />
    </div>
  )
}

export default PDFDisplay
