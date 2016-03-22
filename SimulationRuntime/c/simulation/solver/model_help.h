/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2010, Linköpings University,
 * Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
 * LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
 * THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
 * PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linköpings University, either from the above address,
 * from the URL: http://www.ida.liu.se/projects/OpenModelica
 * and in the OpenModelica distribution.
 *
 * This program is distributed  WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */
#ifndef MODEL_HELP_H
#define MODEL_HELP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "simulation_data.h"

/* lochel: I guess this is used for discrete relations */
#define RELATION(res,exp1,exp2,index,op_w) \
{ \
  if(data->simulationInfo->initial) \
  { \
    res = ((op_w)((exp1),(exp2))); \
    data->simulationInfo->relations[index] = res; \
  } \
  else if(data->simulationInfo->discreteCall == 0 || data->simulationInfo->solveContinuous) \
  { \
    res = data->simulationInfo->relationsPre[index]; \
  } \
  else \
  { \
    res = ((op_w)((exp1),(exp2))); \
    data->simulationInfo->relations[index] = res; \
  } \
}

/* lochel: I guess this is used for continuous relations */
#define RELATIONHYSTERESIS(res,exp1,exp2,index,op_w) \
{ \
  if(data->simulationInfo->initial) \
  { \
    res = ((op_w)((exp1),(exp2))); \
    data->simulationInfo->relations[index] = res; \
  } \
  else if(data->simulationInfo->discreteCall == 0 || data->simulationInfo->solveContinuous) \
  { \
    res = data->simulationInfo->relationsPre[index]; \
  } \
  else \
  { \
    res = ((op_w##ZC)((exp1),(exp2),data->simulationInfo->storedRelations[index])); \
    data->simulationInfo->relations[index] = res; \
  } \
}

extern const size_t SIZERINGBUFFER;

void initializeDataStruc(DATA *data, threadData_t *threadData);

void deInitializeDataStruc(DATA *data);

void updateDiscreteSystem(DATA *data, threadData_t *threadData);

void saveZeroCrossings(DATA *data, threadData_t *threadData);

void copyStartValuestoInitValues(DATA *data);

/* functions that are only used in USE_DEBUG_OUTPUT mode */
#ifdef USE_DEBUG_OUTPUT
  void printAllVarsDebug(DATA *data, int ringSegment, int stream);
  void printRelationsDebug(DATA *data, int stream);
#else
  #define printAllVarsDebug(data, ringSegment, stream) {}
  #define printRelationsDebug(data, stream) {}
#endif

void printAllVars(DATA *data, int ringSegment, int stream);
void printRelations(DATA *data, int stream);
void printZeroCrossings(DATA *data, int stream);
void printParameters(DATA *data, int stream);
void printSparseStructure(DATA *data, int stream);

void overwriteOldSimulationData(DATA *data);
void copyRingBufferSimulationData(DATA *data, threadData_t *threadData, SIMULATION_DATA **destData, RINGBUFFER* destRing);

void restoreExtrapolationDataOld(DATA *data);

void setAllVarsToStart(DATA* data);
void setAllStartToVars(DATA* data);
void setAllParamsToStart(DATA *data);

void restoreOldValues(DATA *data);

void storePreValues(DATA *data);

void updateRelationsPre(DATA *data);

modelica_boolean checkRelations(DATA *data);

void printHysteresisRelations(DATA *data);
void activateHysteresis(DATA* data);
void storeRelations(DATA* data);
void setZCtol(double relativeTol);

double getNextSampleTimeFMU(DATA *data);

void storeOldValues(DATA *data);

modelica_integer _event_integer(modelica_real x, modelica_integer index, DATA *data);
modelica_real _event_floor(modelica_real x, modelica_integer index, DATA *data);
modelica_real _event_ceil(modelica_real x, modelica_integer index, DATA *data);
modelica_integer _event_div_integer(modelica_integer x1, modelica_integer x2, modelica_integer index, DATA *data, threadData_t *threadData);
modelica_real _event_div_real(modelica_real x1, modelica_real x2, modelica_integer index, DATA *data, threadData_t *threadData);

/* functions used for relation which
 * are not used as zero-crossings
 */
modelica_boolean Less(double a, double b);
modelica_boolean LessEq(double a, double b);
modelica_boolean Greater(double a, double b);
modelica_boolean GreaterEq(double a, double b);

/* functions used to evaluate relation in
 * zero-crossing with hysteresis effect
 */
modelica_boolean LessZC(double a, double b, modelica_boolean);
modelica_boolean LessEqZC(double a, double b, modelica_boolean);
modelica_boolean GreaterZC(double a, double b, modelica_boolean);
modelica_boolean GreaterEqZC(double a, double b, modelica_boolean);

extern int measure_time_flag;

void setContext(DATA* data, double* currentTime, int currentContext);
void increaseJacContext(DATA* data);
void unsetContext(DATA* data);

#ifdef __cplusplus
}
#endif

#endif
