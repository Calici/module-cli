import { ControlsT, MessagesT, ProgressT, StatusT, TimeT } from "../displayCommon"
import { SmartBoxesT } from "../oneTableDisplay/v1"

export type PDFFileT = {
  url : string | Blob,
  params : Record<string, any>,
  version : number // this is just a signal to refresh the PDF File
}

export type PDFDisplayT = {
  version : "0.0",
  progress : ProgressT,
  messages : MessagesT,
  status : StatusT,
  time : TimeT,
  smartBoxes : SmartBoxesT,
  controls : ControlsT
  pdf_file : PDFFileT
}
