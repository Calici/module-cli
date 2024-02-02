import { TableT as TableT_v1 } from "./oneTableDisplay/v1";
import { TableConfig, TableConfig as TableT_v0 } from "./oneTableDisplay/v0";

const adaptZoomable = (
  initial : TableConfig["types"][number]["zoomable"]
) : 'normal' | 'admet' | 'none' => {
  if (initial === undefined) return 'none'
  else if (typeof(initial) === 'string') return initial
  else {
    return initial === true ? 'normal' : 'none'
  }
}

export const adaptV0TableToV1 = (table : TableT_v0) : TableT_v1 => {
  const headers = table.headers
  const rows = table.rows
  const types = table.types
  const haveIdColumn = rows.length > 0 && (rows[0].length != headers.length)

  const adaptedHeaders = table.headers.map(
    (header, id) => {
      const tableType = types[id]
      return {
        displayName : header,
        type : {
          type : (haveIdColumn && id === 0) ?
            'id' as 'id' : 'string' as 'string',
          zoomable : adaptZoomable(tableType.zoomable),
          sortable : tableType.sortable === true ? true : false
        }
      }
    }
  )
  const adaptedRows : TableT_v1["rows"] = rows.map(
    (row) => {
      row = haveIdColumn ? ["", ...row] : row
      return row.map((cell, id) => {
        const header = adaptedHeaders[id]
        if (header.type.zoomable === 'normal')
          return {
            shortText : cell, zoomedText : cell
          }
        else
          return cell
      })
    }
  )
  return {
    headers : adaptedHeaders, rows : adaptedRows
  }
}
