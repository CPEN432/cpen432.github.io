/*******************************************************************************
 *
 * Chronos: A Timing Analyzer for Embedded Software
 * =============================================================================
 * http://www.comp.nus.edu.sg/~rpembed/chronos/
 *
 * Copyright (C) 2005 Xianfeng Li
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
 * $Id: isa.h,v 1.2 2006/06/24 08:54:56 lixianfe Exp $
 *
 ******************************************************************************/

#ifndef ISA_H
#define ISA_H


// instruction types broadly in three groups:
// computation,
// memory access
// control flow transfer
enum inst_type_t {
    INST_NOP = 0,   // instr. doing nothing
    // (1) computation
    INST_ICOMP,	    // integer arithmetic instr.
    INST_FCOMP,	    // floating-point arithmetic instr.
    // (2) memory access
    INST_LOAD,
    INST_STORE,
    // (3) control flow transfer
    INST_COND,
    INST_UNCOND,
    INST_CALL,
    INST_RET,
    // (4) trap instr such as syscall, break, etc
    INST_TRAP
};



// each instruction type has the following fields useful for analysis
typedef struct {
    int	    opcode;	// inst opcode
    int	    type;	// inst type
    char    *name;	// inst name
} isa_t;



typedef unsigned int	addr_t;

// decoded instruction type
typedef struct {
    addr_t  addr;
    int	    op_enum;	    // continuous numbered opcode (orginal non-contenuous)
    int	    size;

    int	    num_in, num_out;	// number of input/output operands
    int	    *in, *out;		// input/output operands (registers)
    addr_t  target;		// target addr for control transfer inst
} de_inst_t;



#endif
