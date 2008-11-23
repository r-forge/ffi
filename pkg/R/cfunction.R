##
## C Functions
ffi.closure <- function(FUN,SIG=ffi.signature()) .Call("FFI_MakeClosure",FUN,SIG)

