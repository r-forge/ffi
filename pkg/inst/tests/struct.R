library(ffi)

#testsuite struct1.c
test.1 <- as.typecode(list(uc="u8",d="d",ui="u32"))
fn.1 <- ffi.closure(function(s1) {
  print(s1)
  x = list(s1[[1]]+1,s1[[2]]-1,s1[[3]]+1)
  print(x)
  x
},ffi.signature(test.1,RET=test.1))
.FFI(fn.1,
     SIG=ffi.signature(test.1,RET=test.1),list(1,3.14159,555))


test.2 <- as.typecode(list(d1="d",d2="d"))
test.3 <- as.typecode(list(si="s32"))
test.4 <- as.typecode(list(u1="u32",u2="u32",u3="u32"))
test.5 <- as.typecode(list(c1="u8",c2="u8"))
test.6 <- as.typecode(list(f="f",d="d"))
test.7 <- as.typecode(list(f1="f",f2="f",d="d"))
test.8 <- as.typecode(list(f1="f",f2="f",f3="f",f4="f"))
test.9 <- as.typecode(list(f="f",i="u32"))



