import axios, {
    AxiosHeaders,
    AxiosRequestConfig,
    AxiosRequestHeaders,
    AxiosResponse,
    Method
  } from 'axios'
  import { Buffer } from 'buffer'
  
  abstract class TokenBase{
    static storageKey : string = 'pharm-token'
  }
  
  export class Token extends TokenBase{
    refreshToken : string
    jwtToken     : string
    constructor(refreshToken : string = '', jwtToken : string = ''){
      super()
      this.refreshToken   = refreshToken
      this.jwtToken       = jwtToken
    }
    clear(){
      this.refreshToken   = ''
      this.jwtToken       = ''
    }
    set(jwtToken : string = '', refreshToken : string = ''){
      if(jwtToken !== '') this.jwtToken = jwtToken
      if(refreshToken !== '') this.refreshToken = refreshToken
      this.saveToken()
    }
    saveToken(){
      const saveItem = {
        refreshToken : this.refreshToken, jwtToken : this.jwtToken
      }
      const saveStr  = Buffer.from(JSON.stringify(saveItem)).toString('base64')
      localStorage.setItem(Token.storageKey, saveStr)
    }
    loadToken(){
      const load    = localStorage.getItem(Token.storageKey)
      const loadStr = Buffer.from(load ? load : '', 'base64').toString('utf-8')
      const loadItem= JSON.parse(loadStr)
      this.refreshToken   = loadItem.refreshToken
      this.jwtToken       = loadItem.jwtToken
    }
  }
  
  abstract class BaseEndpoint{
    url     : string
    method  : Method
    config  : AxiosRequestConfig
    constructor(
      method : Method, url : string, base_url : string = '',
      config? : AxiosRequestConfig
    ){
      this.url = this._createUrl(base_url, url)
      this.method = method
      this.config = config ? config : {}
    }
    _createUrl(base_url : string, url : string){
      const isUrl = /(http|https)(.*)/.test(url)
      if (isUrl) return url
      else if(base_url === '')
        return `${process.env.REACT_APP_API_ENDPOINT}` + url
      else
        return base_url + url
    }
    abstract req() : Promise<AxiosResponse>
  }
  
  export class Endpoint extends BaseEndpoint{
    req(
      data : Object = {},
      headers : Partial<AxiosRequestHeaders> = {},
      params : Object = {}
    ){
      return axios.request({
        url     : this.url,
        method  : this.method,
        headers : headers as AxiosHeaders,
        params  : params,
        data    : data,
        ...this.config
      })
    }
  }
  
  export abstract class AuthEndpoint extends BaseEndpoint{
    token               : Token
    constructor(
      method : Method,
      url : string,
      token : Token = new Token(),
      base_url : string = '', config? : AxiosRequestConfig
    ){
      super(method, url, base_url, config)
      this.token        = token
    }
    abstract _createHeader() : Object
    abstract _createFallbackHeader() : Object
    abstract fallbackRequest() : Promise<AxiosResponse>
  
    // Common implementation
    req(
      data : Object = {},
      headers : Partial<AxiosRequestHeaders> = {},
      params : Object = {}
    ){
      return new Promise<AxiosResponse>((res, rej) => {
        axios.request({
          url : this.url,
          method : this.method,
          headers : {
            ...this._createHeader(), ...headers
          } as AxiosHeaders,
          params,
          data,
          ...this.config
        })
        .then((resp:any) => res(resp))
        .catch((err:any) => {
          if(err.response && err.response.status === 403){
            this.fallbackRequest()
            .then(() => {
              axios.request({
                url : this.url,
                method : this.method,
                headers : {
                  ...this._createHeader(), ...headers
                } as AxiosHeaders,
                params,
                data,
                ...this.config
              })
              .then((resp:any) => res(resp))
              .catch((err:any) => rej(err))
            })
            .catch((err) => rej(err))
          }
          else rej(err)
        })
      })
    }
  }
  
  export class TokenEndpoint extends AuthEndpoint{
    _createHeader(){
      return {Authorization : `Token ${this.token.jwtToken}`}
    }
    _createFallbackHeader(){
      return {Authorization : `Token ${this.token.jwtToken} ${this.token.refreshToken}`}
    }
    fallbackRequest() : Promise<AxiosResponse>{
      let endpoint  = new Endpoint('get', 'user/refresh-token/')
      let header    = this._createFallbackHeader()
      return new Promise((res, rej) => {
        endpoint.req({}, header)
        .then((resp: any) => {
          UNIVERSAL_TOKEN.set(resp.data.jwt)
          res(resp)
        })
        .catch((err: any) => rej(err))
      })
    }
  }
  
  export var UNIVERSAL_TOKEN = new Token()
  
  export abstract class APIObject{
    abstract data : Object
    abstract get() : Promise<AxiosResponse>
    abstract update(payload : Object) : Promise<AxiosResponse>
    abstract delete(payload? : Object) : Promise<AxiosResponse>
  }
  
  export type APIWithDetailsT<
    MinimalDataT,
    DetailedDataT,
    DetailedGetterT extends
    Record<
      string,
      (...args : any[]) => Promise<APIWithDetailsT<
        MinimalDataT,
        DetailedDataT,
        DetailedGetterT,
        true
      >>
    >,
    IsDetailed extends boolean
  > = {
    detailed : IsDetailed
    data : IsDetailed extends true ? DetailedDataT : MinimalDataT
    clone : () => APIWithDetailsT<
      MinimalDataT,
      DetailedDataT,
      DetailedGetterT,
      IsDetailed
    >,
  } &
  DetailedGetterT
  