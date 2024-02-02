import React from 'react'
import './input.css'

export interface NormalInputProps extends
  React.InputHTMLAttributes<HTMLInputElement>, React.PropsWithChildren
{
  status    : boolean,
  type      : React.HTMLInputTypeAttribute
}
export interface TextAreaInputProps extends
  React.TextareaHTMLAttributes<HTMLTextAreaElement>, React.PropsWithChildren
{
  status    : boolean,
  type      : "textarea"
}
type InputProps = TextAreaInputProps | NormalInputProps

export default class Input extends React.Component<InputProps>{
  render(): React.ReactNode {
    const {children, status, type, ...other}  = this.props
    const error   = status ? "" : "error"
    if(this.props.type === 'textarea')
      return (
        <div className = {`input ${error}`}>
          <textarea
            {...other as React.TextareaHTMLAttributes<HTMLTextAreaElement>}
          />
          {children}
        </div>
      )
    else
      return (
        <div className = {`input ${error}`}>
          <input
            type = {type}
            {...other as React.InputHTMLAttributes<HTMLInputElement>}
          />
          {children}
        </div>
      )
  }
}
