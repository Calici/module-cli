import './table.css';
import { TableCell_ButtonI, TableType_Button_APIT, TableType_Button_RedirectT } from '../../../interfaces/oneTableDisplay/v1';
import Zoomable from "./Zoomable";
type ButtonCellProps = {
  data: TableCell_ButtonI,
  type: TableType_Button_APIT | TableType_Button_RedirectT
};

const ButtonCell = ({ data }: ButtonCellProps) => {
    return (
        <Zoomable
          zoomable= {'normal'}
          noZoom= {<p>{data.buttonText}</p>}
          zoom = {<p>{data.hiddenText}</p>}
        />
      )
};
export default ButtonCell
