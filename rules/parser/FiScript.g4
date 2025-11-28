grammar FiScript;

script   : (reacton | schedule) EOF ;
schedule   : SCHEDULE argumentList ;
reacton   : REACTON argumentList ;


argumentList : '(' argumentComposition ')' ;
argumentComposition : ARGUMENT (COMMA ARGUMENT)*;


WHITESPACE : [ \t\r\n]+ -> skip ;
EVENT   : [A-Z]+ ;
SCHEDULE : 'Schedule' ;
REACTON : 'ReactOn' ;
ARGUMENT    : ([a-zA-Z] | [0-9])+ ;
COMMA   : ',' ;
