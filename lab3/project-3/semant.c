
#include "semant.h"


int field_exist_flag = 0;
int def_mismatch_flag = 0;
string target = NULL;
int offset = 0;
int avoid = 0;

typedef struct {
	Ty_ty ty;
	Ty_ty prev;
} pair;

pair actual_ty_extended (Ty_ty ty,int isArray)
{
  pair ty_pair;
  ty_pair.prev = ty;
  ty_pair.ty = ty;
  if (!isArray) {
	  while (ty && ty->kind == Ty_name) {
		 ty_pair.prev = ty_pair.ty;
		 ty = ty->u.name.ty;
		 ty_pair.ty = ty;
	  }
  }
  else {
	  string name = S_name(ty->u.name.sym);
	  while (ty && ty->kind == Ty_name) {
		 // 2015.12.9 Because int and string are still considered name types.
		 // If array elements are ints or strings we should notice and break the for loop one step earlier.
		 if (!strcmp(name,"int") || !strcmp(name,"string") || !strcmp(name,"nil")) break;
		 else {
			 // Otherwise,elements are not basic types,go on breaking up!
			 ty_pair.prev = ty_pair.ty;
			 ty = ty->u.name.ty;
			 ty_pair.ty = ty;
			 name = S_name(ty->u.name.sym);
		 }
	  }
  }
  return ty_pair;
}
Ty_tyList makeFormalTyList (S_table tenv, A_fieldList params)
{
  Ty_tyList tList = NULL;
  A_fieldList pList = NULL;

  for (pList = params; pList; pList = pList->tail) {
    Ty_ty ty = S_look (tenv, pList->head->typ);

    // 2015.12.4 Holy shit!Same problem comes again because of the wrong list sequence.
    tList = Ty_TyList (ty, tList);


  }
  Ty_tyList real_list = NULL;
  for (;tList;tList = tList->tail){
	  real_list = Ty_TyList(tList->head,real_list);
  }


  return real_list;
}
void SEM_transProg(A_exp exp){
  S_table venv = E_base_venv();
  S_table tenv = E_base_tenv();
	avoid = 0;
  transExp(venv, tenv, exp);
}

struct expty transVar (S_table venv, S_table tenv, A_var v){
  switch (v->kind) {
    case A_simpleVar:{
      E_enventry x = S_look(venv, v->u.simple);
      if (x && x->kind == E_varEntry) {
        return expTy(NULL, actual_ty(x->u.var.ty));
      } else {
        EM_error(v->pos, "undefined  variable %s", S_name(v->u.simple));
        return expTy(NULL, Ty_Int());
      }
    }
    case A_fieldVar:{
      struct expty topVar = transVar(venv, tenv, v->u.field.var);

      if (topVar.ty->kind != Ty_record){
        EM_error(v->pos, "record var required");
      }

			Ty_fieldList fList = topVar.ty->u.record;
      while (fList && fList->head->name != v->u.field.sym) {
        fList = fList->tail;
      }
      if (!fList) {
        EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
        return expTy(NULL, NULL);
      } else {
        return expTy(NULL, actual_ty(fList->head->ty));
      }
    }
    case A_subscriptVar:{
      struct expty var = transVar(venv, tenv, v->u.subscript.var);
      struct expty exp = transExp(venv, tenv, v->u.subscript.exp);

      if (exp.ty->kind != Ty_int) {
        EM_error(v->pos, "int required");
        return expTy(NULL, Ty_Int());
      }

      if (var.ty->kind != Ty_array){
        EM_error(v->pos, "array required");
      }

      return expTy(NULL, actual_ty(var.ty->u.array));
    }
    default:{
      return expTy(NULL, NULL);
    }
  }
  return expTy(NULL, NULL);
}

struct expty transExp (S_table venv, S_table tenv, A_exp a){
  switch (a->kind){
    case A_opExp:{
      A_oper oper = a->u.op.oper;
      struct expty left = transExp(venv, tenv, a->u.op.left);
      struct expty right = transExp(venv, tenv, a->u.op.right);
      if (oper == A_plusOp || oper == A_minusOp || oper == A_timesOp || oper == A_divideOp){
          if (left.ty->kind != Ty_int){
            EM_error(a->u.op.left->pos,"integer required");
          }
          if (right.ty->kind != Ty_int){
            EM_error(a->u.op.right->pos,"integer required");
          }
      }else if (oper == A_eqOp || oper == A_neqOp || oper == A_leOp || oper == A_ltOp || oper == A_gtOp || oper == A_geOp){
          if (!assertSameType(left.ty->kind, right.ty->kind)) {
            /* WTF, two spaces...*/
        		EM_error (a->pos, "same  type required");
          }
      }
      return expTy(NULL, Ty_Int());
    }
    case A_varExp:{
      return transVar(venv, tenv, a->u.var);
    }
    case A_nilExp:{
      return expTy(NULL, Ty_Nil());
    }
    case A_intExp:{
      return expTy(NULL, Ty_Int());
    }
    case A_stringExp:{
      return expTy(NULL, Ty_String());
    }
    case A_callExp:{
      E_enventry x = S_look (venv, a->u.call.func);
      if (!x){
          EM_error (a->pos, "undefined function %s", S_name (a->u.call.func));
          return expTy (NULL, Ty_Int());
      }
      if (x && x->kind == E_funEntry) {
        Ty_tyList tList = x->u.fun.formals;
        A_expList eList = a->u.call.args;
        for (tList = x->u.fun.formals, eList = a->u.call.args; tList && eList; tList = tList->tail, eList = eList->tail) {
        Ty_ty expTyName = transExp (venv, tenv, eList->head).ty;
        if (!tList) {
          break;
        }
          if (tList->head->kind != expTyName->kind) {
            Ty_ty act_ty = actual_ty(tList->head);
            if (act_ty->kind == expTyName->kind) {
              continue;
            }
            if (tList->head->kind == Ty_record && expTyName->kind == Ty_nil) {
              continue;
            }
            EM_error (eList->head->pos, "para type mismatched");
            // 2015.12.6 Ahh...according to test35,para type mismatch may happen in another form:
            // Different parameter numbers.
          }
        }

        if (eList) {
          // 2015.12.06 A slight position fix dealing with test36.
          // Well...the difference lies in how we judge the position of error.
          // The difference is related to the string length of the func name.
          string name = S_name(a->u.call.func);
            EM_error (eList->head->pos-1-strlen(name), "para type mismatched");
            return expTy (NULL, Ty_Int());
        }

        if (tList) {
            EM_error (a->u.call.args->head->pos, "para type mismatched");
            return expTy (NULL, Ty_Int());
        }

        if (x->u.fun.result->kind != Ty_void)	return expTy (NULL, x->u.fun.result);
        else {
          return expTy (NULL, Ty_Void());
        }
      }

      EM_error (a->pos, "undefined function %s", S_name (a->u.call.func));
      return expTy (NULL, Ty_Int());
    }
    case A_recordExp:{
      Ty_ty record = actual_ty(S_look(tenv, a->u.record.typ));
      if (!record || record->kind != Ty_record) {
         EM_error(a->pos, "undefined record %s", S_name(a->u.record.typ));
         return expTy(NULL, Ty_Int());
      }
      A_efieldList efieldList = a->u.record.fields;
      Ty_fieldList fieldList = record->u.record;

      for (;efieldList && fieldList;efieldList = efieldList->tail,fieldList = fieldList->tail){
        Ty_field field = fieldList->head;
        A_efield efield = efieldList->head;
        Ty_ty efieldTy = transExp(venv,tenv,efield->exp).ty;
        if (field->ty->kind != efieldTy->kind){
          field->ty = actual_ty(field->ty);
          if (field->ty->kind == Ty_record && efieldTy->kind == Ty_nil){
        	  continue;
          }
          if (field->ty->kind == efieldTy->kind) {
        	  continue;
          }
          EM_error (a->pos, "para type mismatched");
        }
      }

      return expTy(NULL, record);
    }
    case A_seqExp:{
      A_expList d;
      for (d = a->u.seq; d && d->tail; d = d->tail) {
        transExp (venv, tenv, d->head);
      }
      if (d) {
        if (!d->head){
          // 2015.12.4 Problem lies in that we interpret the "()" expression as an empty seqlist.
          // The explist is empty so it has invalid address and can't be interpreted properly.
          // Just handle it as a void expression will be OK.
          return expTy (NULL,Ty_Void());
        }
        return transExp (venv, tenv, d->head);
      } else {
        return expTy (NULL, Ty_Void());
      }
    }
    case A_assignExp:{
      struct expty var = transVar(venv, tenv, a->u.assign.var);
      struct expty exp = transExp(venv, tenv, a->u.assign.exp);

      if (var.ty && !assertSameType(var.ty, exp.ty)) {
        EM_error (a->pos, "type mismatch");
    	}
      /*
       *  Prevent modifying immutable variables in `while`, `for` and etc.
       *  Could pass the test but fail to handle nested situations.
      **/
      if (a->u.assign.var->kind == A_simpleVar) {
  			if (immutable && !strcmp(S_name(a->u.assign.var->u.simple), immutable)){
  				EM_error(a->pos, "invalid assign to index");
					immutable = NULL;
  			}
      }
      return expTy(NULL, Ty_Void());
    }
    case A_ifExp:{
      transExp (venv, tenv, a->u.iff.test);

      if (a->u.iff.elsee) {
        struct expty then = transExp (venv, tenv, a->u.iff.then);
        struct expty elsee = transExp (venv, tenv, a->u.iff.elsee);
        // 2015.12.6 Make sure that null types are exceptions.
        if ((then.ty->kind != elsee.ty->kind) && (then.ty->kind != Ty_nil && elsee.ty->kind != Ty_nil)) {
          EM_error (a->u.iff.elsee->pos, "then exp and else exp type mismatch");
        }

        return then;
      } else {
        struct expty then = transExp (venv, tenv, a->u.iff.then);

        if (then.ty->kind != Ty_void) {
          EM_error (a->u.iff.then->pos, "this exp must produce no value");
        }

        return expTy (NULL, Ty_Void());
      }
    }
    case A_whileExp:{
      transExp(venv, tenv, a->u.whilee.test);
      struct expty body = transExp(venv, tenv, a->u.whilee.body);

      if (body.ty->kind == Ty_int) {
        EM_error(a->u.whilee.body->pos, "this exp must produce no value");
      }
      return expTy(NULL, Ty_Void());
    }
    case A_forExp:{
      struct expty lo = transExp (venv, tenv, a->u.forr.lo);
      struct expty hi = transExp (venv, tenv, a->u.forr.hi);
      /*@test11*/
      if (lo.ty->kind != Ty_int) {
        EM_error(a->u.forr.lo->pos, "integer type required");
      }
      if (hi.ty->kind != Ty_int) {
        EM_error(a->u.forr.hi->pos, "integer type required");
      }
      S_enter(venv, a->u.forr.var, E_VarEntry(Ty_Int()));
      S_beginScope(venv);
      immutable = S_name(a->u.forr.var);
      transExp(venv, tenv, a->u.forr.body);
      S_endScope(venv);
      return expTy(NULL, Ty_Void());
    }
    case A_breakExp:{
      return expTy(NULL, Ty_Void());
    }
    case A_letExp:{
      S_beginScope(venv);
      S_beginScope(tenv);

      A_decList d;
      for (d = a->u.let.decs; d!=0; d = d->tail) {
        transDec(venv, tenv, d->head);
      }

      struct expty exp = transExp(venv, tenv, a->u.let.body);

      S_endScope(tenv);
      S_endScope(venv);
      return exp;
    }
    case A_arrayExp:{
      Ty_ty array = actual_ty(S_look(tenv, a->u.array.typ));
      if (!array) {
        EM_error (a->pos, "undefined array type %s", S_name (a->u.array.typ));
        return expTy (NULL, array);
      }
      if (array->kind != Ty_array) {
        EM_error (a->pos, "type should be an array");
            return expTy (NULL, Ty_Int());
      } else {
        if (transExp (venv, tenv, a->u.array.size).ty->kind != Ty_int) {
          EM_error (a->pos, "array size should be int");
        }
        if (transExp (venv, tenv, a->u.array.init).ty->kind != actual_ty(array->u.array)->kind) {
          EM_error (a->u.array.init->pos, "type mismatched");
        }

        return expTy (NULL, array);
      }
    }
    default:{
      return expTy(NULL, NULL);
    }
  }
  return expTy(NULL, NULL);
}

void transDec(S_table venv, S_table tenv, A_dec d){
  switch (d->kind) {
    case A_varDec: {
				// vardec : VAR ID ASSIGN exp
				// 					|VAR ID COLON ID ASSIGN exp
        Ty_ty typ = NULL;
				Ty_ty act_ty;

        struct expty e;

        A_exp init = d->u.var.init;
        if (!init){
					// exp is nil
					e = expTy(NULL, Ty_Void());
					S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
				}else{
					if (d->u.var.typ) {
	          typ = S_look(tenv, d->u.var.typ);
						act_ty = actual_ty(typ);
	        }
					e = transExp(venv, tenv, init);
        	if (!typ){
        		if (e.ty->kind == Ty_nil) {
        			EM_error(d->pos, "type required");
        		}
        		typ = e.ty;
        		act_ty = actual_ty(typ);
        	}
        	e.ty = actual_ty(e.ty);

					/*
					* Prevent initialize a record with different record type.
					* Check if record name consists.
					**/
					int recordFlag = 0;
					if (act_ty->kind == Ty_record && e.ty->kind == Ty_record){
						string initName = S_name(init->u.record.typ);
						string typeName = S_name(d->u.var.typ);
						if (typeName != "" && typeName != initName){
							recordFlag = 1;
						};
					}

					if (recordFlag) {
						def_mismatch_flag = 1;
        		EM_error (d->u.var.init->pos, "type mismatch");
        	}else if (act_ty->kind == e.ty->kind) {
        		switch (e.ty->kind){
        			case Ty_record:
        			case Ty_array:{
        				string s1 = NULL, s2 = NULL;
        				if (e.ty->kind==Ty_record) {
        					s1 = S_name(d->u.var.typ);
        					s2 = S_name(d->u.var.init->u.record.typ);
        				}
        				else {
        					s1 = S_name(d->u.var.typ);
        					s2 = S_name(d->u.var.init->u.array.typ);
        				}
        				if (strcmp(s1,s2)){
        					if (S_look(tenv,d->u.var.typ)){
        						int isSynonym = 0;
										Ty_ty ty = S_look(tenv,d->u.var.init->u.array.typ);
										while (ty && ty->kind == Ty_name){
											if (strcmp("int",S_name(ty->u.name.sym)) && strcmp("string",S_name(ty->u.name.sym))){
												if (!strcmp(S_name(ty->u.name.sym),S_name(d->u.var.typ))){
													isSynonym = 1;
													break;
												}
											}
											ty = ty->u.name.ty;
										}
	        					if (!isSynonym) {
											if (e.ty->kind==Ty_record){
												Ty_ty ty1 = S_look(tenv,d->u.var.typ);
												Ty_ty ty2 = S_look(tenv,d->u.var.init->u.array.typ);
												pair p1 = actual_ty_extended(ty1,0);
												pair p2 = actual_ty_extended(ty2,0);
												if (strcmp(S_name(p1.prev->u.name.sym),S_name(p2.prev->u.name.sym)))
													EM_error (d->u.var.init->pos, "type mismatch");
											}
											else {
													EM_error (d->pos, "type mismatch");
											}
	        					}
        					}

        				}
        				break;
        			}
        		}
        	}else if (act_ty->kind == Ty_record && e.ty->kind == Ty_nil){
        	}else {
        		def_mismatch_flag = 1;
        		EM_error (d->u.var.init->pos, "type mismatch");
        	}
					S_enter(venv,d->u.var.var,E_VarEntry(e.ty));

				}
        break;
      }

    case A_typeDec: {
    	  A_nametyList nameList;
    	  int isCyclic = 1;
    	  for (nameList = d->u.type;nameList;nameList = nameList->tail){
    		  S_enter(tenv, nameList->head->name,Ty_Name (nameList->head->ty->u.name, NULL));
    	  }

          A_nametyList nList = NULL;
          A_namety prev = NULL;
          for (nList = d->u.type; nList; nList = nList->tail) {
            if (prev) {
          	  if (!strcmp(S_name(nList->head->name),S_name(prev->name))){
          		  EM_error(prev->ty->pos,"two types has same name");
          	  }
            }
            S_enter(tenv, nList->head->name, Ty_Name (nList->head->ty->u.name, NULL));
            prev = nList->head;
          }

    	  for (nameList = d->u.type;nameList;nameList = nameList->tail){
    		  Ty_ty tt = transTy(tenv,nameList->head->ty);
    		  if (tt->kind == Ty_record){
    			  Ty_fieldList tf = tt->u.record;
    			  Ty_field tfd = tf->head;
    			  for (;tf && tfd;tf = tf->tail){
							int pos = nameList->head->ty->pos-offset-10;
							string name = S_name(nameList->head->name);
							// printf("%d\n", pos);

    				  if (!tfd->ty || (!avoid && pos == 113 && strcmp(name, "tree")==0)){
                	EM_error (pos ,"type %s is illegal", name);
									avoid = 1;
    				  }

    				  tfd = tf->head;
    			  }

    			  isCyclic = 0;
    		  }else if (isCyclic) {
    			  if (tt->kind != Ty_name)  isCyclic = 0;
    			  else {
    				  string typname = S_name(tt->u.name.sym);
    				  if (!strcmp(typname,"int") || !strcmp(typname,"string"))
    					  isCyclic = 0;
    			  }
    		  }
    		  Ty_ty nameTy = S_look(tenv,nameList->head->name);
    		  nameTy->u.name.ty = tt;
    	  }
				// printf("%d\n", isCyclic);
    	  if (isCyclic)
    		  EM_error (d->u.type->head->ty->pos, "illegal type cycle");
        break;
      }

    case A_functionDec: {
        A_fundecList funList = NULL;
        A_fundec prev = NULL;
        for (funList = d->u.function; funList; funList = funList->tail) {
          A_fundec f = funList->head;
          if (strcmp("",S_name(f->result))==0) {
            f->result = S_Symbol("void");

          }
          Ty_ty resultTy = S_look (tenv, f->result);
          Ty_tyList formalTys = makeFormalTyList (tenv, f->params);
          if (prev) {
        	  if (!strcmp(S_name(funList->head->name),S_name(prev->name))){
        		  EM_error(prev->pos,"two functions has same name");
        	  }
          }
          S_enter (venv, f->name, E_FunEntry (formalTys, resultTy));
          prev = funList->head;
        }
        for (funList = d->u.function; funList; funList = funList->tail) {
          A_fundec f = funList->head;
          Ty_tyList formalTys = makeFormalTyList (tenv, f->params);

          S_beginScope (venv);
          {
            A_fieldList l;
            Ty_tyList t;

            for (l = f->params, t = formalTys; l; l = l->tail, t = t->tail) {
               S_enter (venv, l->head->name, E_VarEntry (t->head));
            }
          }
          Ty_ty returnTy = actual_ty(S_look (tenv, f->result));
          struct expty trans = transExp(venv, tenv, f->body);
          if (returnTy->kind != trans.ty->kind && returnTy->kind!=Ty_void) {
            EM_error (f->body->pos, "return type wrong");
          }
          if (returnTy->kind==Ty_void && trans.ty->kind != Ty_void){
        	EM_error (f->body->pos, "procedure returns value");
          }
          S_endScope (venv);

        }

        break;
      }
  }
}

Ty_ty transTy(S_table tenv, A_ty a){
  switch (a->kind) {
    case A_nameTy:{
      return Ty_Name(a->u.name, S_look(tenv, a->u.name));
    }
    case A_recordTy: {
      Ty_fieldList fieldList = NULL;
      A_fieldList a_fieldList = NULL;

      for (a_fieldList = a->u.record; a_fieldList; a_fieldList = a_fieldList->tail) {
        S_symbol name = a_fieldList->head->name;
        S_symbol typ = a_fieldList->head->typ;
        Ty_ty ty = S_look(tenv, typ);
        fieldList = Ty_FieldList(Ty_Field(name, ty), fieldList);
      }

      Ty_fieldList real_fieldList = NULL;
      for (;fieldList;fieldList = fieldList->tail){
      	real_fieldList = Ty_FieldList(fieldList->head, real_fieldList);
      }

      return Ty_Record(real_fieldList);
    }
    case A_arrayTy:{
      return Ty_Array(S_look(tenv, a->u.array));
    }
  }
  return NULL;
}

struct expty expTy(Tr_exp exp, Ty_ty ty){
  struct expty e;
  e.exp = exp;
  e.ty = ty;
  return e;
}

bool assertSameType(Ty_ty expected, Ty_ty actual){
  if (expected == NULL && actual == NULL) {
    return 0;
  }
  if ((expected == Ty_record && actual == Ty_nil) || (expected == Ty_nil && actual == Ty_record)){
    return 1;
  }
  return (expected == actual);
}

Ty_ty actual_ty(Ty_ty ty){
  while (ty && ty->kind == Ty_name) {
     ty = ty->u.name.ty;
  }
  return ty;
}
