library(ffi)

getenv <- BindFn(c("libc.dylib","getenv"),RET="*",retcvt=ffi.string,
                 str="*")
getenv("PATH")
getenv("R_HOME")


