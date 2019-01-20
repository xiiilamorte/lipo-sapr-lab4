/*
	Virtual Machine main

	This file is part of YLA VM (Yet another Language for Academic purpose: Virtual Machine).

	YLA VM is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Foobar is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "yla_vm.h"
#include <string.h>
#include <stdio.h>
#include "yla_cop.h"
#include "yla_type.h"
#include "yla_test.h"
#include "yla_test_gencode.c"
#include "yla_compliance_table.h"
#include "yla_array.h"

int main(int argc, int argv)
{
	yla_int_type stack_size = 30;
	yla_int_type vartable_size = 200;
	yla_int_type prg_size = 5440;
	yla_cop_type prg[HEADER_SIZE + prg_size];
	yla_cop_type *prog_ptr = prg;
	
	yla_int_type prog_counter = 0;
	yla_int_type *prog_counter_ptr = prog_counter;
	
	compliance_table subprog_addr;
	compliance_table_init(&subprog_addr, 40);
	
	// Global variables addresses
	yla_int_type global_var[4];
	global_var[0] = 0x0033;     // CX
	global_var[1] = 0x0034;     // DX
	global_var[2] = 0x0035;		// Array 1 count
	global_var[3] = 0x0036;		// Array 2 count
	
	put_header(&prog_ptr, stack_size, vartable_size, prg_size);
	
	// Array 1 : {170, 224}, count: 2
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x00aa);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x00e0);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;		// two elements in array
	put_value(&prog_ptr, 0x0002);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Array 2: {2048, 4047}, count: 2
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0800);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0fcf);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;		// two elements in array
	put_value(&prog_ptr, 0x0002);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[3]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[3]);	prog_counter += 2;
	
	// Concatenation function calling
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0200);			prog_counter += 2;
	
	// Value: 227
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x00e3);			prog_counter += 2;
	
	// Inserting on left place of array function calling
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0270);			prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);			prog_counter++;
	
	put_array_concatenation(&prog_ptr, 0x0200, &subprog_addr, &prog_counter, global_var);
	put_array_insert_left(&prog_ptr, 0x0270, &subprog_addr, &prog_counter, global_var);
	put_array_insert_right(&prog_ptr, 0x0290, &subprog_addr, &prog_counter);
	
	yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE));
	
	if (link_state == 1) {
		yla_vm_run(&vm);
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
	yla_vm_done(&vm);
}
