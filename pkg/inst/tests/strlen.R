library(ffi)

strlen <- BindFn(c("libc.dylib","strlen"),RET="u32",
                 str="*")

strlen("1")
strlen("1234567")
strlen("Hello World")


