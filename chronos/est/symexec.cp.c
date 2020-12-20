/*******************************************************************************
 *
 * Chronos: A Timing Analyzer for Embedded Software
 * =============================================================================
 * http://www.comp.nus.edu.sg/~rpembed/chronos/
 *
 * Infeasible path analysis for Chronos
 * Vivy Suhendra
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * 03/2007 symexec.c
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg.h"
#include "infeasible.h"



int addAssign( char deritree[], cfg_node_t *bb, int lineno, int rhs, char rhs_var ) {

  int i;
  assign_t *assg;
  inf_node_t *ib;

  ib = &(inf_procs[bb->proc->id].inf_cfg[bb->id]);

  // check if there is previous assignment to the same memory address in the same block
  for( i = 0; i < ib->num_assign; i++ ) {
    assg = ib->assignlist[i];

    if( streq( assg->deritree, deritree )) {
      // overwrite this assignment
      assg->lineno  = lineno;
      assg->rhs     = rhs;
      assg->rhs_var = rhs_var;

      //if( DEBUG_INFEAS ) { printf( "OVR " ); printAssign( assg, 0 ); }
      return 1;
    }
  }

  // else add new
  assg = (assign_t*) malloc( sizeof(assign_t) );
  strcpy( assg->deritree, deritree );
  assg->rhs           = rhs;
  assg->rhs_var       = rhs_var;
  assg->bb            = bb;
  assg->lineno        = lineno;

  ib->num_assign++;

  i = ib->num_assign;
  ib->assignlist = (assign_t**) realloc( ib->assignlist, i * sizeof(assign_t*) );
  ib->assignlist[i-1] = assg;

  //if( DEBUG_INFEAS ) { printf( "NEW " ); printAssign( assg, 0 ); }
  return 0;
}

int addBranch( char deritree[], cfg_node_t *bb, int rhs, char rhs_var, char jump_cond ) {

  int i;
  branch_t *br;

  int numbb = prog.procs[bb->proc->id].num_bb;

  br = (branch_t*) malloc( sizeof(branch_t) );
  strcpy( br->deritree, deritree );
  br->rhs              = rhs;
  br->rhs_var          = rhs_var;
  br->jump_cond        = jump_cond;
  br->bb               = bb;

  br->num_BA_conflicts = 0;
  br->BA_conflict_list = NULL;
  br->num_BB_conflicts = 0;
  br->BB_conflict_list = NULL;

  inf_procs[bb->proc->id].inf_cfg[bb->id].branch = br;

  //if( DEBUG_INFEAS ) { printBranch( br, 0 ); }
  return 0;
}


// Note: for commutative operations on mixed-type operands
// (one is a variable and the other is a constant value),
// the derivation string is stored in the format "<var>+<const>"

#define PROCESS_LOAD \
      regPos1 = findReg( insn->r1 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos3 ); } */ \
      if( strlen(regList[regPos3].deritree) > 0 ) \
	sprintf( tmp, "%d(%s)", atoi(insn->r2), regList[regPos3].deritree ); \
      else \
	sprintf( tmp, "%d(%s)", atoi(insn->r2), regList[regPos3].name ); \
      regval = 0; \
      isConstant = 0; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_LUI \
      regPos1 = findReg( insn->r1 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); } */ \
      regval = atoi(insn->r2) * 65536; \
      isConstant = 1; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_ADDI \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s+%d", regList[regPos2].deritree, atoi(insn->r3) ); \
	else \
	  sprintf( tmp, "%s+%d", regList[regPos2].name, atoi(insn->r3) ); \
      } \
      regval = regList[regPos2].value + atoi(insn->r3); \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_ORI \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s|%d", regList[regPos2].deritree, atoi(insn->r3) ); \
	else \
	  sprintf( tmp, "%s|%d", regList[regPos2].name, atoi(insn->r3) ); \
      } \
      regval = regList[regPos2].value | atoi(insn->r3); \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_XORI \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s^%d", regList[regPos2].deritree, atoi(insn->r3) ); \
	else \
	  sprintf( tmp, "%s^%d", regList[regPos2].name, atoi(insn->r3) ); \
      } \
      regval = regList[regPos2].value ^ atoi(insn->r3); \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_ANDI \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s&%d", regList[regPos2].deritree, atoi(insn->r3) ); \
	else \
	  sprintf( tmp, "%s&%d", regList[regPos2].name, atoi(insn->r3) ); \
      } \
      regval = regList[regPos2].value & atoi(insn->r3); \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_SLL \
      /* r1 = r2 * ( 2^r3 ) */ \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s<<%s", regList[regPos2].deritree, insn->r3 ); \
	else \
	  sprintf( tmp, "%s<<%s", regList[regPos2].name, insn->r3 ); \
      } \
      regval = regList[regPos2].value; \
      power = hexValue( insn->r3 ); \
      for( m = 0; m < power ; m++ ) \
	regval *= 2; \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_SRL \
      /* r1 = r2 / ( 2^r3 ) */ \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s>>%s", regList[regPos2].deritree, insn->r3 ); \
	else \
	  sprintf( tmp, "%s>>%s", regList[regPos2].name, insn->r3 ); \
      } \
      regval = regList[regPos2].value; \
      power = hexValue( insn->r3 ); \
      for( m = 0; m < power ; m++ ) \
	regval /= 2; \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_SLTI \
      /* set r1 to 1 if r2 < r3 (r3 constant) */ \
      /* r2 <  r3 --> r1 is 1 --> value >  0    */ \
      /* r2 >= r3 --> r1 is 0 --> value <= 0    */ \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  strcpy( tmp, regList[regPos2].deritree ); \
	else \
	  strcpy( tmp, regList[regPos2].name ); \
      } \
      else if( strlen(regList[regPos2].deritree) == 0 ) \
	strcpy( tmp, regList[regPos2].name ); \
      regval = atoi(insn->r3)/* - regList[regPos2].value*/; \
      isConstant = regList[regPos2].valid; \
      setReg( regPos1, tmp, regval, isConstant, SLTI ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_SLT \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); printReg( regPos3 ); } */ \
      if( !regList[regPos2].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  strcat( tmp, regList[regPos2].deritree ); \
	else \
	  strcat( tmp, regList[regPos2].name ); \
        if( !regList[regPos3].valid ) \
	  strcat( tmp, ":<" ); \
      } \
      if( !regList[regPos3].valid ) { \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  strcat( tmp, regList[regPos3].deritree ); \
	else \
	  strcat( tmp, regList[regPos3].name ); \
      } \
      regval = regList[regPos3].value/* - regList[regPos2].value*/; \
      isConstant = regList[regPos2].valid && regList[regPos3].valid; \
      if( regList[regPos3].valid ) \
	setReg( regPos1, tmp, regval, isConstant, SLT ); \
      else \
	setReg( regPos1, tmp, regval, isConstant, KO ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_ADDU \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); printReg( regPos3 ); } */ \
      if( !regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s+", regList[regPos2].deritree ); \
	else \
	  sprintf( tmp, "%s+", regList[regPos2].name ); \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  strcat( tmp, regList[regPos3].deritree ); \
	else \
	  strcat( tmp, regList[regPos3].name ); \
      } \
      else if( !regList[regPos2].valid && regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s+%d", regList[regPos2].deritree, regList[regPos3].value ); \
	else \
	  sprintf( tmp, "%s+%d", regList[regPos2].name, regList[regPos3].value ); \
      } \
      else if( regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  sprintf( tmp, "%s+%d", regList[regPos3].deritree, regList[regPos2].value ); \
	else \
	  sprintf( tmp, "%s+%d", regList[regPos3].name, regList[regPos2].value ); \
      } \
      regval = regList[regPos2].value + regList[regPos3].value; \
      isConstant = regList[regPos2].valid && regList[regPos3].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_SUBU \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); printReg( regPos3 ); } */ \
      if( !regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s-", regList[regPos2].deritree ); \
	else \
	  sprintf( tmp, "%s-", regList[regPos2].name ); \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  strcat( tmp, regList[regPos3].deritree ); \
	else \
	  strcat( tmp, regList[regPos3].name ); \
      } \
      else if( !regList[regPos2].valid && regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s-%d", regList[regPos2].deritree, regList[regPos3].value ); \
	else \
	  sprintf( tmp, "%s-%d", regList[regPos2].name, regList[regPos3].value ); \
      } \
      else if( regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  sprintf( tmp, "%d-%s", regList[regPos2].value, regList[regPos3].deritree ); \
	else \
	  sprintf( tmp, "%d-%s", regList[regPos2].value, regList[regPos3].name ); \
      } \
      regval = regList[regPos2].value - regList[regPos3].value; \
      isConstant = regList[regPos2].valid && regList[regPos3].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_OR \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); printReg( regPos3 ); } */ \
      if( !regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s|", regList[regPos2].deritree ); \
	else \
	  sprintf( tmp, "%s|", regList[regPos2].name ); \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  strcat( tmp, regList[regPos3].deritree ); \
	else \
	  strcat( tmp, regList[regPos3].name ); \
      } \
      else if( !regList[regPos2].valid && regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s|%d", regList[regPos2].deritree, regList[regPos3].value ); \
	else \
	  sprintf( tmp, "%s|%d", regList[regPos2].name, regList[regPos3].value ); \
      } \
      else if( regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  sprintf( tmp, "%s|%d", regList[regPos3].deritree, regList[regPos2].value ); \
	else \
	  sprintf( tmp, "%s|%d", regList[regPos3].name, regList[regPos2].value ); \
      } \
      regval = regList[regPos2].value | regList[regPos3].value; \
      isConstant = regList[regPos2].valid && regList[regPos3].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define PROCESS_XOR \
      regPos1 = findReg( insn->r1 ); \
      regPos2 = findReg( insn->r2 ); \
      regPos3 = findReg( insn->r3 ); \
      /* if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); printReg( regPos3 ); } */ \
      if( !regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s^", regList[regPos2].deritree ); \
	else \
	  sprintf( tmp, "%s^", regList[regPos2].name ); \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  strcat( tmp, regList[regPos3].deritree ); \
	else \
	  strcat( tmp, regList[regPos3].name ); \
      } \
      else if( !regList[regPos2].valid && regList[regPos3].valid ) { \
	if( strlen(regList[regPos2].deritree) > 0 ) \
	  sprintf( tmp, "%s^%d", regList[regPos2].deritree, regList[regPos3].value ); \
	else \
	  sprintf( tmp, "%s^%d", regList[regPos2].name, regList[regPos3].value ); \
      } \
      else if( regList[regPos2].valid && !regList[regPos3].valid ) { \
	if( strlen(regList[regPos3].deritree) > 0 ) \
	  sprintf( tmp, "%s^%d", regList[regPos3].deritree, regList[regPos2].value ); \
	else \
	  sprintf( tmp, "%s^%d", regList[regPos3].name, regList[regPos2].value ); \
      } \
      regval = regList[regPos2].value ^ regList[regPos3].value; \
      isConstant = regList[regPos2].valid && regList[regPos3].valid; \
      setReg( regPos1, tmp, regval, isConstant, NIL ); \
      /* if( DEBUG_INFEAS ) { printf( "==> " ); printReg( regPos1 ); } */

#define ADD_ASSIGN \
      if( !streq(insn->r1, "$31") && !streq(insn->r1, "$30") && \
	  !streq(insn->r1, "$29") && !streq(insn->r1, "$28") ) { \
        if( isConstant ) \
	  addAssign( insn->r1, ib->bb, k, regval, 0 ); \
        else \
	  addAssign( insn->r1, ib->bb, k, 0, 1 ); \
      }


/*
 * Go through the entire list of instructions and collect effects (assignments, branches).
 * Calculate and store register values (in the form of deritree) for each effect.
 */
int execute() {

  int i, j, k, m, id;
  int power;
  int direction; 

  int regPos1, regPos2, regPos3;
  int regval;
  char isConstant;

  int in_test;
  int newEffect = 1;

  char tmp[ DERI_LEN ];
  char offset[ DERI_LEN ];

  char jal, ignore;  // procedure call cancels conflict

  inf_proc_t *p;
  inf_node_t *ib;
  insn_t *insn;

  // preprocessing part: only apply register modifications, no need to check for effects
  for( k = 0; k < num_insn_st; k++ ) {
    insn = &(insnlist_st[k]);
    //if( DEBUG_INFEAS ) { printf( "\n[st,--,%2d] ", k ); printInstr( insn ); }

    tmp[0] = '\0';

    if( streq(insn->op, "lw" ) || streq(insn->op, "lb") || streq(insn->op, "lbu") ||
	streq(insn->op, "l.d") || streq(insn->op, "lh") )
    {
      PROCESS_LOAD;
    }
    else if( streq(insn->op, "lui") )
    {
      PROCESS_LUI;
    }
    else if( streq(insn->op, "addiu") || streq(insn->op, "addi") )
    {
      PROCESS_ADDI;
    }
    else if( streq(insn->op, "ori") )
    {
      PROCESS_ORI;
    }
    else if( streq(insn->op, "xori") )
    {
      PROCESS_XORI;
    }
    else if( streq(insn->op, "andi") )
    {
      PROCESS_ANDI;
    }
    else if( streq(insn->op, "sll") )
    {
      PROCESS_SLL;
    }
    else if( streq(insn->op, "srl") )
    {
      PROCESS_SRL;
    }
    else if( streq(insn->op, "slti") || streq(insn->op, "sltiu") )
    {
      PROCESS_SLTI;
    }
    else if( streq(insn->op, "slt") || streq(insn->op, "sltu") )
    {
      PROCESS_SLT;
    }
    else if( streq(insn->op, "addu") )
    {
      PROCESS_ADDU;
    }
    else if( streq(insn->op, "subu") )
    {
      PROCESS_SUBU;
    }
    else if( streq(insn->op, "or") )
    {
      PROCESS_OR;
    }
    else if( streq(insn->op, "xor") )
    {
      PROCESS_XOR;
    }
  }    
  // end preprocessing

  // start executing the program
  for( i = 0; i < prog.num_procs; i++ ) {
    p = &(inf_procs[i]);
    jal = 0;

    // clear registers (parameter passing not handled)
    clearReg();

    for( j = 0; j < p->num_bb; j++ ) {
      ib = &(p->inf_cfg[j]);
      
      for( k = 0; k < ib->num_insn; k++ ) {
	insn = &(ib->insnlist[k]);
	// if( DEBUG_INFEAS ) { printf( "\n[%2d,%2d,%2d] ", i, j, k ); printInstr( insn ); }

	ignore = jal;
	jal = 0;

	tmp[0] = '\0';

	// test previous effects on the current instruction
	if( streq(insn->op, "beq") || streq(insn->op, "bne") )
        {
	  if( ignore )
	    continue;

	  regPos1 = findReg( insn->r1 );
	  regPos2 = findReg( insn->r2 );
	  // if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos2 ); }

	  if(( regList[regPos1].flag == SLTI || regList[regPos1].flag == SLT ) &&
	     regList[regPos2].valid && regList[regPos2].value == 0 ) {

	    // r1's value is the result of slt/slti operation (0 or 1) compared against zero (r2)
	    if( streq(insn->op, "beq") )
	      // jump if r1 == 0, i.e. slt is false, i.e. x >= y
	      addBranch( regList[regPos1].deritree, ib->bb, regList[regPos1].value, 0, GE );
	    else
	      // jump if r1 != 0, i.e. slt is true, i.e. x < y
	      addBranch( regList[regPos1].deritree, ib->bb, regList[regPos1].value, 0, LT );
	  }
	  else if( regList[regPos1].flag == NIL && regList[regPos2].flag == NIL ) {
	    
	    // r1 and r2 are constants or results of some calculation

	    // skip if both are memory accesses or both are constants
	    // ( we only handle conflicts of type var-rel-const )
	    if( regList[regPos1].valid && regList[regPos2].valid )
	      continue;

	    if( !regList[regPos1].valid && !regList[regPos2].valid )
	      continue;
	    
	    if( !regList[regPos1].valid && regList[regPos2].valid ) {
	      // r1 expr, r2 const
	      in_test = regPos1;
	      regval = regList[regPos2].value - regList[regPos1].value;
	    }
	    else if( regList[regPos1].valid && !regList[regPos2].valid ) {
	      // r1 const, r2 expr
	      in_test = regPos2;
	      regval = regList[regPos1].value - regList[regPos2].value;
	    }
	    if( streq(insn->op, "bne") )
	      addBranch( regList[in_test].deritree, ib->bb, regval, 0, NE );
	    else
	      addBranch( regList[in_test].deritree, ib->bb, regval, 0, EQ );
	  }			
	}  		
	else if( streq(insn->op, "bgez") || streq(insn->op, "blez") || streq(insn->op, "bgtz") )
	{
	  if( ignore )
	    continue;

	  // only one register used, its value compared against zero
	  regPos1 = findReg( insn->r1 );
	  // if( DEBUG_INFEAS ) { printReg( regPos1 ); }

	  if( strlen(regList[regPos1].deritree) > 0 ) {
	    if( streq(insn->op, "bgez") )
	      addBranch( regList[regPos1].deritree, ib->bb, 0 - regList[regPos1].value, 0, GE );
	    else if( streq(insn->op, "bgtz") )
	      addBranch( regList[regPos1].deritree, ib->bb, 0 - regList[regPos1].value, 0, GT );
	    else
	      addBranch( regList[regPos1].deritree, ib->bb, 0 - regList[regPos1].value, 0, LE );
	  }
	}	

	// skip updates on ra[31], s8[30], sp[29], gp[28] (those are system processing)
	else if( streq(insn->r1, "$31") || streq(insn->r1, "$30") || streq(insn->r1, "$29") ||
		 streq(insn->r1, "$28") || streq(insn->r3, "$29") )
	  continue;

	// store operation (assignment)
	else if( streq(insn->op, "sw") || streq(insn->op, "sb" ) || streq(insn->op, "sh") || streq(insn->op, "sbu") )
	{
	  regPos1 = findReg( insn->r1 );
	  regPos3 = findReg( insn->r3 );
	  // if( DEBUG_INFEAS ) { printReg( regPos1 ); printReg( regPos3 ); }

	  // target memory address
	  if( strlen(regList[regPos3].deritree) > 0 )
	    sprintf( tmp, "%d(%s)", atoi(insn->r2), regList[regPos3].deritree );
	  else
	    sprintf( tmp, "%d(%s)", atoi(insn->r2), regList[regPos3].name );

	  if( ignore && streq(insn->r1, REG_RETURN) )
	    // is a return value from some function call
	    addAssign( tmp, ib->bb, k, 0, 1 );

	  else if( regList[regPos1].valid )
	    // a constant
	    addAssign( tmp, ib->bb, k, regList[regPos1].value, 0 );
	  else
	    addAssign( tmp, ib->bb, k, 0, 1 );
	}
	else if( streq(insn->op, "lw" ) || streq(insn->op, "lb") || streq(insn->op, "lbu") ||
		 streq(insn->op, "l.d") || streq(insn->op, "lh") )
	{
	  PROCESS_LOAD;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "lui") )
	{
	  PROCESS_LUI;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "addiu") || streq(insn->op, "addi") )
	{
	  PROCESS_ADDI;
	  ADD_ASSIGN;  // may be an assignment, associated with register instead of memory address
	}
	else if( streq(insn->op, "ori") )
	{
	  PROCESS_ORI;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "xori") )
	{
	  PROCESS_XORI;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "andi") )
	{
	  PROCESS_ANDI;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "sll") )
        {
	  PROCESS_SLL;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "srl") )
	{
	  PROCESS_SRL;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "slti") || streq(insn->op, "sltiu") )
	{
	  PROCESS_SLTI;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "slt") || streq(insn->op, "sltu") )
	{
	  PROCESS_SLT;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "addu") )
	{
	  PROCESS_ADDU;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "subu") )
	{
	  PROCESS_SUBU;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "or") )
	{
	  PROCESS_OR;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "xor") )
	{
	  PROCESS_XOR;
	  ADD_ASSIGN;  // may modify register value associated with assignment
	}
	else if( streq(insn->op, "jal") )
	  jal = 1;

	else if( streq(insn->op, "j") );

	else printf( "Ignoring opcode: %s\n", insn->op );

      } // end for instr

    } // end for bb

  } // end for procs

  return 1;	
}

