import React from 'react'
import { randomString } from 'libs/utils'

export type ListerMappedP<T, U> =
  { data : T, id : number } & U

type LegacyP<T, U> = {
  array : T[],
  render : React.ReactElement<ListerMappedP<T, U>>
}

export const LegacyLister =
<T, U>(props : LegacyP<T, U>) => {
  const randName = React.useMemo(() => randomString(10), [])
  return <React.Fragment>
    {props.array.map((val, id) => React.cloneElement(
      props.render, {
        key : randName + id.toString(),
        ...props.render.props,
        data : val,
        id : id,
      }
    ))}
  </React.Fragment>
}

type ListerP<T, U> = {
  array : T[],
  render : React.ComponentType<ListerMappedP<T, U>> |
    React.LazyExoticComponent<React.ComponentType<ListerMappedP<T, U>>>
  props : U
}

export const NewLister = <T, U>(props : ListerP<T, U>) => {
  const Component = props.render
  const randName = React.useMemo(() => randomString(10), [])
  const mappedContent = React.useMemo(() => {
    return props.array.map((val, id) => {
      const newProps = {
        key : randName + id.toString(),
        data : val,
        id : id,
        ...props.props
      } as (
        ListerMappedP<T, U> &
        JSX.IntrinsicAttributes &
        React.PropsWithRef<ListerMappedP<T, U>>
      )
      return <Component {...newProps} />
    })
  }, [randName, props.render, props.array, props.props])
  return (
    <React.Fragment>
      {mappedContent}
    </React.Fragment>
  )
}


type LegacySupportP<T, U> =
  LegacyP<T, U> | ListerP<T, U>

const Lister = <T, U>(props : LegacySupportP<T, U>) => {
  return "props" in props ?
    <NewLister {...props} /> : <LegacyLister {...props} />
}

export default Lister
