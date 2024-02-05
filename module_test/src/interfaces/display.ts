import { NoTableDisplayI as NoTableDisplayI_v0 } from "./noTableDisplay/v0";
import { NoTableDisplayI as NoTableDisplayI_v1 } from "./noTableDisplay/v1";
import { OneTableDisplayI as OneTableDisplayI_v0 } from "./oneTableDisplay/v0";
import { OneTableDisplayT as OneTableDisplayI_v1 } from "./oneTableDisplay/v1";

export type DisplayDataT =
  | NoTableDisplayI_v0
  | NoTableDisplayI_v1
  | OneTableDisplayI_v0
  | OneTableDisplayI_v1;

/**
 * @brief dtype 0 -> no table, dtype 1 -> one table
 */
export type DisplayTypeT = 0 | 1;

export interface DisplayArrayAppendI<T> {
  type: "append";
  element: T;
}
/**
 * @brief Reorders the array with newOrder
 * @param newOrder => array of indexes where [new_pos_of_elm_0, new_pos_of_elm1, ...]
 * @example given initial array [elm0, elm1, elm2, elm3] and newOrder [2, 0, 3, 1]
 * the array will be reordered into [elm1, elm3, elm0, elm2]
 */
export interface DisplayArrayReorderI {
  type: "reorder";
  newOrder: Array<number>;
}
export interface DisplayArrayModifyI<T> {
  type: "modify";
  pos: number;
  newElem: RecursivePartial<T>;
}
export interface DisplayArrayRemoveI {
  type: "remove";
  pos: number;
}
export interface DisplayArrayEmptyI {
  type : "empty"
}
export type DisplayArrayOp<T> = DisplayArrayAppendI<T>
  | DisplayArrayReorderI
  | DisplayArrayModifyI<T>
  | DisplayArrayRemoveI
  | DisplayArrayEmptyI

type RecursivePartial<T> = {
  [P in keyof T]?: RecursivePartial<T[P]>;
};

export type PartialDisplayDataT = RecursivePartial<DisplayDataT>;

interface Display_InitI {
  _init: true;
  _data: DisplayDataT;
  _dtype: DisplayTypeT;
}
interface Display_UnInitI {
  _init: false;
  _data?: DisplayDataT;
  _dtype?: DisplayTypeT;
}
export default class Display {
  static DEFAULT_VERSION = "0.0";
  _data: Display_InitI | Display_UnInitI;
  constructor() {
    this._data = { _init: false };
  }
  isComplete = (): boolean => {
    if (!this._data._init) throw Error("Display has not been initialized");
    return this._data._data.status === "COMPLETE";
  };
  isInitialized = (): boolean => {
    return this._data._init;
  };
  isInitializedOrException = () => {
    if (!this.isInitialized()) throw Error("Display has not been initialized");
  };
  dtype = (): number => {
    if (!this._data._init) throw Error("Display has not been initialized");
    return this._data._dtype;
  };
  version = (): string => {
    if (!this._data._init) throw Error("Display has not been initialized");
    const version = this._data._data.version;
    return version ? version : Display.DEFAULT_VERSION;
  };
  init = (payload: DisplayDataT, dtype: DisplayTypeT): void => {
    this._data._init = true;
    this._data._dtype = dtype;
    this._data._data = payload;
  };
  set = (payload: PartialDisplayDataT, dtype?: DisplayTypeT) => {
    if (!this._data._init) throw Error("Display has not been initialized");
    if (dtype) this._data._dtype = dtype;
    Display.recursiveSet(this._data._data, payload);
  };
  get = (): DisplayDataT => {
    if (!this._data._init) throw Error("Display has not been initialized");
    return this._data._data;
  };


  static reorderArray = (arr : Array<any>, op : DisplayArrayReorderI) => {
    for (let i = 0; i < arr.length; i++) {
      while (op.newOrder[i] != i) {
        const oldTargetI = op.newOrder[op.newOrder[i]];
        const oldTargetE = arr[op.newOrder[i]];

        arr[op.newOrder[i]] = arr[i];
        op.newOrder[op.newOrder[i]] = op.newOrder[i];

        op.newOrder[i] = oldTargetI;
        arr[i] = oldTargetE;
      }
    }
  }
  static modArray = (arr: Array<any>, op: DisplayArrayOp<any> | Array<any>) => {
    // Implement Array operation based on type, do not create new object
    if (Array.isArray(op)){
      arr.splice(0, arr.length)
      op.forEach((elm) => arr.push(elm))
    }
    else Display.modArrayWithDict(arr, op)
  };
  static modArrayWithDict = (arr : Array<any>, op : DisplayArrayOp<any>) => {
    switch (op.type) {
      case "append":
        arr.push(op.element);
        break
      case "reorder":
        this.reorderArray(arr, op)
        break
      case "modify":
        const { pos, newElem } = op;
        const currentElement = arr[pos];
        if (typeof currentElement === "object") {
          Display.recursiveSet(currentElement, newElem);
        }
        else {
          arr[pos] = newElem;
        }
        break
      case "empty":
        arr.length = 0
        break
      case "remove":
        arr.splice(arr[op.pos], 1);
        break
    }
  }
  static modObject = (
    current: Record<string, any>, payload: Record<string, any>
  ) => {
    // Implement Recursive Setting
    Object.keys(payload).forEach((key) => {
      if (current.hasOwnProperty(key)) {
        const curElm = current[key];
        if (typeof(curElm) === "object")
          Display.recursiveSet(curElm, payload[key]);
        else
          current[key] = payload[key]
      }
      else {
        current[key] = payload[key];
      }

    });
  };

  static recursiveSet = (current: any, payload: any) => {
    if (Array.isArray(current)) {
      Display.modArray(current, payload);
    }
    else if (typeof current === "object" && typeof payload === 'object') {
      Display.modObject(current, payload);
    }
  };
}
