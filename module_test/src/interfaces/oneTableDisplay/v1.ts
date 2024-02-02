import { MessagesT, StatusT, ProgressT, TimeT, QueryT, ControlsT } from "../displayCommon"
/**
 *
 * @brief
 * Display Type 1 : Table with Smart Boxes®
 */
/**
 * @entry src -> the image src
 * @entry textString -> what to show when not clicked
 */
export interface NGLFile_StringI{
  type : 'string', text : string
}
export interface NGLFile_QueryI{
  type : 'query', config : QueryT
}
export type NGLFileT = NGLFile_QueryI | NGLFile_StringI
export interface MsgBox_NGLI{
  type : 'ngl',
  files : Array<NGLFileT>
}
export interface MsgBox_StringI{
  type : 'string',
  content : string
}

export type MsgBoxT = MsgBox_NGLI | MsgBox_StringI

export interface TableCell_ImgI{
  src : string
  textString : string
}
export interface TableCell_ButtonI{
  buttonText : string
  hiddenText : string
}

export interface TableCell_ZoomableI{
  shortText : string
  zoomedText? : string
}
export type TableCell_InputT = {
  placeholder? : string
}
export type TableCellT =
  string | number | TableCell_ButtonI | TableCell_ZoomableI |
  TableCell_ImgI | React.ReactNode

/*
  TABLE ROW TYPES
*/
/**
 * @brief A Generic for Table Row types.
 */
export type TableType_GenericT<T, Extra> = {type : T} & Extra
/**
 * @brief Describes the configuration for table type
 * @entry string -> string entries
 * @entry number -> number entries
 * @entry id -> fill this row with the id of the cell
 */
export type TableType_ZoomableSortableT =
  TableType_GenericT<
    'string' | 'number' | 'id',
    {
      zoomable : 'normal' | 'admet' | 'none',
      sortable : boolean,
      fpAccuracy? : number
    }
  >
export type TableType_NGLT =
  TableType_GenericT<'ngl', {}>
export type TableType_ImageT =
  TableType_GenericT<'img', { height : number, width : number }>
export type TableType_Button_RedirectT =
  TableType_GenericT<'button', { action : 'redirect', src : string }>
export type TableType_InputT =
  TableType_GenericT<'input', { inputType : 'text' | 'number' | 'textarea' }>
/**
 * @brief Button
 * @note Parseable string will contain the following form $(hiddenText) which
 * should be replaced accordingly
*/
export type TableType_Button_APIT =
  TableType_GenericT<'button', { action : 'dl_api', config : QueryT }>

export type TableTypeT =
  TableType_ImageT | TableType_ZoomableSortableT | TableType_Button_RedirectT |
  TableType_Button_APIT | TableType_InputT

export type TableRowT = Array<TableCellT>

export type TableHeaderT = {
  displayName : React.ReactNode
  type : TableTypeT
}

export type TableTypesT = Array<TableTypeT>
export type TableHeadersT = Array<TableHeaderT>
export type TableRowsT = Array<TableRowT>

export type TableT = {
  headers : TableHeadersT,
  rows : TableRowsT
}

// Only one type for now but this allows for subsequent updates to make it
// better
export type SmartBoxT = {
  type : 0
  title : string
  content : string
}

export type SmartBoxesT = Array<SmartBoxT>

export type OneTableDisplayT = {
  version : "1.0"
  table : TableT
  progress : ProgressT,
  messages : MessagesT,
  status : StatusT,
  time : TimeT,
  smartBoxes : SmartBoxesT,
  controls : ControlsT
};
