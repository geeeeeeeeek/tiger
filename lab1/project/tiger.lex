%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos = 1;

int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

int commentNesting = 0;

/* String buffer. */
const int INITIAL_BUFFER_LENGTH = 32;
char *string_buffer;
unsigned int string_buffer_capacity;
unsigned int STRING_START = 0;

void init_string_buffer(void)
{
  string_buffer = checked_malloc(INITIAL_BUFFER_LENGTH);
  string_buffer[0] = 0;
  string_buffer_capacity = INITIAL_BUFFER_LENGTH;
}

void append_char_to_stringbuffer(char ch)
{
    size_t new_length = strlen(string_buffer) + 1;
    if (new_length == string_buffer_capacity)
    {
        char *temp;

        string_buffer_capacity *= 2;
        temp = checked_malloc(string_buffer_capacity);
        memcpy(temp, string_buffer, new_length);
        free(string_buffer);
        string_buffer = temp;
    }
    string_buffer[new_length - 1] = ch;
    string_buffer[new_length] = 0;
}

%}

%x COMMENT_STATE STRING_STATE

%%
  /* Skip white spaces. */
[ \r\t]	 {adjust(); continue;}
\n	 {adjust(); EM_newline(); continue;}


  /* Reserved words. */
while     {adjust(); return WHILE;}
for       {adjust(); return FOR;}
to        {adjust(); return TO;}
break     {adjust(); return BREAK;}
let       {adjust(); return LET;}
in        {adjust(); return IN;}
end       {adjust(); return END;}
function  {adjust(); return FUNCTION;}
var       {adjust(); return VAR;}
type      {adjust(); return TYPE;}
array     {adjust(); return ARRAY;}
if        {adjust(); return IF;}
then      {adjust(); return THEN;}
else      {adjust(); return ELSE;}
do        {adjust(); return DO;}
of        {adjust(); return OF;}
nil       {adjust(); return NIL;}

  /* Punctuation symbols. */
","   {adjust(); return COMMA;}
":"   {adjust(); return COLON;}
";"   {adjust(); return SEMICOLON;}
"("   {adjust(); return LPAREN;}
")"   {adjust(); return RPAREN;}
"["   {adjust(); return LBRACK;}
"]"   {adjust(); return RBRACK;}
"{"   {adjust(); return LBRACE;}
"}"   {adjust(); return RBRACE;}
"."   {adjust(); return DOT;}
"+"   {adjust(); return PLUS;}
"-"   {adjust(); return MINUS;}
"*"   {adjust(); return TIMES;}
"/"   {adjust(); return DIVIDE;}
"="   {adjust(); return EQ;}
"<>"  {adjust(); return NEQ;}
"<"   {adjust(); return LT;}
"<="  {adjust(); return LE;}
">"   {adjust(); return GT;}
">="  {adjust(); return GE;}
"&"   {adjust(); return AND;}
"|"   {adjust(); return OR;}
":="  {adjust(); return ASSIGN;}

  /* Identifiers. */
[a-z|A-Z]+[a-z|A-Z|0-9|_]*  {
  adjust();
  yylval.sval = yytext;
  return ID;
}

  /* Integer literals. */
[0-9]+	 {
  adjust();
  yylval.ival=atoi(yytext);
  return INT;
}

  /* String literals. */
\"  {
    adjust();
    init_string_buffer();
    STRING_START = charPos - 1;
    BEGIN(STRING_STATE);
}

  /* Comments. */
"/*" {
     adjust();
     commentNesting += 1;
     BEGIN(COMMENT_STATE);
   }

"*/" {
     adjust();
     EM_error(EM_tokPos, "Comment not open!");
     yyterminate();
   }

.	 {adjust(); EM_error(EM_tokPos,"illegal token");}

<STRING_STATE>{
    \" {
          adjust();
          // printf("%s\n",string_buffer);
          if(string_buffer[0]=='\0'){
            yylval.sval = "(null)";
          }else{
            yylval.sval = string_buffer;
          }

          EM_tokPos = STRING_START;

          BEGIN(INITIAL);

          return STRING;
       }

    \n {
         adjust();
         EM_error(EM_tokPos, "Unterminated string constant!");
         yyterminate();
       }

    \\n {adjust();append_char_to_stringbuffer('\n');}
    \\t {adjust();append_char_to_stringbuffer('\t');}
    \\r {adjust();append_char_to_stringbuffer('\r');}
    \\b {adjust();append_char_to_stringbuffer('\b');}
    \\f {adjust();append_char_to_stringbuffer('\f');}

    "\\\"" {adjust();append_char_to_stringbuffer('"');}
    "\\'" {adjust();append_char_to_stringbuffer('\'');}
    "\\/" {adjust();append_char_to_stringbuffer('/');}

    "\\\\" {adjust();append_char_to_stringbuffer('\\');}

    <<EOF>> {
              EM_error(EM_tokPos, "Encounter EOF.");
              yyterminate();
            }

    . {
      adjust();
      char *yptr = yytext;
      while (*yptr) {
        append_char_to_stringbuffer(*yptr++);
      }
    }
}


<COMMENT_STATE>{
    "/*" {
           adjust();
           commentNesting += 1;
           continue;
         }

    "*/" {
           adjust();
           commentNesting -= 1;
           if (commentNesting == 0) {
             BEGIN(INITIAL);
           }
         }

    <<EOF>> {
              EM_error(EM_tokPos, "Encounter EOF.");
              yyterminate();
            }

    \n  {
      adjust();
      EM_newline();
      continue;
    }

    . {
      adjust();
      }

}
