"use strict";(self.webpackChunkmodule_test=self.webpackChunkmodule_test||[]).push([[224],{9843:(e,o,t)=>{t.r(o),t.d(o,{default:()=>s});var r=t(9060),a=(t(9600),t(6916)),l=t(8619),n=t(2496);const s=e=>{let{data:o,type:t}=e;const s=r.useMemo((()=>t.fpAccuracy?"string"===typeof o?parseFloat(o).toFixed(t.fpAccuracy):"number"===typeof o?o.toFixed(t.fpAccuracy):"normal"===t.zoomable?{shortText:parseFloat(o.shortText).toFixed(t.fpAccuracy),zoomedText:o.zoomedText}:void 0:o),[o,t]),c=r.useCallback((()=>{new l.Yv("get","api-library/direct/ligand-name-to-zinc",l.m6).req({},{},{ligand:s}).then((e=>{window.open(e.data,"_blank")})).catch((e=>console.error(e)))}),[s]),m=r.useMemo((()=>"normal"===t.zoomable?(0,n.jsx)("p",{children:s.shortText}):s),[o,t]),d=r.useMemo((()=>{if("normal"===t.zoomable){const e=s;return(0,n.jsx)("p",{children:e.zoomedText?e.zoomedText:e.shortText})}return s}),[o,t]),i=r.useMemo((()=>{if("normal"===t.zoomable){const e=s;return e.zoomedText||e.shortText}return""}),[o,t]);return"string"===t.type&&"string"===typeof s&&/(FDA|MCULE|ZINC|Z)/.test(s)?(0,n.jsx)("div",{className:"table-cell-content zoomable",onClick:c,children:(0,n.jsx)("p",{children:s})}):"none"===t.zoomable?(0,n.jsx)("div",{className:"table-cell-content",children:(0,n.jsx)("p",{children:s})}):(0,n.jsx)(a.c,{zoomable:t.zoomable,noZoom:m,zoom:d,smile:i})}}}]);
//# sourceMappingURL=224.f4fec325.chunk.js.map