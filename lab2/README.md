Parsing & Abstract Syntax
===================

> Chapter Three & Four, Modern Compiler Implementation in C.

Implement a parser and add semantic actions to produce abstract syntax for the Tiger language. See `tiger.grm`.

All given testcases passed, with one shift/reduce conflict left. 

Grammar
---
```
 0 $accept: program $end

 1 program: exp

 2 exp: varExp
 3    | nilExp
 4    | intExp
 5    | stringExp
 6    | callExp
 7    | opExp
 8    | recordExp
 9    | seqExp
10    | assignExp
11    | ifExp
12    | whileExp
13    | forExp
14    | breakExp
15    | letExp
16    | arrayExp

17 varExp: lvalue

18 nilExp: NIL

19 intExp: INT

20 stringExp: STRING

21 callExp: ID LPAREN argList RPAREN

22 opExp: exp PLUS exp
23      | exp MINUS exp
24      | exp TIMES exp
25      | exp DIVIDE exp
26      | exp EQ exp
27      | exp NEQ exp
28      | exp LT exp
29      | exp LE exp
30      | exp GT exp
31      | exp GE exp
32      | MINUS exp

33 recordExp: ID LBRACE efieldList RBRACE

34 seqExp: LPAREN seqList RPAREN

35 assignExp: lvalue ASSIGN exp

36 ifExp: IF exp THEN exp ELSE exp
37      | IF exp THEN exp
38      | exp OR exp
39      | exp AND exp

40 whileExp: WHILE exp DO exp

41 forExp: FOR ID ASSIGN exp TO exp DO exp

42 breakExp: BREAK

43 letExp: LET decList IN seqList END

44 arrayExp: ID LBRACK exp RBRACK OF exp

45 lvalue: ID
46       | lvalue DOT ID
47       | lvalue LBRACK exp RBRACK
48       | ID LBRACK exp RBRACK

49 argList: /* empty */
50        | exp argList_

51 argList_: /* empty */
52         | COMMA exp argList_

53 efield: ID EQ exp

54 efieldList: /* empty */
55           | efield efieldList_

56 efieldList_: /* empty */
57            | COMMA efield efieldList_

58 seqList: /* empty */
59        | exp
60        | exp SEMICOLON seqList

61 decList: dec
62        | dec decList

63 dec: typeDec
64    | varDec
65    | funcDec

66 typeDec: nametyList

67 varDec: VAR ID ASSIGN exp
68       | VAR ID COLON ID ASSIGN exp

69 funcDec: funcDecList

70 funcDecList: funcDec_
71            | funcDec_ funcDecList

72 funcDec_: FUNCTION ID LPAREN fieldList RPAREN COLON ID EQ exp
73         | FUNCTION ID LPAREN fieldList RPAREN EQ exp

74 nametyList: namety
75           | namety nametyList

76 namety: TYPE ID EQ ID
77       | TYPE ID EQ LBRACE fieldList RBRACE
78       | TYPE ID EQ ARRAY OF ID

79 field: ID COLON ID

80 fieldList: /* empty */
81          | field fieldList_

82 fieldList_: /* empty */
83           | COMMA field fieldList_
```

States
---
See `y.out`.