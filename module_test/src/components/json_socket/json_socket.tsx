export interface Socket{
  send : (msg : Object) => void
  close : () => void
}

export interface SocketConfig<T extends Socket>{
  onOpen : (sock : T) => void
  onError : (ev : Event) => void
  onClose : (ev : CloseEvent) => void
  onMsg : (msg : any, socket : T) => void
  endpoint : string
  protocols? : string
}


export default class JSONSocket implements Socket{
  socket : WebSocket
  constructor(config : SocketConfig<JSONSocket>){
    const socket  = new WebSocket(config.endpoint, config.protocols)

    socket.onmessage = (ev : MessageEvent) => {
      const msgObj = JSON.parse(ev.data)
      config.onMsg(msgObj, this)
    }
    socket.onopen = () => config.onOpen(this)
    socket.onerror = config.onError
    socket.onclose = config.onClose
    this.socket = socket
  }
  send = (msg : any) => {
    this.socket.send(JSON.stringify(msg))
  }
  close = () => {
    this.socket.close()
  }
}
