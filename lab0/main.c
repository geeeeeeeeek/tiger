#include <stdio.h>

#include "util.h"
#include "slp.h"
#include "prog1.h"

int exp_helper(A_expList list){
	if (list->kind == A_lastExpList){
		return 1;
	}
	return 1 + exp_helper(list->u.pair.tail);
}

int maxargs(A_stm prog){
	switch (prog->kind){
		case A_compoundStm :{
			int left = maxargs(prog->u.compound.stm1);
			int right = maxargs(prog->u.compound.stm2);
			return (left > right)? left : right;
		}
		case A_assignStm:{
			A_exp exp = prog->u.assign.exp;
			if (exp->kind == A_eseqExp){
				return maxargs(exp->u.eseq.stm);
			}else{
				return 0;
			}
		}
		case A_printStm:{
			A_expList list = prog->u.print.exps;
			return exp_helper(list);
		}
		default :
			return 0;
	}
}

typedef struct table *Table_;
struct table {string id; int value; Table_ tail;};
Table_ Table(string id, int value, struct table *tail) {
	Table_ t = checked_malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
}

typedef struct IntAndTable *iTable_;
struct IntAndTable {int value; Table_ t;};
iTable_ iTable(int value, Table_ t){
	iTable_ it = checked_malloc(sizeof(*it));
	it->value = value;
	it->t = t;
	return it;
}

Table_ interpStm(A_stm, Table_);
iTable_ interpExp(A_exp, Table_);
Table_ interpExpList(A_expList, Table_);
int ID_VALID = 1;

int lookup(Table_ t, string id){
	while ((t != NULL) && (t->id != id)) {
		t = t->tail;
	}
	if (t != NULL){
		return t-> value;
	}else{
		ID_VALID = 0;
		return -1;
	}
}

Table_ update(Table_ t, string id, int value){
	return Table(id, value, t);
}

Table_ interpStm(A_stm s, Table_ t){
	switch (s->kind) {
		case A_compoundStm:{
			t = interpStm(s->u.compound.stm1, t);
			t = interpStm(s->u.compound.stm2, t);
			return t;
		}
		case A_assignStm:{
			iTable_ it = interpExp(s->u.assign.exp, t);
			t = update(it->t, s->u.assign.id, it->value);
			return t;
		}
		case A_printStm:{
			t = interpExpList(s->u.print.exps, t);
			printf("\n");
			return t;
		}
		default:
			return t;
	}
}

iTable_ interpExp(A_exp e, Table_ t){
	switch (e->kind){
		case A_idExp:{
			int value = lookup(t, e->u.id);
			if (!ID_VALID){
				printf("[Error] Identifier %s does not exist!\n", e->u.id);
				ID_VALID = 1;
			}
			return iTable(value, t);
		}
		case A_numExp:{
			iTable_ it = iTable(e->u.num, t);
			return it;
		}
		case A_opExp:{
			iTable_ left = interpExp(e->u.op.left, t);
			iTable_ right = interpExp(e->u.op.right, t);
			switch (e->u.op.oper) {
				case A_plus:
					return iTable(left->value + right->value, t);
				case A_minus:
					return iTable(left->value - right->value, t);
				case A_times:
					return iTable(left->value * right->value, t);
				case A_div:
					return iTable(left->value / right->value, t);
				default:
					return NULL;
			}
		}
		case A_eseqExp:{
			t = interpStm(e->u.eseq.stm, t);
			iTable_ it = interpExp(e->u.eseq.exp, t);
			return it;
		}
		default:
			return NULL;
	}
}

Table_ interpExpList(A_expList e, Table_ t){
	switch (e->kind) {
		case A_pairExpList:{
			iTable_ it = interpExp(e->u.pair.head, t);
			printf("%d ", it->value);
			t = interpExpList(e->u.pair.tail, it->t);
			return t;
		}
		case A_lastExpList:{
			iTable_ it = interpExp(e->u.last, t);
			printf("%d ", it->value);
			return it->t;
		}
		default:
			return t;
	}
}


void interp(A_stm prog){
	interpStm(prog, NULL);
}

// DO NOT CHANGE!
int main(){
	printf(">> Right Prog Section:\n");
	A_stm rp = right_prog();
	printf("the maximum number of arguments of any print statement is %d\n",maxargs(rp));
	interp(rp);

	printf(">> Error Prog Section:\n");
	A_stm ep = error_prog();
	printf("the maximum number of arguments of any print statement is %d\n",maxargs(ep));
	interp(ep);
	return 0;
}
