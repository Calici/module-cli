import NewTable, { TableP as NewTableP } from "./new-table";
import OldTable , { TableProps }from "./old-table";


type TableP<T, U, V, W> = NewTableP<T, U, V, W> | TableProps

const Table = <T, U, V, W>(props : TableP<T, U, V, W>) => {
  return "rowProps" in props ?
    <NewTable {...props} /> : <OldTable {...props} />
}

export default Table
