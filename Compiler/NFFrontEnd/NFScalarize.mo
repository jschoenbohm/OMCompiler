/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Linköping University,
 * Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linköping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

encapsulated package NFScalarize

import NFFlatten.Elements;
import NFFlatten.FunctionTree;

protected
import ExecStat.execStat;
import ComponentRef = NFComponentRef;
import Type = NFType;
import Expression = NFExpression;
import Binding = NFBinding;
import Equation = NFEquation;
import ExpressionIterator = NFExpressionIterator;
import Dimension = NFDimension;
import MetaModelica.Dangerous.listReverseInPlace;

public
function scalarize
  input output Elements elements;
  input String name;
protected
  list<tuple<ComponentRef, Binding>> comps = {};
  list<Equation> eql = {}, ieql = {};
algorithm
  for c in elements.components loop
    comps := scalarizeComponent(c, comps);
  end for;

  for eq in elements.equations loop
    eql := scalarizeEquation(eq, eql);
  end for;

  for eq in elements.initialEquations loop
    ieql := scalarizeEquation(eq, ieql);
  end for;

  elements.components := listReverseInPlace(comps);
  elements.equations := listReverseInPlace(eql);
  elements.initialEquations := listReverseInPlace(ieql);

  execStat(getInstanceName() + "(" + name + ")");
end scalarize;

protected
function scalarizeComponent
  input tuple<ComponentRef, Binding> component;
  input output list<tuple<ComponentRef, Binding>> components;
protected
  ComponentRef cref;
  Binding binding;
  Type ty;
  ExpressionIterator binding_iter;
  list<ComponentRef> crefs;
  Expression exp;
algorithm
  (cref, binding) := component;
  ty := ComponentRef.getType(cref);

  if Type.isArray(ty) then
    crefs := ComponentRef.scalarize(cref);

    if Binding.isBound(binding) and not Binding.isEach(binding) then
      binding_iter := ExpressionIterator.fromExpOpt(Binding.typedExp(binding));

      for cr in crefs loop
        (binding_iter, exp) := ExpressionIterator.next(binding_iter);
        components := (cr, Binding.FLAT_BINDING(exp)) :: components;
      end for;
    else
      for cr in crefs loop
        components := (cr, binding) :: components;
      end for;
    end if;
  else
    components := component :: components;
  end if;
end scalarizeComponent;

function scalarizeEquations
  input list<Equation> eql;
  output list<Equation> equations = {};
algorithm
  for eq in eql loop
    equations := scalarizeEquation(eq, equations);
  end for;

  equations := listReverseInPlace(equations);
end scalarizeEquations;

function scalarizeEquation
  input Equation eq;
  input output list<Equation> equations;
algorithm
  equations := match eq
    local
      ExpressionIterator lhs_iter, rhs_iter;
      Expression lhs, rhs;
      Type ty;
      SourceInfo info;
      list<Equation> eql;

    case Equation.EQUALITY(ty = ty, info = info) guard Type.isArray(ty)
      algorithm
        lhs_iter := ExpressionIterator.fromExp(eq.lhs);
        rhs_iter := ExpressionIterator.fromExp(eq.rhs);
        ty := Type.arrayElementType(ty);

        while ExpressionIterator.hasNext(lhs_iter) loop
          if not ExpressionIterator.hasNext(rhs_iter) then
            Error.addInternalError(getInstanceName() + " could not expand rhs " +
              Expression.toString(eq.rhs), eq.info);
          end if;

          (lhs_iter, lhs) := ExpressionIterator.next(lhs_iter);
          (rhs_iter, rhs) := ExpressionIterator.next(rhs_iter);
          equations := Equation.EQUALITY(lhs, rhs, ty, info) :: equations;
        end while;
      then
        equations;

    case Equation.ARRAY_EQUALITY()
      algorithm
        rhs := Expression.expand(eq.rhs);
      then
        Equation.ARRAY_EQUALITY(eq.lhs, rhs, eq.ty, eq.info) :: equations;

    case Equation.CONNECT() then equations;

    case Equation.IF()
      then scalarizeIfEquation(eq.branches, eq.info, equations);

    case Equation.WHEN()
      then Equation.WHEN(list(scalarizeBranch(b) for b in eq.branches), eq.info) :: equations;

    else eq :: equations;
  end match;
end scalarizeEquation;

function scalarizeIfEquation
  input list<tuple<Expression, list<Equation>>> branches;
  input SourceInfo info;
  input output list<Equation> equations;
protected
  list<tuple<Expression, list<Equation>>> bl = {};
  Expression cond;
  list<Equation> eql;
algorithm
  for b in branches loop
    (cond, eql) := b;
    eql := scalarizeEquations(eql);

    // Remove branches with no equations after scalarization.
    if not listEmpty(eql) then
      bl := (cond, eql) :: bl;
    end if;
  end for;

  // Add the scalarized if equation to the list of equations unless we don't
  // have any branches left.
  if not listEmpty(bl) then
    equations := Equation.IF(listReverseInPlace(bl), info) :: equations;
  end if;
end scalarizeIfEquation;

function scalarizeBranch
  input output tuple<Expression, list<Equation>> branch;
protected
  Expression exp;
  list<Equation> eql;
algorithm
  (exp, eql) := branch;
  branch := (exp, scalarizeEquations(eql));
end scalarizeBranch;

annotation(__OpenModelica_Interface="frontend");
end NFScalarize;