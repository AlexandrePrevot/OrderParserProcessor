grammar FiScript;

script   : statement* EOF ;

statement
    : schedule
    | reacton
    | print
    ;

schedule   : SCHEDULE argumentList block ;
reacton   : REACTON argumentList block ;
print   : PRINT argumentList;

block
    : '{' statement* '}'
    ;


argumentList : '(' argumentComposition ')' ;
argumentComposition : ARGUMENT (COMMA ARGUMENT)*;


fragment WORD : [a-zA-Z] ;
fragment NUMBER : [0-9] ;
fragment WORD_OR_NBR : (WORD | NUMBER)+ ;
fragment STRING : '"' (~['"'])* '"' ;


WHITESPACE : [ \t\r\n]+ -> skip ;
SCHEDULE : 'Schedule' ;
REACTON : 'ReactOn' ;
PRINT : 'Print' ;
ARGUMENT    : (STRING | WORD_OR_NBR) ;
COMMA   : ',' ;
