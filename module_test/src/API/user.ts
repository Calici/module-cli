import { AxiosRequestHeaders, AxiosResponse } from 'axios'
import {
  Endpoint,
  TokenEndpoint,
  UNIVERSAL_TOKEN,
  APIObject
} from './base'
import { Buffer } from 'buffer'

export interface UserObject{
  email       : string,
  first_name  : string,
  last_name   : string,
  username    : string,
  company     : any,
  id          : number
}

abstract class UserBase extends APIObject{
  static login          : (payload : any) => Promise<UserAPI>
  static register       : (payload : any) => Promise<UserAPI>
  static loadFromLocal  : () => UserAPI
  static checkUser      : (params : Object) => Promise<AxiosResponse>
  static getLoginFields : () => Promise<AxiosResponse>
  static storageKey     : string
  static requestForgetPassword :
    (how : string, val : string) => Promise<AxiosResponse>
  static fetchSecurityQuestion:
    (headers : Partial<AxiosRequestHeaders>) => Promise<AxiosResponse>
  static answerSecurityQuestion:
    (
      answer : string, headers : Partial<AxiosRequestHeaders>
    ) => Promise<AxiosResponse>
  static changeForgetPassword: (
    new_password : string, headers : Partial<AxiosRequestHeaders>
  ) => Promise<AxiosResponse>

  abstract delete    : (payload : Object) => Promise<AxiosResponse>
}

export default class UserAPI extends UserBase{
  data        : UserObject
  // Internal Methods
  constructor(user : UserObject){
    super()
    this.data   = user
    this.saveToLocalStorage()
  }
  // This saves to local storage
  saveToLocalStorage = () => {
    let saveItem : Object = {user : this.data}
    let saveStr : string = Buffer.from(JSON.stringify(saveItem)).toString('base64')
    localStorage.setItem(UserAPI.storageKey, saveStr)
  }
  // This clears the local storage
  clearLocalStorage = () => {
    localStorage.setItem(UserAPI.storageKey, JSON.stringify({}))
  }
  // This logs the user out properly
  logout = () : Promise <AxiosResponse> => {
    let endpoint = new TokenEndpoint(
      'get', 'user/logout/', UNIVERSAL_TOKEN
    )
    this.clearLocalStorage()
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => {
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }

  // This checks login
  checkLogin = () : Promise<AxiosResponse> => {
    let endpoint = new TokenEndpoint(
      'get', 'user/check-login/', UNIVERSAL_TOKEN
    )
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }

  // This queries for profile
  get = () : Promise<AxiosResponse> => {
    let endpoint = new TokenEndpoint(
      'get', 'user/profile/', UNIVERSAL_TOKEN
    )
    return new Promise((res, rej) => {
      endpoint.req()
      .then((resp) => {
        this.data   = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }

  // This update the profile
  update = (payload : Object) : Promise<AxiosResponse> => {
    let endpoint  = new TokenEndpoint(
      'post', 'user/profile/', UNIVERSAL_TOKEN
    )
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        this.data   = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }

  delete = (payload : Object) : Promise<AxiosResponse> => {
    let endpoint  = new TokenEndpoint("post", "user/profile/delete/", UNIVERSAL_TOKEN)
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        this.clearLocalStorage()
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }

  // This update the profile
  changePassword = (payload : Object) : Promise<AxiosResponse> => {
    let endpoint  = new TokenEndpoint(
      'post', 'user/change-password/', UNIVERSAL_TOKEN
    )
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        this.data   = resp.data
        res(resp)
      })
      .catch((err) => rej(err))
    })
  }

  // Static Methods
  static storageKey = 'pharm-net'

  static login(payload : Object) : Promise<UserAPI>{
    let endpoint = new Endpoint('post', 'user/login/')
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        let user  = resp.data.user
        UNIVERSAL_TOKEN.set(
          resp.data.jwt, resp.data.token
        )
        res(new UserAPI(user))
      })
      .catch((err) => rej(err))
    })
  }

  static getLoginFields = () : Promise<AxiosResponse> => {
    let endpoint = new Endpoint('get', 'user/login/')
    return endpoint.req()
  }

  static register(payload : Object) : Promise<UserAPI>{
    let endpoint  = new Endpoint('post', 'user/register/')
    return new Promise((res, rej) => {
      endpoint.req(payload)
      .then((resp) => {
        let user  = resp.data.user
        UNIVERSAL_TOKEN.set(
          resp.data.jwt, resp.data.token
        )
        res(new UserAPI(user))
      })
      .catch((err) => rej(err))
    })
  }

  static loadFromLocal() : UserAPI{
    let load      = localStorage.getItem(UserAPI.storageKey)
    let loadStr   = Buffer.from(load ? load : '', 'base64').toString('utf-8')
    // if (loadStr.length > 0){
      let loadItem  = JSON.parse(loadStr)
      UNIVERSAL_TOKEN.loadToken()
      return new UserAPI(loadItem.user)
    // }
  }

  static checkUser(params : Object) : Promise<AxiosResponse>{
    let endpoint = new Endpoint('get', 'user/check/')
    return new Promise((res, rej) => {
      endpoint.req({}, {}, params)
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }

  // ****FORGOT PASSWORD PART****
  static requestForgetPassword(how:string, val:string) : Promise<AxiosResponse> {
    let postData: any  = {}
    postData['type'] = how
    if (how==="phone") {
      postData['phone_number'] = val
    } else {
      postData[how] = val
    }
    let endpoint = new Endpoint('post', 'user/forget-request/')
    return new Promise((res, rej) => {
      endpoint.req(postData)
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }

  static fetchSecurityQuestion(
    headers: Partial<AxiosRequestHeaders>
  ) : Promise<AxiosResponse>{
    let endpoint = new Endpoint('get', 'user/forget-change/')
    return new Promise((res, rej) => {
      endpoint.req({}, headers)
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }

  static answerSecurityQuestion(
    answer:string, headers: Partial<AxiosRequestHeaders>
  ) : Promise<AxiosResponse> {
    const postData = {security_a: answer}
    let endpoint = new Endpoint('post', 'user/forget-change/')
    return new Promise((res, rej) => {
      endpoint.req(postData, headers)
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }

  static changeForgetPassword(
    new_password: string, headers: Partial<AxiosRequestHeaders>
  ) : Promise<AxiosResponse> {
    const putData = {password: new_password}
    let endpoint = new Endpoint('put', 'user/forget-change/')
    return new Promise((res, rej) => {
      endpoint.req(putData, headers)
      .then((resp) => res(resp))
      .catch((err) => rej(err))
    })
  }
  // ****FORGOT PASSWORD PART****
}
