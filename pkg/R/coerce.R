as.typecode   <- function(val) {
  x <- .Call("FFI_asTypeCode",val)
  class(x) <- "typecode"
  x
}

ffi.signature <- function(...,RET="v") {
  x <- .Call("FFI_makeCIF",as.typecode(RET),lapply(list(...),as.typecode))
  class(x) <- "ffi.signature"
  x
}
summary.ffi.signature <- function(x,...) {
  x <- .Call("FFI_SummaryCIF",x)
  names(x) <- c("RET","SIG")
  x
}
as.foreign   <- function(value,type) .Call("FFI_asType",as.typecode(type),value)
from.foreign <- function(value,type) .Call("FFI_fromType",as.typecode(type),value)

ffi.vector <- function(value,typecode,n)
  .Call("FFI_fromPtr",as.typecode(typecode),value,as.integer(n))
ffi.string <- function(value)
  .Call("FFI_fromCStringType",value)

ffi.pointer <- function(value) {
  x = .Call("FFI_TypeCodePointer",as.typecode(value))
  class(x) <- "typecode"
  x
}

print.typecode <- function(x,...) invisible(.Call("FFI_TypeCodePrint",x))
