import React from 'react';
import './table.css';
import {
  TableCell_ZoomableI,
  TableType_ZoomableSortableT
} from '../../../interfaces/oneTableDisplay/v1';
import Zoomable from "./Zoomable";
import { TokenEndpoint, UNIVERSAL_TOKEN } from '../../../API/base';


export type TextCellProps = {
  data: TableCell_ZoomableI | string | number | React.ReactNode,
  type: TableType_ZoomableSortableT
};


const isLigand = (text:string) => {
  const validLigandRegex = /(FDA|MCULE|ZINC|Z)/
  return validLigandRegex.test(text)
}

const TextCell = ({ data, type }: TextCellProps) => {
  const content = React.useMemo(() => {
    if (!type.fpAccuracy) return data
    if (typeof data === 'string')
      return parseFloat(data).toFixed(type.fpAccuracy)
    else if (typeof data === 'number')
      return data.toFixed(type.fpAccuracy)
    else if (type.zoomable === 'normal')
      return {
        shortText : parseFloat(
          (data as TableCell_ZoomableI).shortText
        ).toFixed(type.fpAccuracy),
        zoomedText : (data as TableCell_ZoomableI).zoomedText
      }
  }, [data, type])
  const redirect = React.useCallback(() => {
    const endpoint = new TokenEndpoint(
      'get', 'api-library/direct/ligand-name-to-zinc', UNIVERSAL_TOKEN
    )
    endpoint.req({}, {}, {ligand : content})
    .then((resp:any) => {
      window.open(resp.data, '_blank')
    })
    .catch((err:any) => console.error(err))
  }, [content])

  const noZoom = React.useMemo(() => {
    if (type.zoomable === 'normal'){
      return <p>{(content as TableCell_ZoomableI).shortText}</p>
    }
    else return content as React.ReactNode
  }, [data, type])

  const zoom = React.useMemo(() => {
    if (type.zoomable === 'normal'){
      const assertedData = content as TableCell_ZoomableI
      return (
        <p>
          {
            assertedData.zoomedText ?
            assertedData.zoomedText : assertedData.shortText
          }
        </p>
      )
    }
    else return content as React.ReactNode
  }, [data, type])

  const smile = React.useMemo(() => {
    if (type.zoomable === 'normal'){
      const assertedData = content as TableCell_ZoomableI
      return assertedData.zoomedText || assertedData.shortText
    }
    else return ''
  }, [data, type])

  if (
    type.type === 'string' && typeof content === 'string' && isLigand(content)
  ){
    return (
      <div className = 'table-cell-content zoomable' onClick = {redirect}>
        <p>{content}</p>
      </div>
    )
  }
  else if (type.zoomable === 'none')
    return (
      <div className = 'table-cell-content'>
        <p>{content as React.ReactNode}</p>
      </div>
    )
  else
    return (
      <Zoomable zoomable = {type.zoomable} noZoom = {noZoom} zoom = {zoom} smile={smile}/>
    )

};

export default TextCell
