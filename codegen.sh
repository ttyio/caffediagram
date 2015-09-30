#!/bin/tcsh
flex -oparser_l.cpp parser.l
bison -v -d -oparser_y.cpp parser.y
