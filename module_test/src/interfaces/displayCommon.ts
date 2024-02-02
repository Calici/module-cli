export type MessageT = string
export type MessagesT = Array<MessageT>
export type StatusT = 'COMPLETE' | 'STOP' | 'RUNNING' | 'INIT' | 'ERROR'

export type ProgressT = {
  value : number,
  bouncy : boolean
}

export type TimeT = {
  startTime : string,
  timeDelta : number
}

export type ControlsT = {
  show_run : boolean, show_stop : boolean
}

export type QueryT = {
  method : 'GET' // Only support GET for now
  requestType? : 'blob' // This allow for overriding
  endpoint : string // This should be checked to prevent querying from other endpoints
  params : any //string in the form of "a string which can be parseable"
}
