import { StatusT, MessagesT, ProgressT, TimeT } from "../displayCommon"

export interface NoTableDisplayI{
  version : "1.0",
  progress : ProgressT,
  messages : MessagesT,
  status : StatusT,
  time : TimeT
}
