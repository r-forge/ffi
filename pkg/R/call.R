.FFI   <- function(sym,...,SIG) .Call("FFI_Call",sym,SIG,list(...))
BindFn <- function(sym,...,RET="v",retcvt=NULL) {
  sig <- ffi.signature(...,RET=RET)
  function(...) {
    x=.Call("FFI_Call",sym,sig,list(...))
    if(is.function(retcvt)) retcvt(x) else x
  }
}
#
# For building packages with ffi. Does a lookup within the package DLL
#
.LibFn <- function(symbol,sig,PACKAGE=.packageName,converter=NULL) {
  sym <- getNativeSymbolInfo(symbol,PACKAGE)
  if(is.null(sym))
    stop(paste("Unable to locate symbol",symbol,"in package",PACKAGE))
  function(...) {
    if(is.function(converter))
      converter(.Call("FFI_Call",sym$address,sig,list(...)))
    else
      .Call("FFI_Call",sym$address,sig,list(...))
  }
}

