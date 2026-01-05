grammar FiScript;

script   : statement* EOF ;

statement
    : schedule
    | reacton
    | print
    | variableDeclaration
    | variableAssignment
    ;

schedule   : SCHEDULE argumentList block NEWLINE* ;
reacton   : REACTON argumentList block NEWLINE* ;
print   : PRINT argumentList NEWLINE* ;

variableDeclaration : IDENTIFIER '=' expression NEWLINE ;
variableAssignment : IDENTIFIER COMPOUND_OP expression NEWLINE ;

expression
    : '(' expression ')'                          # ParenExpression
    | expression ('*' | '/') expression           # MulDiv
    | expression ('+' | '-') expression           # AddSub
    | IDENTIFIER                                  # VariableRef
    | NUMBER                                      # NumericLiteral
    | STRING_LITERAL                              # StringLiteral
    ;

block
    : '{' NEWLINE* statement* NEWLINE* '}'
    ;


argumentList : '(' argumentComposition ')' ;
argumentComposition : argument (COMMA argument)* ;
argument : IDENTIFIER | NUMBER | ARGUMENT_TOKEN | STRING_LITERAL ;


fragment WORD : [a-zA-Z] ;
fragment DIGIT : [0-9] ;


SCHEDULE : 'Schedule' ;
REACTON : 'ReactOn' ;
PRINT : 'Print' ;
COMPOUND_OP : '+=' | '-=' | '*=' | '/=' ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
ARGUMENT_TOKEN : DIGIT+ [a-zA-Z_] [a-zA-Z0-9_]* ;
NUMBER : DIGIT+ ('.' DIGIT+)? ;
STRING_LITERAL : '"' (~['"\\] | '\\' .)* '"' ;
COMMA   : ',' ;
NEWLINE : [\r\n]+ ;
WHITESPACE : [ \t]+ -> skip ;
