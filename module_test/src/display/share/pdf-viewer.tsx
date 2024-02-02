import React from 'react'
import { PDFDocumentProxy } from 'pdfjs-dist';
import { Endpoint, TokenEndpoint, UNIVERSAL_TOKEN } from '../../API/base';
import { Document, pdfjs, Page } from 'react-pdf';
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faAngleLeft, faAngleRight } from "@fortawesome/free-solid-svg-icons";
import Loading from '../../core/loading/loading';
import "./pdf-viewer.css"
import EmptyList from '../../components/empty-list/emptylist';
import NoResult from 'assets/noresult/no-request.png';


pdfjs.GlobalWorkerOptions.workerSrc =
  `//cdnjs.cloudflare.com/ajax/libs/pdf.js/${pdfjs.version}/pdf.worker.js`;

type AutoQueryP<T> = {
  file : string,
  version : number,
  params : Record<string, any>
  render : React.ComponentType<{ file : Blob } & T>
  props : T
}

const AutoQuery = <T, >(
  {file : fileURL, version, render : Component, props, params} : AutoQueryP<T>
) => {
  const [file, setFile] = React.useState<Blob | null>(null)
  const [error, setError] = React.useState(false)
  React.useEffect(() => {
    if (fileURL.length === 0) return ;
    const isExternal = /(http|https)(.*)/.test(fileURL)
    const endpoint = isExternal ?
      new Endpoint(
        "GET", fileURL, "",
        {
          responseType : 'blob',
          withCredentials : false
        }
      ) :
      new TokenEndpoint(
        "GET", fileURL, UNIVERSAL_TOKEN, undefined, {responseType : 'blob'}
      )
    endpoint.req({}, {}, params)
    .then((resp) => {
      if (!(resp.data instanceof Blob))
        console.error("Error when querying ", fileURL, " response is not blob")
      setFile(resp.data)
    })
    .catch((err) => {
      setError(true)
      console.error(err)
    })
  }, [fileURL, setFile, version, setError, params])
  if (fileURL.length === 0)
    return (
      <EmptyList
        text="PDF Report has not been generated yet."
        img={NoResult}
        className="no-result"
      />
    )
  else if (file !== null)
    return <Component file = {file} {...props} />
  else if (!error)
    return <Loading />
  else
    return(
      <EmptyList
        text="There has been an error while loading the file."
        img={NoResult}
        className="no-result"
      />
    )
}

type PDFViewerP = {
  file : Blob | string
  version : number,
  className? : string,
  params : Record<string, any>
}

export const PDFViewer = ({file, version, className, params} : PDFViewerP) => {
  const [numOfPage, setNumOfPage] = React.useState(0)
  const [pageNum, setPageNum] = React.useState(0)
  const onLoadSuccess = React.useCallback((pdf : PDFDocumentProxy) => {
    setNumOfPage(pdf._pdfInfo.numPages)
    setPageNum(1)
  }, [setNumOfPage, setPageNum])
  const changePage = React.useCallback((offset : number) => {
    const newPage = offset + pageNum
    if (newPage > 0 && newPage <= numOfPage)
      setPageNum(newPage)
  }, [setPageNum, pageNum, numOfPage])
  if (file instanceof Blob)
    return (
      <div className = 'pdf-file-main'>
        <div className = 'pdf-file-container'>
          <Document
            file = {file}
            onLoadSuccess = {onLoadSuccess}
            loading = {<Loading />}
          >
            <Page
              pageNumber = {pageNum}
              renderAnnotationLayer = {false}
              renderTextLayer = {false}
              height = {window.innerHeight - 300}
            />
          </Document>
        </div>
        <div className = 'pdf-control'>
            <button className="icon-button" onClick = {() => changePage(-1)}>
              <FontAwesomeIcon icon = {faAngleLeft} />
            </button>
            <p>{pageNum} / {numOfPage}</p>
            <button className="icon-button" onClick = {() => changePage(1)}>
              <FontAwesomeIcon icon = {faAngleRight} />
            </button>
        </div>
      </div>
    )
  else
    return (
      <AutoQuery
        file = {file}
        version = {version}
        params = {params}
        render = {PDFViewer}
        props = {{
          className : className,
          version : version,
          params : params
        }}
      />
    )
}


export default PDFViewer
