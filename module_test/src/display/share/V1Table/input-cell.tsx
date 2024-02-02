import React from "react"
import Input from "../../../components/input/input"
import {
  TableType_InputT ,
  TableCell_InputT
} from "../../../interfaces/oneTableDisplay/v1"

export type InputCellP = {
  data : TableCell_InputT,
  type : TableType_InputT
  onChange : (value : any) => void
}

const InputCell = (props : InputCellP) => {
  const onChange = React.useCallback(
    (ev : React.ChangeEvent<HTMLInputElement>) => {
      props.onChange(ev.target.value)
    }, [ props ]
  )
  return (
    <div className = 'component-table-cell input'>
      <Input
        status = {true}
        type = {props.type.inputType}
        onChange = {onChange}
        placeholder = {props.data.placeholder}
      />
    </div>
  )
}

export default InputCell
