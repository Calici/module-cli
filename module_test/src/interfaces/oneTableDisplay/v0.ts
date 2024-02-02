import { ControlsT, StatusT } from "../displayCommon"

export interface TableTypes{
  type?     : string,
  zoomable? : 'normal' | 'none' | 'admet' | boolean,
  sortable? : boolean
}

export interface TableConfig{
  headers   : Array<string>,
  types     : Array<TableTypes>,
  rows      : Array<Array<string>>
}

export interface OneTableDisplayI{
  version     : "0.0"
  progress    : number,
  messages    : Array<string>,
  status      : StatusT,
  table       : TableConfig,
  start_time  : string
  controls : ControlsT
}
