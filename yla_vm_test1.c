/*
    Virtual machine Test #1

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

#include <stdio.h>
#include "yla_vm.h"
#include "yla_cop.h"
#include "yla_test.h"
#include "yla_test_gencode.h"
#include "yla_array.h"

static int test_gencode()
{
    yla_cop_type buf[100];
    yla_cop_type *ptr = buf;
    
    put_commd(&ptr, 0x12);
    put_value(&ptr, 0x3456);
    put_commd(&ptr, 0x78);
    put_commd(&ptr, 0x9a);

    YLATEST_ASSERT_TRUE(buf[0] == 0x12, "put_commd");
    YLATEST_ASSERT_TRUE(buf[1] == 0x34, "put_value");
    YLATEST_ASSERT_TRUE(buf[2] == 0x56, "put_value");
    YLATEST_ASSERT_TRUE(buf[3] == 0x78, "put_value");
    YLATEST_ASSERT_TRUE(buf[4] == 0x9a, "put_value");
    
    return 0;
}

static int test_init_null()
{
    yla_vm vm;

    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, NULL, 100), "init NULL");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init NULL");
    
    return 0;
}

static int test_init_0()
{
    yla_cop_type code[1];
    yla_vm vm;

    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, code, 0), "init 0 size code");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init 0 size code");

    return 0;
}

static int test_init_simple()
{
    yla_cop_type prg[HEADER_SIZE + 1];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 1);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 1), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple2()
{
    yla_cop_type prg[HEADER_SIZE + 2];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 2);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 2), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == 1, "OK expected")
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple_run()
{
    yla_cop_type prg[HEADER_SIZE + 2];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 2);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 2), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_push()
{
    yla_cop_type prg[HEADER_SIZE + 4];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 1, 0, 4);
    put_commd(&ptr, CPUSH);
    put_value(&ptr, 0x1234);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 4), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_get_stack_full()
{
    yla_cop_type prg[HEADER_SIZE + 4];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 4);
    put_commd(&ptr, CPUSH);
    put_value(&ptr, 0x1234);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 4), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_FULL, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_code_limit()
{
    yla_cop_type prg[HEADER_SIZE + 1];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 1);
    put_commd(&ptr, CNOP);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 1), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_CODE_SEG_EXCEED, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_array_concat()
{
   	yla_int_type stack_size = 10;
	yla_int_type vartable_size = 200;
	yla_int_type prg_size = 1024;
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
	
	// Array 1: {7,8,9}, count: 3
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0007);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0008);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0009);			prog_counter += 2;
	//	[count]
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0003);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Array 2: {3,4}, count: 2
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0003);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0004);			prog_counter += 2;
	//	[count]
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0002);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[3]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[3]);	prog_counter += 2;
	
	// Concatenation function calling
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0100);			prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);			prog_counter++;
	
	put_array_concatenation(&prog_ptr, 0x0100, &subprog_addr, &prog_counter, global_var);
	
	yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	YLATEST_ASSERT_TRUE(yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE)), "normal");
	
	if (link_state == 1) {
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	YLATEST_ASSERT_TRUE( ((vm.stack.ptr[0] == 7)
	    && (vm.stack.ptr[1] == 8)
	    && (vm.stack.ptr[2] == 9)
	    && (vm.stack.ptr[3] == 3)
	    && (vm.stack.ptr[4] == 4)
	    && (vm.stack.ptr[5] == 5) /*concatenated arrays count*/
	    ), "incorrect stack structure after concaten");
	
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_OK, "incorrect error code");
	
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_array_insert_left()
{
   	yla_int_type stack_size = 10;
	yla_int_type vartable_size = 200;
	yla_int_type prg_size = 1024;
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
	
	// Array 1: {7,8,9}, count: 3
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0007);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0008);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0009);			prog_counter += 2;
	//	[count]
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0003);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Value: 6
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0006);			prog_counter += 2;
	
	// Concatenation function calling
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0100);			prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);			prog_counter++;
	
	put_array_insert_left(&prog_ptr, 0x0100, &subprog_addr, &prog_counter, global_var);
	
	yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	YLATEST_ASSERT_TRUE(yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE)), "normal");
	
	if (link_state == 1) {
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	YLATEST_ASSERT_TRUE( ((vm.stack.ptr[0] == 6)
	    && (vm.stack.ptr[1] == 7)
	    && (vm.stack.ptr[2] == 8)
	    && (vm.stack.ptr[3] == 9)
	    && (vm.stack.ptr[4] == 4) /*concatenated arrays count*/
	    ), "incorrect stack structure after concaten");
	
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_OK, "incorrect error code");
	
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_array_insert_right()
{
   	yla_int_type stack_size = 10;
	yla_int_type vartable_size = 200;
	yla_int_type prg_size = 1024;
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
	
	// Array 1: {7,8,9}, count: 3
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0007);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0008);			prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0009);			prog_counter += 2;
	//	[count]
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x0003);			prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);			prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Value: 10
	put_commd(&prog_ptr, CPUSH);			prog_counter++;
	put_value(&prog_ptr, 0x000a);			prog_counter += 2;
	
	// Concatenation function calling
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0100);			prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);			prog_counter++;
	
	put_array_insert_right(&prog_ptr, 0x0100, &subprog_addr, &prog_counter);
	
	yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	YLATEST_ASSERT_TRUE(yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE)), "normal");
	
	if (link_state == 1) {
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	YLATEST_ASSERT_TRUE( ((vm.stack.ptr[0] == 7)
	    && (vm.stack.ptr[1] == 8)
	    && (vm.stack.ptr[2] == 9)
	    && (vm.stack.ptr[3] == 10)
	    && (vm.stack.ptr[4] == 4) /*concatenated arrays count*/
	    ), "incorrect stack structure after concaten");
	
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_OK, "incorrect error code");
	
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

YLATEST_BEGIN(yla_vm_test1)
  YLATEST_ADD_TEST_CASE(test_gencode)
  YLATEST_ADD_TEST_CASE(test_init_null)
  YLATEST_ADD_TEST_CASE(test_init_0)
  YLATEST_ADD_TEST_CASE(test_init_simple)
  YLATEST_ADD_TEST_CASE(test_init_simple2)
  YLATEST_ADD_TEST_CASE(test_init_simple_run)
  YLATEST_ADD_TEST_CASE(test_push)
  YLATEST_ADD_TEST_CASE(test_get_stack_full)
  YLATEST_ADD_TEST_CASE(test_code_limit)
  YLATEST_ADD_TEST_CASE(test_array_concat)
  YLATEST_ADD_TEST_CASE(test_array_insert_left)
  YLATEST_ADD_TEST_CASE(test_array_insert_right)
YLATEST_END
