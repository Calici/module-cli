import React from 'react'
import ProcessAPI from "./API/process"
import ProjectAPI from "./API/project"
import {faFolder, faFileAlt, faFlask} from "@fortawesome/free-solid-svg-icons"
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import './params-container.css'
import { NavigateParamsContext } from './routes/utils';

export default class ParamsContainer extends React.Component{
  static contextType = NavigateParamsContext;
  constructor(props){
    super(props)
    this.state  = {
      project   : "",
      process   : ""
    }
  }
  componentDidMount(){
    const {moduleId, processId}  = this.props
    if(moduleId) {
      this.queryModule(moduleId)
    } else if(processId) {
      this.queryProcess(processId)
    }
  }
  componentDidUpdate(prevProps){
    if(this.props.moduleId !== prevProps.moduleId)
      this.queryModule(this.props.moduleId)
  }
  queryProcess(processId){
    ProjectAPI.all({processes : processId}).then((project)=>{
      this.setState({
        project : project[0]
      })
    }).catch((err)=>console.error(err))
    ProcessAPI.all({id : processId}).then((process)=>{
      this.setState({
        process : process[0]
      })
    }).catch((err)=>console.error(err))
  }
  queryModule(moduleId){
    ProjectAPI.all({processes__modules: moduleId}).then((project)=>{
      this.setState({
        project : project[0]
      })
    }).catch((err)=>console.error(err))
    ProcessAPI.all({modules : moduleId}).then((process)=>{
      this.setState({
        process : process[0]
      })
    }).catch((err)=>console.error(err))
  }
  handleNavigateClick = (to) =>{
    if(to === 'project'){
      this.context.navigate(`/dashboard/project/${this.state.project.data.id}`)
    }
    else if(to === 'process'){
      this.context.navigate(`/dashboard/process/${this.state.process.data.id}/node`)
    }
    else if(to === 'file'){
      this.context.navigate(`/dashboard/process/${this.state.process.data.id}/upload`)
    }
  }
  removeExcess = (fileName) =>{
    if(fileName){
      return fileName.substring(21)
    }
  }
  render(){
    const projectName   = this.state.project?.data?.name
    const processName   = this.state.process?.data?.name
    const fileName      = this.state.process?.data?.file
    const className     = this.props.className ? this.props.className : ''
    return (
      <div className= {`project-information ${className}`}>
        <div
          className='project-information-btn'
          onClick={()=>this.handleNavigateClick('project')}
        >
          <FontAwesomeIcon icon={faFolder}/>
          <p>{projectName}</p>
        </div>
        <div
          className='project-information-btn'
          onClick={()=>this.handleNavigateClick('process')}
        >
          <FontAwesomeIcon icon={faFileAlt}/>
          <p>{processName}</p>
        </div>
      </div>
    )
  }
}
