import { UNIVERSAL_TOKEN } from "./API/base"
import { useParams } from "react-router-dom"
import ParamsContainer from "./params-container"
import CaliciSocket from "./socket"


const App = () => {
  console.log(process.env.REACT_APP_WS_ENDPOINT)
  const params = useParams()
  const moduleId = params.moduleId
  const useNew = params.useNew
  const jwt = UNIVERSAL_TOKEN.jwtToken
  const url = useNew !== undefined ?
    `${process.env.REACT_APP_WS_ENDPOINT}new_process/`: undefined
  if (!moduleId) return <></>
  return (
    <div className = 'module-run box-simulation'>
      <ParamsContainer moduleId = {parseInt(moduleId)} className = 'module-run' />
      <div className = 'module-socket'>
        <CaliciSocket
          moduleId = {parseInt(moduleId)} jwt = {jwt} url = {url}
        />
      </div>
    </div>
  )
}

export default App
