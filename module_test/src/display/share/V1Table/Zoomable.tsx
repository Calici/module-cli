import MessageBox from "components/messagebox/messagebox"
import React from "react"
import {
  TableType_ZoomableSortableT
} from "interfaces/oneTableDisplay/v1"
import {
  FontAwesomeIcon
} from "@fortawesome/react-fontawesome"
import {
  faCheckCircle,
  faTimesCircle,
  faDotCircle
} from "@fortawesome/free-solid-svg-icons"
import Lister from "components/lister/lister"
import { TokenEndpoint, UNIVERSAL_TOKEN } from 'API/base';
import { isEmptyString } from 'libs/validator'
interface ZoomableP{
  zoomable: TableType_ZoomableSortableT["zoomable"],
  noZoom: React.ReactNode,
  zoom: React.ReactNode,
  smile?: string,
}

type ParsedAdmetData = {
  name : string
  data : {
    Disposed : 'Accepted' | 'Rejected'
    Parameter : string
  }[]
}[]

type AdmetItemItemP = {
  data : ParsedAdmetData[number]["data"][number]
  id : number
}

const AdmetItemItem = ({data, id} : AdmetItemItemP) => {
  const isAccepted = (data.Disposed === 'Accepted')
  const isRejected = (data.Disposed === 'Rejected')
  const mapToStyles = isAccepted ? 'accept' : isRejected ? 'reject' : ''
  return (
    <div className = 'admet-item-row'>
      <div className = {'admet-item-parameter ' + mapToStyles}>
        {
          isAccepted ?
          <FontAwesomeIcon icon={faCheckCircle}/> :
          isRejected ?
          <FontAwesomeIcon icon={faTimesCircle}/> :
          <FontAwesomeIcon icon={faDotCircle}/>
        }
        {data.Parameter}
      </div>
      <div className = {`admet-item-value ` + mapToStyles}>
        {data.Disposed}
      </div>
    </div>
  )
}

type AdmetItemP = {
  data : ParsedAdmetData[number]
  id : number
}

const AdmetItem = ({data, id} : AdmetItemP) => {
  return (
    <div className = 'admet-item-block'>
      <div className = 'admet-item-title'>{data.name}</div>
      <Lister
        array = {data.data}
        render = {AdmetItemItem}
        props = {{}}
      />
    </div>
  )
}

const Zoomable = ({zoomable, noZoom, zoom, smile} : ZoomableP) => {
  const [isZoomed, setIsZoomed] = React.useState(false)
  const toggleZoom = React.useCallback(() => {
    setIsZoomed((isZoomed) => !isZoomed)
  }, [setIsZoomed])

  const isSmile = (text:string) => {
    return true
    // const validSmileRegex = /^([^J][0-9BCOHNSOPrIFla@+\-\[\]\(\)\\\/%=#$,.~&!]{6,})$/
    // return validSmileRegex.test(text)
  }


  const redirect = React.useCallback(() => {
    // if (typeof zoom == 'object' && zoom?.hasOwnProperty('props')) {
      if (smile!==undefined && !isEmptyString(smile) && isSmile(smile)){
        const endpoint = new TokenEndpoint(
          'get', 'api-library/direct/ligand-name-to-zinc', UNIVERSAL_TOKEN
        )
        endpoint.req({}, {}, {smile : smile})
        .then((resp: any) => {
          window.open(resp.data, '_blank')
        })
        .catch((err: any) => console.error(err))
      }
  }, [smile])

  const content = React.useMemo(() => {
    if (zoomable === 'normal')
      return (
        <React.Fragment>
          <MessageBox
            state={isZoomed}
            className="message-box-normal-zoom"
          >
            <div className='zoom-cell-content normal-zoom' onClick = {redirect}>
              {zoom}
            </div>
            <div className='zoom-close-button'>
              <button
                onClick={toggleZoom}
                className='popup-btn-close'
              >
                Close
              </button>
            </div>
          </MessageBox>
          <div
            className='table-cell-content smiles'
            onClick={toggleZoom}
          >
            {noZoom}
          </div>
        </React.Fragment>
      )
    else if (zoomable === 'none')
      return (
        <div className = 'table-cell-content'>
          {noZoom}
        </div>
      )
    else {
      const admetData = JSON.parse(noZoom as string)
      return (
        <div className = 'admet-item'>
          <React.Fragment>
            <MessageBox
              state={isZoomed}
              className="message-box-admet-zoom"
            >
              <div className='zoom-cell-content admet-zoom'>
                <Lister
                  render = {AdmetItem}
                  props = {{}}
                  array = {admetData.data}
                />
              </div>
              <div className='zoom-close-button'>
                <button
                  onClick={toggleZoom}
                  className='popup-btn-close'
                >
                  Close
                </button>
              </div>
            </MessageBox>
            <div
              className='table-cell-content zoomable admet-zoomable'
              onClick={toggleZoom}
            >
              {admetData.title}
            </div>
          </React.Fragment>
        </div>
      )
    }
  }, [zoomable, zoom, noZoom, toggleZoom, isZoomed])
  return (
    <div className = 'zoomable-container'>
      {content}
    </div>
  )
}

export default Zoomable;
