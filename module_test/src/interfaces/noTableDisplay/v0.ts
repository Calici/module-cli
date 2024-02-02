import { StatusT } from "../displayCommon"

export interface NoTableDisplayI{
  version : "0.0",
  progress : number,
  messages : Array<string>,
  status : StatusT,
  start_time  : string
}
