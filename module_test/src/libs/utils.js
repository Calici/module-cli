/**
 * Create random string
 *
 * @param {number} length
 * @returns
 */
export function randomString(length) {
    let result           = '';
    let characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    for(let i = 0; i < length; i++){
      result += characters.charAt(Math.floor(Math.random() * characters.length));
    }
    return result;
  }
  
  /**
   * Format date as 'dd/mm/yyyy'
   *
   * @param {string | number} date : date data
   * @returns
   */
  export function formatDate(date){
    date = new Date(date)
    return [date.getDate(), date.getMonth()+1, date.getFullYear()]
    .map(n => n < 10 ? `0${n}` : `${n}`).join('/');
  }
  
  /**
   * Format date as 'HH:MM:SS dd/mm/yyyy'
   *
   * @param {string | number} date : date data
   * @returns
   */
  export function formatDateTime(date){
    date = new Date(date)
    return ([date.getHours(), date.getMinutes(), date.getSeconds()].map(n => n < 10 ? `0${n}` : `${n}`).join(':'))+ ' ' +
    ([date.getDate(), date.getMonth()+1, date.getFullYear()].map(n => n < 10 ? `0${n}` : `${n}`).join('/'))
  
    ;
  }
  
  export function imageLink(link){
    return `${process.env.REACT_APP_BACKEND_ENDPOINT}${link}`
  }
  
  export function formParser(format){
    if(format.type !== 'checkbox' && format.type !== 'radio'){
      return {
        type  : format.type,
        status: true,
        value : undefined,
        warn  : "",
        title: format.msg
      }
    }
    else{
      return{
        type  : format.type,
        value : undefined,
        warn  : "",
        title: format.title,
        options: format.options.map((val, id) => {return {option : val, checked : false}})
      }
    }
  }
  
  export function logError(err, desc="Response value:") {
    if (err?.response?.data){
      console.error(desc, err.response.data)
      return {status:err.response.status, statusText:err.response.statusText, data:err.response.data }
    } else {
      console.error(desc, err)
      return err
    }
  }
  
  export function logInfo(...data) {
    if (process.env.REACT_APP_DEBUG!=="true"){
      return
    }
    console.info(data)
  }
  
  /**
   * Convert number to string as K, M, B
   * Etc: 1000 = 1K, 2000000 = 2M
   *
   * @param {number} amount
   * @returns Object {amount, unit}
   */
  export function convertAmountToKMBUnit(amount){
    let ret = {
      amount:amount,
      unit:'',
    }
    if(amount < 1_000){}
    else if(amount >= 1_000 && amount < 1_000_000){
      ret.amount = amount / 1_000
      ret.unit = "K"
    }
    else if(amount >= 1_000_000 && amount < 1_000_000_000){
      ret.amount = amount / 1_000_000
      ret.unit = "M"
    } else {
      ret.amount = amount / 1_000_000_000
      ret.unit = "B"
    }
  
    return ret
  }
  export function formatNumberWithCommas(number) {
    return number.toLocaleString();
  }
  
  
  /**
   * Format date as 'yyyy/mm/dd' or 'yyyy-mm-dd
   *
   * @param {Date} inputDate : date data
   * @param {string} sep : separator character
   * @returns
   */
  export function dateToStringSimple (inputDate, sep='/'){
    let date, month, year
    date = inputDate.getDate()
    month = inputDate.getMonth() + 1
    year = inputDate.getFullYear()
    date = date.toString().padStart(2, '0')
    month = month.toString().padStart(2, '0')
    return `${year}${sep}${month}${sep}${date}`
  }
  
  /**
   * Format date as 'yyyy/mm/dd HH:MM:SS' or 'yyyy-mm-dd HH:MM:SS'
   *
   * @param {Date} inputDate : date data
   * @param {string} sep : separator date character
   * @param {string} sepTime : separator time character
   * @returns
   */
  export function dateTimeToStringSimple (inputDate, sep='/', sepTime=':'){
    let date, month, year, hours, minutes, seconds
    date = inputDate.getDate()
    month = inputDate.getMonth() + 1
    year = inputDate.getFullYear()
    date = date.toString().padStart(2, '0')
    month = month.toString().padStart(2, '0')
    hours = inputDate.getHours().toString().padStart(2, '0')
    minutes = inputDate.getMinutes().toString().padStart(2, '0')
    seconds = inputDate.getSeconds().toString().padStart(2, '0')
    return `${year}${sep}${month}${sep}${date} ${hours}${sepTime}${minutes}${sepTime}${seconds}`
  }
  
  export function checkNaN (value, retVal=0){
    if (Number.isNaN(value)) {return retVal}
    return value
  }
  
  /**
   * Convert number to string as Korean format
   *
   * @param {number} value
   * @returns
   */
  export function numberToString(value) {
    const localNumberFormatter = new Intl.NumberFormat('ko-KR');
    return localNumberFormatter.format(value)
  }
  
  export function convertToFourDecimalValue(value){
    if(isNumeric(value)){
      const number = parseFloat(value);
      const roundedNumber = number.toFixed(4);
      if (parseFloat(roundedNumber) === 0) {
        return "0";
      } else {
        return roundedNumber;
      }
    }
    else {
      return value
    }
  
  }
  
  export function isNumeric(str) {
    if (typeof str != "string") return false
    return !isNaN(str) &&
      !isNaN(parseFloat(str))
  }
  
  
  export class SearchSortUtil {
    static FilterType = {
      E: '=',
      G: '>',
      L: '<',
      GE: '>=',
      LE: '<=',
      IN: 'in',
      IN_IGNORE_CASE: 'in_ignore_case',
    }
  
    static SortType = {
      ASC: 'ascending',
      DESC: 'descending',
    }
  
  
  
    static filterAndSort(data, params, propertyName='') {
      /*
      params = {
        filter: {
          name: {
            compare_type:SearchSortUtil.FilterType.IN_IGNORE_CASE,
            value:'test'
          },
          member_count: {
            compare_type:SearchSortUtil.FilterType.G,
            value:0
          },
        },      //  name has 'test' AND member_count > 0
        sort: {
          name: SearchSortUtil.SortType.ASC   // name is one property of data[0]
        } // Sort by name ASC
      }
  
      propertyName='data' -> sorting, filtering property is data[0]['data']
      */
      let ret = data
      if (ret === undefined || ret === null || ret.length === 0) {
        return []
      }
      if (propertyName!=='' && !ret[0].hasOwnProperty(propertyName)) {
        throw new Error(`Error in filterAndSort, ret[0] has no '${propertyName}' property.}`)
      }
      if (params['filter'] !== undefined) {
        ret = ret.filter((item, index) => {
          let retValue = item
          if (propertyName!=='') {
            retValue = retValue[propertyName]
          }
          for (const [k, v] of Object.entries(params['filter'])) {
            if (v===undefined ||  !v.hasOwnProperty('compare_type') || !v.hasOwnProperty('value')) {
              continue
            }
            if (retValue != null && retValue.hasOwnProperty(k)) {
              switch (v.compare_type) {
                case SearchSortUtil.FilterType.E:
                  if (retValue[k] === v.value) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.L:
                  if (retValue[k] < v.value) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.G:
                  if (retValue[k] > v.value) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.LE:
                  if (retValue[k] <= v.value) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.GE:
                  if (retValue[k] >= v.value) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.IN:
                  if (retValue[k].indexOf(v.value) != -1) {
                  } else {
                    return false
                  }
                  break
                case SearchSortUtil.FilterType.IN_IGNORE_CASE:
                  if (typeof retValue[k].toUpperCase === "function" && typeof v.value.toUpperCase === "function") {
                    if (retValue[k].toUpperCase().indexOf(v.value.toUpperCase()) != -1) {
                    } else {
                      return false
                    }
                  } else {
                    return false
                  }
                  break
                default:
                  return false
                  break
              }
            }
          }
          return true
        })
      }
      if (ret.length > 0 && params['sort'] !== undefined) {
        let bFound = false
        for (const [k, v] of Object.entries(params['sort'])) {
          if (bFound) {
            break
          }
          if (propertyName !== '') {
            if (!ret[0]['data'].hasOwnProperty(k)) {
              continue
            }
          } else {
            if (!ret[0].hasOwnProperty(k)) {
              continue
            }
          }
          bFound = true
          if (v == SearchSortUtil.SortType.DESC) {
            ret = ret.sort((a, b) => {
              if (propertyName !== '') {
                return (a[propertyName][k] > b[propertyName][k] ? -1 : 0)
              } else {
                return (a[k] > b[k] ? -1 : 0)
              }
            })
          } else {
            ret = ret.sort((a, b) => {
              if (propertyName !== '') {
                return (a[propertyName][k] < b[propertyName][k] ? -1 : 0)
              } else {
                return (a[k] < b[k] ? -1 : 0)
              }
            })
          }
        }
      }
      return ret
    }
  }
  