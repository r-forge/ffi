library(ffi)

#Set up a closure
testclos <- ffi.closure(function(x,i) { print(paste(ffi.string(x),i));i+1 },
                        ffi.signature("*","s32",RET="s32"))
print(testclos)
#Call the closure as if it were an FFI
.FFI(testclos,SIG=ffi.signature("*","s32",RET="s32"),"Hello World",15)
