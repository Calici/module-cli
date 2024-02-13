import './table.css';
import { TableCell_ImgI, TableType_ImageT } from 'interfaces/oneTableDisplay/v1';
import Zoomable from "./Zoomable";

type ImageCellProps = {
  data: TableCell_ImgI,
  type: TableType_ImageT
};

export const ImageCell = ({ data }: ImageCellProps) => {
    return (
        <Zoomable
          zoomable= {'normal'}
          noZoom= {<p>{data.textString}</p>}
          zoom = {<p>{data.src}</p>}
        />
      )
};

export default ImageCell;
