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
 * 03/2007 reg.c
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "infeasible.h"


int initRegSet() {

  strcpy( regList[0].name , "$0"  ); strcpy( regList[1].name , "$1"  ); 
  strcpy( regList[2].name , "$2"  ); strcpy( regList[3].name , "$3"  );	
  strcpy( regList[4].name , "$4"  ); strcpy( regList[5].name , "$5"  ); 
  strcpy( regList[6].name , "$6"  ); strcpy( regList[7].name , "$7"  );	
  strcpy( regList[8].name , "$8"  ); strcpy( regList[9].name , "$9"  ); 
  strcpy( regList[10].name, "$10" ); strcpy( regList[11].name, "$11" );	
  strcpy( regList[12].name, "$12" ); strcpy( regList[13].name, "$13" ); 
  strcpy( regList[14].name, "$14" ); strcpy( regList[15].name, "$15" );	
  strcpy( regList[16].name, "$16" ); strcpy( regList[17].name, "$17" ); 
  strcpy( regList[18].name, "$18" ); strcpy( regList[19].name, "$19" );	
  strcpy( regList[20].name, "$20" ); strcpy( regList[21].name, "$21" ); 
  strcpy( regList[22].name, "$22" ); strcpy( regList[23].name, "$23" );	
  strcpy( regList[24].name, "$24" ); strcpy( regList[25].name, "$25" ); 
  strcpy( regList[26].name, "$26" ); strcpy( regList[27].name, "$27" );	
  strcpy( regList[28].name, "$28" ); strcpy( regList[29].name, "$29" ); 
  strcpy( regList[30].name, "$30" ); strcpy( regList[31].name, "$31" );
  
  strcpy( regList[34].name, "$f0"  ); strcpy( regList[35].name, "$f1"  ); 
  strcpy( regList[36].name, "$f2"  ); strcpy( regList[37].name, "$f3"  );
  strcpy( regList[38].name, "$f4"  ); strcpy( regList[39].name, "$f5"  ); 
  strcpy( regList[40].name, "$f6"  ); strcpy( regList[41].name, "$f7"  );
  strcpy( regList[42].name, "$f8"  ); strcpy( regList[43].name, "$f9"  ); 
  strcpy( regList[44].name, "$f10" ); strcpy( regList[45].name, "$f11" );
  strcpy( regList[46].name, "$f12" ); strcpy( regList[47].name, "$f13" ); 
  strcpy( regList[48].name, "$f14" ); strcpy( regList[49].name, "$f15" );
  strcpy( regList[50].name, "$f16" ); strcpy( regList[51].name, "$f17" ); 
  strcpy( regList[52].name, "$f18" ); strcpy( regList[53].name, "$f19" );
  strcpy( regList[54].name, "$f20" ); strcpy( regList[55].name, "$f21" ); 
  strcpy( regList[56].name, "$f22" ); strcpy( regList[57].name, "$f23" );
  strcpy( regList[58].name, "$f24" ); strcpy( regList[59].name, "$f25" ); 
  strcpy( regList[60].name, "$f26" ); strcpy( regList[61].name, "$f27" );
  strcpy( regList[62].name, "$f28" ); strcpy( regList[63].name, "$f29" ); 
  strcpy( regList[64].name, "$f30" ); strcpy( regList[65].name, "$f31" );
  
  strcpy( regList[66].name, "$fcc" );
  
  regList[0].deritree[0] = '\0';
  regList[0].value = 0;
  regList[0].valid = 1;
  regList[0].flag = NIL;
  
  strcpy( regList[28].deritree, "$28" );
  regList[28].value = 0;
  regList[28].valid = 0;
  regList[28].flag = NIL;

  strcpy( regList[29].deritree, "$29" );
  regList[28].value = 0;
  regList[28].valid = 0;
  regList[28].flag = NIL;
  
  strcpy( regList[30].deritree, "$30" );
  regList[30].value = 0;
  regList[30].valid = 0;
  regList[30].flag = NIL;

  return 0;
}


int clearReg() {

  int i;
  for( i = 1; i < NO_REG; i++ ) {
   
    if( i == 28 || i == 29 || i == 30 )
      continue;

    regList[i].deritree[0] = '\0';
    regList[i].value = 0;
    regList[i].valid = 0;
    regList[i].flag = NIL;
  }
  return 0;
}


int findReg( char key[] ) {

  int i;
  for( i = 0; i < NO_REG; i++ ) { 
    if( streq( key, regList[i].name ))
      return i; 
  }
  return -1;
}


int hexValue( char *hexStr ) {

  int val;
  sscanf( hexStr, "%x", &val );
  return val;
}


int setReg( int pos, char deritree[], int value, char valid, char flag ) {

  //printf( "SET %d %s %d %d %d\n", pos, deritree, value, valid, flag ); fflush( stdout );
  regList[pos].deritree[0] = '\0';  // reset
  strcpy( regList[pos].deritree, deritree );
  regList[pos].value = value;
  regList[pos].valid = valid;
  regList[pos].flag  = flag;

  return 0;
}


char regOp( insn_t *insn, int *val, char *deri ) {

  if( streq(insn->op, "addiu") ) {}
  if( streq(insn->op, "addu" ) ) {}
  if( streq(insn->op, "add.d") ) {}
  if( streq(insn->op, "add.s") ) {}
  if( streq(insn->op, "subu" ) ) {}
  if( streq(insn->op, "sub.d") ) {}
  if( streq(insn->op, "sub.s") ) {}
  if( streq(insn->op, "div"  ) ) {}
  if( streq(insn->op, "divu" ) ) {}
  if( streq(insn->op, "mult" ) ) {}
  if( streq(insn->op, "mul.d") ) {}

  if( streq(insn->op, "and" ) ) {}
  if( streq(insn->op, "andi") ) {}
  if( streq(insn->op, "nor" ) ) {}
  if( streq(insn->op, "or"  ) ) {}
  if( streq(insn->op, "ori" ) ) {}
  if( streq(insn->op, "xor" ) ) {}
  if( streq(insn->op, "xori") ) {}

  if( streq(insn->op, "beq" ) ) {}
  if( streq(insn->op, "bne" ) ) {}
  if( streq(insn->op, "bgez") ) {}
  if( streq(insn->op, "bgtz") ) {}
  if( streq(insn->op, "blez") ) {}
  if( streq(insn->op, "bltz") ) {}

  if( streq(insn->op, "mfhi") ) {}
  if( streq(insn->op, "mflo") ) {}

  if( streq(insn->op, "sll" ) ) {}
  if( streq(insn->op, "sllv") ) {}
  if( streq(insn->op, "sra" ) ) {}
  if( streq(insn->op, "srav") ) {}
  if( streq(insn->op, "srl" ) ) {}
  if( streq(insn->op, "srlv") ) {}

  if( streq(insn->op, "slt"  ) ) {}
  if( streq(insn->op, "slti" ) ) {}
  if( streq(insn->op, "sltiu") ) {}
  if( streq(insn->op, "sltu" ) ) {}

  if( streq(insn->op, "lb" ) ) {}
  if( streq(insn->op, "lbu") ) {}
  if( streq(insn->op, "lhu") ) {}
  if( streq(insn->op, "lui") ) {}
  if( streq(insn->op, "lw" ) ) {}
  if( streq(insn->op, "l.d") ) {}
  if( streq(insn->op, "l.s") ) {}

  if( streq(insn->op, "sw" ) ) {}
  if( streq(insn->op, "sb" ) ) {}
  if( streq(insn->op, "sh" ) ) {}
  if( streq(insn->op, "s.s") ) {}

  if( streq(insn->op, "cvt.d.s") ) {}
  if( streq(insn->op, "cvt.s.d") ) {}

  if( streq(insn->op, "mtc1" ) ) {}
  if( streq(insn->op, "mov.d") ) {}

  return 0;
}
