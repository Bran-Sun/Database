%{
	#include "pmysql.tab.h"
	#include <stdarg.h>
	#include <string.h>
	void yyerror(char *s, ...); 
	int oldstate;
%}

%%
/* keyword */
DATABASE 	{ return DATABASE; }
DATABASES 	{ return DATABASES; }
TABLE 		{ return TABLE; }
TABLES 		{ return TABLES; }
SHOW 		{ return SHOW; }
CREATE 		{ return CREATE; }
DROP		{ return DROP; }
USE			{ return USE; }
PRIMARY		{ return PRIMARY; }
KEY 		{ return KEY; }
NOT			{ return NOT; }
NULL 		{ return NULL; }
INSERT		{ return INSERT; }
INTO		{ return INTO; }
VALUES		{ return VALUES; }
DELETE		{ return DELETE; }
FROM		{ return FROM; }
WHERE		{ return WHERE; }
UPDATE		{ return UPDATE; }
SET 		{ return SET; }
SELECT 		{ return SELECT; }
IS 			{ return IS; }
INT 		{ return INT; }
VARCHAR 	{ return VARCHAR; }
DESC 		{ return DESC; }
REFERENCES	{ return REFERENCES; }
INDEX 		{ return INDEX; }
AND 		{ return AND; }
DATE        { return DATE; }
FLOAT 		{ return FLOAT; }
FOREIGN 	{ return FOREIGN; }

/* numbers */
VALUE_INT: [0-9]+ 	{ yylval.intval = atoi(yytext); return INTNUM; }
-?[0-9]+"."[0-9]* |
-?"."[0-9]+ |
-?[0-9]+E[-+]?[0-9]+ |
-?[0-9]+"."[0-9]*E[-+]?[0-9]+ |
-?"."[0-9]+E[-+]?[0-9]+ 	{ yylval.floatval = atof(yytext) ; return APPROXNUM; }

IDENTIFIER: [A-Za-z][_0-9A-Za-z]*
VALUE_STRING: ’[^’]*’

%%

int main(int argc, char **argv) {
	yylex();
	printf("%8d%8d%8d\n", lines, words, chars);
}