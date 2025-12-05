grammar FiScript;

script   : (reacton | schedule | print) EOF ;
schedule   : SCHEDULE argumentList ;
reacton   : REACTON argumentList ;
print   : PRINT argumentList ;


argumentList : '(' argumentComposition ')' ;
argumentComposition : ARGUMENT (COMMA ARGUMENT)*;


WHITESPACE : [ \t\r\n]+ -> skip ;
EVENT   : [A-Z]+ ;
SCHEDULE : 'Schedule' ;
REACTON : 'ReactOn' ;
PRINT : 'Print' ;
ARGUMENT    : ([a-zA-Z] | [0-9])+ ;
COMMA   : ',' ;
