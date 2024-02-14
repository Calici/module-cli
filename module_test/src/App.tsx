import CaliciSocket from "./socket"
import "./App.css"

const App = () => {
  const moduleId = "1"
  const jwt = "1"
  const url = `${process.env.REACT_APP_WS_ENDPOINT}`
  if (!moduleId) return <></>
  return (
    <div className = 'module-run box-simulation'>
      <div className = 'module-socket'>
        <CaliciSocket
          moduleId = {parseInt(moduleId)} jwt = {jwt} url = {url}
        />
      </div>
    </div>
  )
}

export default App
