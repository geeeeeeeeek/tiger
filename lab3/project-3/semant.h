#ifndef SEMANT_H
#define SEMANT_H

#include "env.h"
typedef void *Tr_exp;

string immutable;
int reset;

struct expty {Tr_exp exp; Ty_ty ty; } ;


struct expty expTy(Tr_exp exp, Ty_ty ty);

void SEM_transProg(A_exp exp);
struct expty transVar (S_table venv, S_table tenv, A_var v);
struct expty transExp (S_table venv, S_table tenv, A_exp a);
void         transDec (S_table venv, S_table tenv, A_dec d);
       Ty_ty transTy  (		     S_table tenv, A_ty a);

bool assertSameType(Ty_ty expected,Ty_ty actual);

Ty_ty actual_ty(Ty_ty ty);
Ty_tyList makeFormalTyList(S_table tenv, A_fieldList a_fielfList);

#endif
