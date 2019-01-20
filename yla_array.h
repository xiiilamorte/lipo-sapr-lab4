#ifndef _yla_array_h
#define _yla_array_h

#include "yla_cop.h"
#include "yla_type.h"
#include "yla_test_gencode.h"
#include "yla_compliance_table.h"

void put_array_concatenation(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[]);
void put_array_insert_left(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[]);
void put_array_insert_right(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter);

#endif