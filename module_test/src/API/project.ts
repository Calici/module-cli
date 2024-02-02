import { AxiosResponse } from 'axios';
import {
  TokenEndpoint,
  AuthEndpoint,
  UNIVERSAL_TOKEN,
  APIObject
} from './base'

class ProjectEndpoint extends AuthEndpoint{
  _createHeader() : Object {
    return {Authorization : `Token ${this.token.jwtToken}`}
  }
  _createFallbackHeader(): Object {
    return this._createHeader()
  }
  fallbackRequest = (): Promise<AxiosResponse> => {
    let endpoint = new TokenEndpoint('get', 'project/jwt/', this.token)
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => {
        UNIVERSAL_TOKEN.set(resp.data.jwt)
        res(resp)
      })
      .catch((err) => {
        rej(err)
      })
    })
  }
}

interface ProjectObject{
  name        : string,
  description : string,
  encryption  : boolean,
  id          : number
}

abstract class ProjectBase extends APIObject{
  static join : (token : string) => Promise<ProjectAPI>
  static all : (params : any) => Promise<Array<ProjectAPI>>
}

export default class ProjectAPI extends ProjectBase{
  data   : ProjectObject
  constructor(project : ProjectObject){
    super()
    this.data   = project
  }
  get = () : Promise<AxiosResponse> => {
    let endpoint = new ProjectEndpoint('get', `project/${this.data.id}/`, UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => {
        this.data   = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }
  update = (payload : Object) : Promise<AxiosResponse> => {
    let endpoint = new ProjectEndpoint('post', `project/${this.data.id}/`, UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        this.data   = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }
  delete = () : Promise<AxiosResponse> => {
    let endpoint = new ProjectEndpoint('delete', `project/${this.data.id}/`, UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => {
        UNIVERSAL_TOKEN.jwtToken = resp.data.jwt
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }
  // statics
  static all = (params : Object) : Promise<Array<ProjectAPI>> => {
    let endpoint = new ProjectEndpoint('get', 'project/', UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req({}, {}, params)
      .then((resp) => {
        let projects = resp.data.map((val : ProjectObject) => new ProjectAPI(val))
        res(projects)
      })
      .catch((err) => rej(err))
    })
  }
  static join = (token : string) : Promise<ProjectAPI> => {
    let endpoint = new TokenEndpoint('get', 'project/join', UNIVERSAL_TOKEN)
    let params  = {token : token}
    return new Promise((res, rej) => {
      endpoint.req({}, {}, params)
      .then((resp) => {
        let project   = new ProjectAPI(resp.data.project)
        UNIVERSAL_TOKEN.set(resp.data.jwt)
        res(project)
      })
      .catch((err) => rej(err))
    })
  }
  static create = (data : Object) : Promise<ProjectAPI> => {
    let endpoint  = new TokenEndpoint('post', 'project/', UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req(data)
      .then((resp) => {
        let project   = new ProjectAPI(resp.data.project)
        UNIVERSAL_TOKEN.set(resp.data.jwt)
        res(project)
      })
      .catch((err) => rej(err))
    })
  }
}
