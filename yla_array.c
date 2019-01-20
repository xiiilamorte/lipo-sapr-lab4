/*  
	Array operations: concatenation, insert to left, insert to right. 
*/

#include "yla_array.h"

/*	Subprogram: array_concatenation 
	(my global alias of the subprogram in comments: concat)
	This program needs 2 global variables for keep iterator of 'while' cycle.
	Reductions: CX -- counter,
		DX -- second array count
*/
void put_array_concatenation(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[])
{
	yla_int_type prog_count = *prog_counter;
	
	yla_int_type cx = global_var[0];
	yla_int_type dx = global_var[1];
	
	yla_int_type second_array_count = 0x0025;
	yla_int_type arrays_count = 0x0026;
	yla_int_type ret = 0x0027;
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	
	// Local vars section
	// var ret = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 		    prog_count++;
	put_value(prog_ptr, ret);     	    	prog_count += 2;
	// var second_array_count = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, second_array_count);    prog_count += 2;
	// var count = { CGDUP second_array_count + second_array_count }
	put_commd(prog_ptr, CGDUP); 	    	prog_count++;
	put_value(prog_ptr, second_array_count);	prog_count += 2;
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, second_array_count);	prog_count += 2;
	put_commd(prog_ptr, CADD); 		    	prog_count++;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, arrays_count);		prog_count += 2;
	// var cx = second_array_count
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, second_array_count);	prog_count += 2;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	// var dx = 0
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, dx);				prog_count += 2;
	
	compliance_table_set_addr(compliance, 0x0290, prog_count);	/*concat_while_start(290)*/
	
	// Begin program
	// while (cx != dx)
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, dx);				prog_count += 2;
	put_commd(prog_ptr, CCMP);	 	    	prog_count++;
	
	// If they are equal -- end subprogram (NOT CONFUSE WITH "RET"!)
	put_commd(prog_ptr, CJNZ);	 	    	prog_count++;
	put_value(prog_ptr, 0x0299);			prog_count += 2;	/*concat_end(299)*/
	
	// Else { for while cycle
	
	// Clean comparation result
	put_commd(prog_ptr, CSTK);		    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// cx-- temporarily
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// CGDUP cx
	put_commd(prog_ptr, CGDUP); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// cx++ temporarily
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// CGDEEP cx
	put_commd(prog_ptr, CGDEEP); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// cx-- permanently
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// } for while cycle
	put_commd(prog_ptr, CJMP); 	    		prog_count++;
	put_value(prog_ptr, 0x0290);			prog_count += 2;	/*concat_while_start(290)*/
	
	compliance_table_set_addr(compliance, 0x0299, prog_count);	/*concat_end(299)*/
	
	// End subprogram
	/*	Load arrays count and push him in stack.
		Deep is needed because of subprogram 
		keep copy of last array value 
		in top of stack. */
	// Clean comparation result
	put_commd(prog_ptr, CSTK);		    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, arrays_count);		prog_count += 2;
	put_commd(prog_ptr, CDEEP); 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	/*	Ret subprogram */
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, ret);				prog_count += 2;
	put_commd(prog_ptr, CRET);	 	    	prog_count++;
	
	*prog_counter = prog_count;
}

/*	Subprogram: array_insert_left
	(my alias for main function: insert_left)
	The subprogram inserts value on begin of stack
	by fully reorganizing array structure 
	after insert on begin of stack.
*/
void put_array_insert_left(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[])
{
	yla_int_type prog_count = *prog_counter;
	
	yla_int_type ret = 0x0027;
	yla_int_type array_count = 0x0028;
	yla_int_type iterator = 0x0029;
	yla_int_type extra_values = 0x002a;
	yla_int_type tmp = 0x002b;
	yla_int_type value = 0x002c;
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	
	// Local vars section
	// var ret = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, ret);				prog_count += 2;
	// var value = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, value);				prog_count += 2;
	// var array_count = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	// var iterator = array_count
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	// var extra_values = 0
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	// var tmp = 0
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, tmp);				prog_count += 2;
	
	// Begin program
	
	// if (array_count == 0)
	put_commd(prog_ptr, CLOAD); 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	put_commd(prog_ptr, CPUSH); 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CCMP);			   	prog_count++;
	// {
	// If array count is equal to 0 -- then end subprogram
	put_commd(prog_ptr, CJNZ);	 	    	prog_count++;
	put_value(prog_ptr, 0x0249);			prog_count += 2;	/*end(249)*/
	// Clean comparation result
	put_commd(prog_ptr, CSTK);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// }
	// CGDUP --iterator:
	// 1. --iterator
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	// 2. CGDUP iterator
	put_commd(prog_ptr, CGDUP);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	
	// Load value
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, value);				prog_count += 2;
	
	// CGDEEP ++iterator:
	// 1. ++iterator
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	// 2. CGDEEP iterator
	put_commd(prog_ptr, CGDEEP);	 	   	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	
	// iterator--
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	
	/*while(241)*/
	compliance_table_set_addr(compliance, 0x0241, prog_count);	/*while(241)*/
	
	// while (iterator != 0)
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CCMP);	 	    	prog_count++;
	// {
	// If iterator is equal to 0 -- then end while cycle 
	put_commd(prog_ptr, CJNZ);	 	    	prog_count++;
	put_value(prog_ptr, 0x0242);			prog_count += 2;	/*while_end(242)*/
	
	// Clean comparation result
	put_commd(prog_ptr, CSTK);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// CGDUP iterator + extra_values:
	// 1. tmp += iterator + extra_values
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, tmp);				prog_count += 2;
	// 2. CGDUP tmp
	put_commd(prog_ptr, CGDUP);	 	    	prog_count++;
	put_value(prog_ptr, tmp);				prog_count += 2;
	
	// CDUP 1
	put_commd(prog_ptr, CDUP);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// CDEEP array_count
	put_commd(prog_ptr, CGDEEP);	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	
	// extra_values_array++
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	
	// iterator--
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, iterator);			prog_count += 2;
	
	// }
	put_commd(prog_ptr, CJMP);	 	    	prog_count++;
	put_value(prog_ptr, 0x0241);			prog_count += 2;
	
	/*while_end(242)*/
	compliance_table_set_addr(compliance, 0x0242, prog_count);	/*while_end(242)*/
	// Clean comparation result
	put_commd(prog_ptr, CSTK);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// CGDEEP extra_values--:
	// 1. extra_values--
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	// 2. CGDEEP extra_values
	put_commd(prog_ptr, CGDEEP);	 	    prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	
	// CGSTK extra_values
	put_commd(prog_ptr, CGSTK);	 	    	prog_count++;
	put_value(prog_ptr, extra_values);		prog_count += 2;
	
	// CLOAD array_count++:
	// 1. array_count++
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	// 2. CLOAD array_count
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	
	/*end(249)*/
	compliance_table_set_addr(compliance, 0x0249, prog_count);	/*end(249)*/
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, ret);				prog_count += 2;
	put_commd(prog_ptr, CRET);	 	    	prog_count++;
	
	*prog_counter = prog_count;
}

/*	Subprogram: array_insert_right 
	(my alias in main function: insert_right)
	The subprogram inserts value on end of array
	without full reorganization of array structure. 
*/
void put_array_insert_right(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter)
{
	yla_int_type prog_count = *prog_counter;
	
	yla_int_type ret = 0x0027;
	yla_int_type array_count = 0x0028;
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	
	// Local vars section
	// var ret = stack->ptr[top]
	put_commd(prog_ptr, CSAVE); 	    	prog_count++;
	put_value(prog_ptr, ret);				prog_count += 2;
	// var array_count = (CDUP 1) ++
	put_commd(prog_ptr, CDUP);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);	 	    	prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);	 	    	prog_count++;
	put_commd(prog_ptr, CSAVE);	 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	
	// Begin program
	// DUP previous array count
	put_commd(prog_ptr, CDEEP);	 	    	prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	// Load already incremented array count
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, array_count);		prog_count += 2;
	
	// End subprogram
	put_commd(prog_ptr, CLOAD);	 	    	prog_count++;
	put_value(prog_ptr, ret);				prog_count += 2;
	put_commd(prog_ptr, CRET);	 	    	prog_count++;
	
	*prog_counter = prog_count;
}