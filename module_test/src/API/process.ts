import { AxiosResponse } from 'axios'
import {
  TokenEndpoint,
  APIObject,
  UNIVERSAL_TOKEN,
} from './base'

abstract class ProcessBase extends APIObject{
  static all : Function
  static create : Function
}

export interface ProcessObject{
  id  : number,
  project: number
}

export default class ProcessAPI extends ProcessBase{
  data : ProcessObject
  constructor(process : ProcessObject){
    super()
    this.data   = process
  }
  get = () : Promise<AxiosResponse> => {
    let endpoint  = new TokenEndpoint('get', `process/${this.data.id}/`, UNIVERSAL_TOKEN)
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
    let endpoint  = new TokenEndpoint('post', `process/${this.data.id}/`, UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        this.data = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }
  delete = () : Promise<AxiosResponse> => {
    let endpoint  = new TokenEndpoint('delete', `process/${this.data.id}/`, UNIVERSAL_TOKEN)
    return endpoint.req()
  }

  static all = (params : Object) : Promise<Array<ProcessAPI>> => {
    let endpoint = new TokenEndpoint('get', 'process/', UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req({}, {}, params)
      .then((resp) => {
        let processes = resp.data.map(
          (val : any) => new ProcessAPI(val)
        )
        res(processes)
      })
      .catch((err) => rej(err))
    })
  }
  static create = (payload : Object) : Promise<ProcessAPI> => {
    let endpoint = new TokenEndpoint('post', 'process/', UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => res(new ProcessAPI(resp.data)))
      .catch((err) => rej(err))
    })
  }
}
