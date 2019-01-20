/*
	Virtual Machine code compliance table
*/

#include "yla_compliance_table.h"

void compliance_table_init(compliance_table* array, size_t size)
{
	array->size = size;
	array->count = 0;
	array->high_byte = calloc(array->size, sizeof(yla_cop_type));
	array->low_byte = calloc(array->size, sizeof(yla_cop_type));
	array->mark = calloc(array->size, sizeof(yla_cop_type));
}

yla_int_type compliance_table_get_addr(compliance_table* array, yla_int_type mark, size_t *addr)
{
	if (array->count == 0) {
		return -8;
	}

	yla_int_type i = 0;
	do
	{
		if (array->mark[i++] == mark) {
			*addr = (array->high_byte[i-1] << 8) + array->low_byte[i-1];
			return 1;
		}
	} while(i != array->count);
	
	return -9;
}

void compliance_table_set_addr(compliance_table* array, yla_int_type mark, size_t addr)
{
	addr++;	// vm->pc has numeration from 0
	array->high_byte[array->count] = addr >> 8;
	array->low_byte[array->count] = addr - (array->high_byte[array->count] << 8);
	array->mark[array->count] = mark;
	array->count++;
}

void compliance_table_done(compliance_table* array)
{
	array->size = 0;
	array->count = 0;
	free(array->high_byte);
	free(array->low_byte);
	free(array->mark);
}

/*	Link program by link table
	param prog_ptr*			Pointer to program
	param prog_counter		Pointer! to program counter
	param link_table*		Pointer to link table
*/
int link_program(yla_cop_type *prog_ptr, yla_int_type prog_counter, compliance_table *link_table)
{
	for (int i = 0; i < 12; i++) {
		yla_cop_type emptiness = (prog_ptr + i);	// skip header
		*(prog_ptr)++;
	}
	for (int i = 12; i < prog_counter + 12; i++) {
		if (i != (prog_counter + 12 - 1)) { 
			yla_cop_type value = *(prog_ptr);
			if ((value == CALO)		||
					(value == CJMP)	||
					(value == CJZ)		||
					(value == CJNZ)	||
					(value == CJE)		||
					(value == CJNE)
			) {
				yla_int_type op1 = *(prog_ptr + 1);
				yla_int_type op2 = *(prog_ptr + 2);
				
				yla_int_type mark = ((yla_int_type)op1 << 8) + (yla_int_type)op2;
				
				yla_int_type addr = 0;
				
				yla_int_type state = compliance_table_get_addr(link_table, mark, &addr);
				
				if (state == -8) {
					return state;
				} else if (state == 1) {
					yla_cop_type high_byte = addr >> 8;
					yla_cop_type low_byte = addr - (high_byte << 8);
					
					(prog_ptr) - 2;
					*(prog_ptr + 1) = high_byte;
					*(prog_ptr + 2) = low_byte;
					(prog_ptr) + 2;
				}
			}
			
			*(prog_ptr)++;
		} else { /* если осталось не меньше 3х элементов */
			break;	
		}
	}
	
	return 1;
}