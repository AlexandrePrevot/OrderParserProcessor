grammar FiScript;

first   : react EOF ;
react   : EVENT ARGUMENT;

ARGUMENT : '(' COUNT ')';

EVENT   : [A-Z]+ ;
COUNT   : [0-9]+ | 'inf' ;