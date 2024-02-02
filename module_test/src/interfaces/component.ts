import { DisplayDataT } from "./display";

export interface SocketControl{
  start : () => void
  stop : () => void
}

export interface DisplayProps{
  component : DisplayDataT,
  controls : SocketControl
}

export type DisplayComponent = React.ComponentType<DisplayProps>
