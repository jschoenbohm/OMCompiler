// This file defines templates for transforming Modelica/MetaModelica code to C
// code. They are used in the code generator phase of the compiler to write
// target code.
//
// There are two root templates intended to be called from the code generator:
// translateModel and translateFunctions. These templates do not return any
// result but instead write the result to files. All other templates return
// text and are used by the root templates (most of them indirectly).
//
// To future maintainers of this file:
//
// - A line like this
//     # var = ""
//   declares a text buffer that you can later append text to. It can also be
//   passed to other templates that in turn can append text to it. In the new
//   version of Susan it should be written like this instead:
//     let &var = buffer ""
//
// - A line like this
//     ..., Text var, ...
//   declares that a template takes a tmext buffer as input parameter. In the
//   new version of Susan it should be written like this instead:
//     ..., Text &var, ...
//
// - A line like this:
//     ..., var, ...
//   passes a text buffer to a template. In the new version of Susan it should
//   be written like this instead:
//     ..., &var, ...
//
// - Style guidelines:
//
//   - Try (hard) to limit each row to 80 characters
//
//   - Code for a template should be indented with 2 spaces
//
//     - Exception to this rule is if you have only a single case, then that
//       single case can be written using no indentation
//
//       This single case can be seen as a clarification of the input to the
//       template
//
//   - Code after a case should be indented with 2 spaces if not written on the
//     same line

package CodegenQSS

import interface SimCodeTV;
import CodegenUtil.*;
import CodegenCFunctions.*;

template translateModel(SimCode simCode)
 "Generates C code and Makefile for compiling and running a simulation of a
  Modelica model using the QSS solver suite."
::=
match simCode
case SIMCODE(modelInfo=modelInfo as MODELINFO(__)) then
  let()= textFile(generateSourcefile(simCode, getName(modelInfo)), '<% getName(modelInfo) %>.c')
  let()= textFile(generateMakefile(simCode, getName(modelInfo)), '<% getName(modelInfo) %>.makefile')
  ""
end translateModel;

template getName(ModelInfo modelInfo)
 "Returns the name of the model"
::=
match modelInfo
case MODELINFO(__) then
  '<%dotPath(name) %>'
end getName;

template stateVars(SimCode simCode, String name)
 "Dummy Funktion"
 ::=
match simCode
	case SIMCODE(modelInfo=MODELINFO(varInfo=VARINFO(__))) then
//	match modelInfo
//		case MODELINFO(varInfo=VARINFO(__)) then

			'<% modelInfo.varInfo.numStateVars %>'



//	end match
end match
end stateVars;

template doSomething(SimEqSystem eq) ::=
let &preExp = buffer ""
let &varDecls = buffer ""
let &auxFunction = buffer ""
match eq
	case SES_SIMPLE_ASSIGN(__) then
	let expPart = daeExp(exp, contextOther, &preExp, &varDecls, &auxFunction)

	<<
		<% modelicaLine(eqInfo(eq)) %>
		<%cref(cref)%>=<%expPart%>;
		<%endModelicaLine() %>
	>>
end doSomething;

template genNames(list<SimEqSystem> eqs) ::=
	<< genNames...
		<%eqs |> eq => doSomething(eq) %>
		...Nothing
	>>
end genNames;



template test(list<SimEqSystem> eqs)
 "Dummy Funktion"
 ::=

<<
	// dumpEqs(e)
	<% eqs |> e =>
	match e
		case SES_SIMPLE_ASSIGN(__) then
		'Hallo 1
			<%match cref
				case CREF_IDENT(__) then
					'Hallo 2
					(<%ident%>)'
				case CREF_QUAL(__) then
					'Hallo 3
					(<%ident%>)
					<%match componentRef
						case CREF_IDENT(__) then
							'Hallo 2.2
							(<%ident%>)'
						case CREF_QUAL(__) then
							'Hallo 3.2
							(<%ident%>)'
					end match%>
					'
			end match%>
		'
	end match
	%>
>>
end test;



template generateSourcefile(SimCode simCode, String name)
 "Generates a QSS Solver model for simulation ."
::=
let &lvar = buffer ""
<<
<% sourceHead() %>

void MOD_definition(int i, double *x, double *d, double t, double *dx)
{
	switch(i)
	{
		<% MOD_definition(simCode) %>
		AnzStateVars: <% stateVars(simCode, "numStateVars") %>
		<% &lvar %>
		<% match simCode
			case SIMCODE(__) then
				'<%genNames(allEquations)%>'
			end match
		%>
	}
}

void MOD_dependencies(int i, double *x, double *d, double t, double *der)
{
	switch(i)
	{
		<% MOD_dependencies(simCode) %>
		<% match simCode
			case SIMCODE(__) then
				'<%test(allEquations)%>'
			end match
		%>
	}
}

void MOD_zeroCrossing(int i, double *x, double *d, double t, double *zc)
{
	switch(i)
	{
		<% MOD_zeroCrossing(simCode) %>
	}
}

void MOD_handlerPos(int i, double *x, double *d, double t)
{
	switch(i)
	{
		<% MOD_handlerPos(simCode) %>
	}
}

void MOD_handlerNeg(int i, double *x, double *d, double t)
{
	switch(i)
	{
		<% MOD_handlerNeg(simCode) %>
	}
}

void MOD_output(int i, double *x, double *d, double t, double *out)
{
	switch(i)
	{
		<% MOD_output(simCode) %>
	}
}

void QSS_initializeDataStructs(QSS_simulator simulator)
{
	simulator->model = QSS_Model(MOD_definition,MOD_dependencies,MOD_zeroCrossing,MOD_handlerPos,MOD_handlerNeg);
}

>>
end generateSourcefile;


template MOD_definition(SimCode simCode)
""
::=
<<
	//MOD_definition
>>
end MOD_definition;

template MOD_dependencies(SimCode simCode)
""
::=
<<
	//MOD_dependencies
>>
end MOD_dependencies;

template MOD_zeroCrossing(SimCode simCode)
""
::=
<<
	//MOD_zeroCrossing
>>
end MOD_zeroCrossing;

template MOD_handlerPos(SimCode simCode)
""
::=
<<
	//MOD_handlerPos
>>
end MOD_handlerPos;

template MOD_handlerNeg(SimCode simCode)
""
::=
<<
	//MOD_handlerNeg
>>
end MOD_handlerNeg;

template MOD_output(SimCode simCode)
""
::=
<<
	//MOD_output
>>
end MOD_output;

template sourceHead()
"All include files"
::=
<<
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <math.h>
	#include <common/utils.h>
	#include <qss/qss_model.h>
>>
end sourceHead;

template generateMakefile(SimCode simCode, String name)
 "Generates the makefile. "
::=
<<
LDFLAGS=-LC:/temp/QssSolver/src/libs
LIBRARIES=-lqssh
OUT_SRC = C:/temp/newQSS/build/bball_downstairs/<%name%>.c
OUT = C:/temp/newQSS/build/bball_downstairs/bball_downstairs
INCLUDES = -IC:/temp/QssSolver/src/engine -IC:/temp/QssSolver/usr/include

CFLAGS= -Wall -msse2 -mfpmath=sse -O2 $(LDFLAGS) $(LIBRARIES)
# Compiler.
CC = gcc
default: $(OUT)
$(OUT):
	$(CC) $(INCLUDES) $(OUT_SRC) $(CFLAGS) -o $@ -lm -lgsl -lconfig -lgslcblas
.PHONY: clean
clean:
	rm -f $(OUT) *.dat *.log
>>
end generateMakefile;

annotation(__OpenModelica_Interface="backend");
end CodegenQSS;
