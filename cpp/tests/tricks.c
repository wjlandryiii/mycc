#define FOO(A,B) (A*B)
#define BAZ FOO(zzz, qqq
BAZ)
// output: (zzz*qqq)
