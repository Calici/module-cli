import UserAPI from '../API/user'
import React, {createContext, useContext, useEffect, useState} from 'react'
import {
  useParams,
  useNavigate ,
  useLocation,
  useSearchParams,
  NavigateFunction,
  Params,
  Location,
} from 'react-router-dom'

export interface NavigateParamsType{
  navigate  : NavigateFunction | null,
  match     : {
    params        : Readonly<Params<string>> | null
    searchParams  : any | null
  },
  user      : {
    user      : UserAPI | null,
    setUser   : null | ((user : UserAPI | null) => void),
    isLoggedIn: boolean | null
  },
  location  : Location | null
}

export interface UserContextType{
  user  : UserAPI | null,
  setUser : (user : UserAPI | null) => void,
  isLoggedIn : boolean
}

export const UserContext = React.createContext<UserContextType>({
  user : null, setUser : () => {}, isLoggedIn : false
})

export const NavigateParamsContext = React.createContext<NavigateParamsType>({
  navigate : null,
  match : {params : null, searchParams : null},
  user : {user : null, setUser : null, isLoggedIn : null},
  location: null
})

export interface NotificationType {
  title : string,
  content : string,
  read : boolean,
  type : 'ERROR' | 'COMPLETE' | 'GENERAL' | 'WARNING',
  module : number,
  date : string,
  id : number
}

export interface NotificationContextProps {
  notifications : NotificationType[],
  markAsread : (message:NotificationType) => void,
  reSort : () => void,
  isReady : boolean
}

export const NotificationContext = React.createContext<NotificationContextProps >({
  notifications : [],
  markAsread : () => {},
  reSort : () => {},
  isReady : false
});



export const NavigateParams:React.FC<React.PropsWithChildren>=({children}) => {
  const params      = useParams()
  const navigate    = useNavigate()
  const user        = useContext(UserContext)
  const location    = useLocation()
  const searchParams= useSearchParams()
  const state       = {
    navigate : navigate,
    match : {params : params, searchParams : searchParams},
    user : user,
    location : location
  }
  const childArr    = React.Children.toArray(children)
  if (!childArr || childArr.length > 1)
    throw Error("Only one children is allowed")
  return (
    <NavigateParamsContext.Provider value = {state}>
      {children}
    </NavigateParamsContext.Provider>
  )
}


