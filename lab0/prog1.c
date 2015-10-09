#include "util.h"
#include "slp.h"

A_stm prog(void) {
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
return 
 A_CompoundStm(A_AssignStm("a",
                 A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
  A_CompoundStm(A_AssignStm("b",
      A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
                 A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                       A_NumExp(1))))),
              A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
   A_PrintStm(A_LastExpList(A_IdExp("b")))));
}

A_stm prog_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, a, a-1), 10*a); print b;
     A_stm stm1 = prog();
     return
	 	A_CompoundStm( stm1, A_CompoundStm(A_AssignStm("a",
                 A_OpExp(A_NumExp(5), A_plus, A_IdExp("b"))),
  A_CompoundStm(A_AssignStm("b",
      A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),A_PairExpList(A_IdExp("a"),
                 A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                       A_NumExp(1)))))),
              A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
   A_PrintStm(A_LastExpList(A_IdExp("b"))))));

}

A_stm right_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, a, a-1), 10*a); print b;
// a = (a = a+b, a);
     A_stm stm1 = prog_prog();
     return
	 	A_CompoundStm( stm1, A_AssignStm("a",
      A_EseqExp(A_AssignStm("a", A_OpExp(A_IdExp("a"), A_plus, A_IdExp("b"))),
              A_IdExp("a"))));

}

A_stm error_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, a, a-1), 10*a); print b;
// a = c;
     A_stm stm1 = prog_prog();
     return
	 	A_CompoundStm( stm1, A_AssignStm("a", A_IdExp("c")));

}
