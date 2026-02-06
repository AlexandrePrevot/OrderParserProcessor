grammar FiScript;

script   : statement* EOF ;

statement
    : schedule
    | reacton
    | print
    | alert
    | if
    | variableDeclaration
    | variableAssignment
    ;

schedule   : SCHEDULE argumentList block NEWLINE* ;
reacton   : REACTON argumentList block NEWLINE* ;
print   : PRINT '(' expression ')' NEWLINE* ;
alert   : ALERT '(' expression ')' NEWLINE* ;
if   : 'if' '(' expression ')' block ('else' 'if' '(' expression ')' block)* ('else' block)? NEWLINE* ;

variableDeclaration : IDENTIFIER '=' expression NEWLINE ;
variableAssignment : IDENTIFIER COMPOUND_OP expression NEWLINE ;

expression
    : '(' expression ')'                          # ParenExpression
    | expression ('*' | '/') expression           # MulDiv
    | expression ('+' | '-') expression           # AddSub
    | expression ('>' | '<' | '>=' | '<=' | '==' | '!=') expression # Comparison
    | expression ('and' | 'or') expression        # LogicalOp
    | BOOL_LITERAL                                # BooleanLiteral
    | IDENTIFIER                                  # VariableRef
    | NUMBER                                      # NumericLiteral
    | STRING_LITERAL                              # StringLiteral
    ;

// Block rule: newlines are optional inside {} blocks.
// but not possible to add newlines before {
// known and intended yet (can be changed in the future)
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
ALERT : 'Alert' ;
BOOL_LITERAL : 'True' | 'False' ;
COMPOUND_OP : '+=' | '-=' | '*=' | '/=' ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
ARGUMENT_TOKEN : DIGIT+ [a-zA-Z_] [a-zA-Z0-9_]* ;
NUMBER : DIGIT+ ('.' DIGIT+)? ;
STRING_LITERAL : '"' (~['"\\] | '\\' .)* '"' ;
COMMA   : ',' ;
NEWLINE : [\r\n]+ ;
WHITESPACE : [ \t]+ -> skip ;
