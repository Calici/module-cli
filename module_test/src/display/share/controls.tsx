import MessageBox from "../../components/messagebox/messagebox"
import { SocketControl } from "../../interfaces/component"
import React from 'react'
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome"
import {
  faPlay,
  faPause
} from "@fortawesome/free-solid-svg-icons"
import { StatusT } from "../../interfaces/displayCommon"
import "./controls.css"

type ControlP<U, V> = {
  progress : number,
  controls : SocketControl
  status : StatusT
  config : {
    show_run : boolean, show_stop : boolean
  }
  stopRender : React.ComponentType<ControlButtonP<U>>,
  stopProps : U,
  startRender : React.ComponentType<ControlButtonP<V>>,
  startProps : V,
  className? : string
}

type ControlButtonP<U> = {
  disabled : boolean
  onClick : () => void
} & U

export const ControlsHOC = <V, U>({
  progress,
  controls,
  status,
  config,
  startRender, startProps,
  stopRender, stopProps,
  className = ''
} : ControlP<V, U>) => {
  const [showConfirm, setShowConfirm] = React.useState(false)
  const [confirmMsg, setConfirmMsg] = React.useState<React.ReactNode>("")
  const [confirmFunc, setConfirmFunc] = React.useState<() => void>(() => {})
  const askConfirm = React.useCallback(
    (text : React.ReactNode, func : () => void) => {
      return () => {
        setConfirmFunc(() => func)
        setConfirmMsg(text)
        setShowConfirm(true)
      }
    }, [setShowConfirm, setConfirmMsg, setConfirmFunc]
  )
  const startButton = React.useMemo(() => {
    if (!config.show_run) return <></>
    const Component = startRender
    const props = {
      onClick : askConfirm(
        <>
          <p>Are you sure you want to start the process ?</p>
          <p>You will need to wait before running.</p>
        </>,
        () => controls.start()
      ),
      disabled : status === 'RUNNING',
      ...startProps
    } as React.ComponentProps<typeof Component>
    return (
      <Component {...props} />
    )
  }, [
    startRender, status, config, startProps,
    controls, askConfirm, window
  ])
  const stopButton = React.useMemo(() => {
    if (!config.show_stop) return <></>
    const Component = stopRender
    const props = {
      onClick : askConfirm(
        "Are you sure you want to stop the process ? ",
        () => controls.stop()
      ),
      disabled : !(status === 'ERROR' || status === 'RUNNING'),
      ...stopProps
    } as React.ComponentProps<typeof Component>
    return (
      <Component {...props} />
    )
  }, [ stopRender, status, config, stopProps, controls ])
  const cancelConfirmation = React.useCallback(() => {
    setShowConfirm(false)
  }, [setShowConfirm])
  const confirmClick = React.useCallback(() => {
    if (confirmFunc) confirmFunc() // This somehow is undefined in some cases ?
    setShowConfirm(false)
  }, [confirmFunc, setShowConfirm])
  if (progress >= 100) return null
  return (
    <div className = {'socket-controls ' + className}>
      <div className = {'socket-start ' + className}>
        {startButton}
      </div>
      <div className = {'socket-stop ' + className}>
        {stopButton}
      </div>
      <MessageBox
        state = {showConfirm} className = 'control-msgbox'
      >
          <p className="popup-desc">{confirmMsg}</p>
          <div className = 'popup-btn'>
            <button
              className = 'popup-btn-no' onClick = {cancelConfirmation}
            >No</button>
            <button
              className = 'popup-btn-yes' onClick = {confirmClick}
            >Yes</button>
          </div>
      </MessageBox>
    </div>
  )
}

type ButtonP = {
  onClick : () => void
  disabled : boolean,
  className? : string
}

const StartButton = ({ onClick, disabled, className = '' } : ButtonP) => {
  return (
    <button
      className = {className}
      onClick = {onClick}
      disabled = {disabled}
    >
      <FontAwesomeIcon icon = {faPlay} />
      Start
    </button>
  )
}

const StopButton = ({onClick, disabled, className = ''} : ButtonP) => {
  return (
    <button
      className = {className}
      onClick = {onClick}
      disabled = {disabled}
    >
      <FontAwesomeIcon icon = {faPause} />
      Pause
    </button>
  )
}

const Controls = ({
  progress, controls, config, className, status
} : Omit<
  ControlP<any, any>, 'startRender' | 'startProps' | 'stopRender' | 'stopProps'
>) => {
  return (
    <ControlsHOC
      progress = {progress}
      controls = {controls}
      status = {status}
      config = {config}
      startRender = {StartButton}
      startProps = {{
        className : "simul-btn simul-btn-run"
      }}
      stopRender = {StopButton}
      stopProps = {{
        className : "simul-btn simul-btn-stop"
      }}
      className = {className}
    />
  )
}


export default Controls
