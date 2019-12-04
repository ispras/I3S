/*
 * This file is used to test the conversion from I3S to C.
 *
 * Written by Oleg Goremykin <goremykin@ispras.ru>
*/

#ifndef INCLUDE_I3S_C
#define INCLUDE_I3S_C

// test include
#include <stdio.h>

extern tcg tcg_array[5];

// test extern
extern tcg my_extern;

/* test struct decl */
struct struct_A {
    /* first field */
    int a;
    unsigned long int b;
    /* end */
};

/* test typedef */
typedef int test_typedef;

// test define
#define N 5

/* test enum decl */
enum test_enum {
    One, //first elem
    Two,
    /* three = 3 */
    Three = 3,
    End
    /* end */
};

int test_decl(short tcg param) {
    //const int
    const signed int mysadf;

    tcg array_test[2][5][7];
    tcg array_test2[12];

    int a_init[2] = {1, 2};
    // declare TCGv
    tcg tcg_tl_1 = param, tcg_tl_2 = N;
    tcg a1, a2 = 5, a3;
    tcg tcg_tl, **pointer;
    TCGv tcg_v;
    TCGv_i32 tcgv_i32;
    TCGv_i64 tcgv_i64;
    // signed TCGv
    signed tcg tcg_tl_signed = -21;
    /* declare TCGv_i32 */;
    short tcg tcg_i32_1, tcg_i32_2 = -N;
    // declare TCGv_i64
    long tcg tcg_i64_1, tcg_i64_2 = param;

    struct A struct_a;
    int b;
    int  q, w, e, * const qwe;
}


int test_expr(void) {
    // declare TCGv
    tcg tcg_tl_1, tcg_tl_2;
    // signed TCGv
    signed tcg tcg_tl_signed;
    /* declare TCGv_i32 */;
    short tcg tcg_i32_1, tcg_i32_2;
    // declare TCGv_i64
    long tcg tcg_i64_1, tcg_i64_2;

    const int c_var_1, c_var_2;

    //test cast
    tcg_tl_1 = tcg_i32_1;
    tcg_tl_1 = tcg_i64_1;
    tcg_i64_1 = tcg_i32_1;
    tcg_i32_1 = tcg_i64_1;
    tcg_i64_1 = tcg_tl_1;
    tcg_i32_1 = tcg_tl_1;

    /*
     * test expr
     */

    // +
    tcg_tl_1 = c_var_1 + c_var_2;
    tcg_tl_1 = tcg_tl_1 + c_var_2;
    tcg_tl_1 = tcg_tl_1 + tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 + tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 + tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 + tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed + tcg_tl_1;
    tcg_tl_1 += tcg_tl_1;
    // -
    tcg_tl_1 = tcg_tl_1 - c_var_2;
    tcg_tl_1 = tcg_tl_1 - tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 - tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 - tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 - tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed - tcg_tl_1;
    tcg_tl_1 -= tcg_tl_1;
    // *
    tcg_tl_1 = tcg_tl_1 * c_var_2;
    tcg_tl_1 = tcg_tl_1 * tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 * tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 * tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 * tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed * tcg_tl_1;
    tcg_tl_1 *= tcg_tl_1;
    // /
    tcg_tl_1 = tcg_tl_1 / c_var_2;
    tcg_tl_1 = tcg_tl_1 / tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 / tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 / tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 / tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed / tcg_tl_1;
    tcg_tl_1 /= tcg_tl_1;
    // %
    tcg_tl_1 = tcg_tl_1 % c_var_2;
    tcg_tl_1 = tcg_tl_1 % tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 % tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 % tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 % tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed % tcg_tl_1;
    tcg_tl_1 %= tcg_tl_1;
    // >>
    tcg_tl_1 = tcg_tl_1 >> c_var_2;
    tcg_tl_1 = tcg_tl_1 >> tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 >> tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 >> tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 >> tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed >> tcg_tl_1;
    tcg_tl_1 >>= tcg_tl_1;
    // <<
    tcg_tl_1 = tcg_tl_1 << c_var_2;
    tcg_tl_1 = tcg_tl_1 << tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 << tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 << tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 << tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed << tcg_tl_1;
    tcg_tl_1 <<= tcg_tl_1;
    // >>>
    tcg_tl_1 = tcg_tl_1 >>> c_var_2;
    tcg_tl_1 = tcg_tl_1 >>> tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 >>> tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 >>> tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 >>> tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed >>> tcg_tl_1;
    tcg_tl_1 >>>= tcg_tl_1;
    // <<<
    tcg_tl_1 = tcg_tl_1 <<< c_var_2;
    tcg_tl_1 = tcg_tl_1 <<< tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 <<< tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 <<< tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 <<< tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed <<< tcg_tl_1;
    tcg_tl_1 <<<= tcg_tl_1;
    // ^
    tcg_tl_1 = tcg_tl_1 ^ c_var_2;
    tcg_tl_1 = tcg_tl_1 ^ tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 ^ tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 ^ tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 ^ tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed ^ tcg_tl_1;
    tcg_tl_1 ^= tcg_tl_1;
    // |
    tcg_tl_1 = tcg_tl_1 | c_var_2;
    tcg_tl_1 = tcg_tl_1 | tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 | tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 | tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 | tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed | tcg_tl_1;
    tcg_tl_1 |= tcg_tl_1;
    // &
    tcg_tl_1 = tcg_tl_1 & c_var_2;
    tcg_tl_1 = tcg_tl_1 & tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 & tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 & tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 & tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed & tcg_tl_1;
    tcg_tl_1 &= tcg_tl_1;
    // &&
    tcg_tl_1 = tcg_tl_1 && c_var_2;
    tcg_tl_1 = tcg_tl_1 && tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 && tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 && tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 && tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed && tcg_tl_1;
    // ||
    tcg_tl_1 = tcg_tl_1 || c_var_2;
    tcg_tl_1 = tcg_tl_1 || tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 || tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 || tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 || tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed || tcg_tl_1;
    // >, <, >=, <=, ==, !=
    tcg_tl_1 = tcg_tl_1 > c_var_2;
    tcg_tl_1 = tcg_tl_1 < tcg_i64_2;
    tcg_tl_1 = tcg_i32_1 >= tcg_i64_2;
    tcg_tl_1 = tcg_tl_1 <= tcg_tl_2;
    tcg_tl_1 = tcg_tl_1 == tcg_tl_signed;
    tcg_tl_1 = tcg_tl_signed != tcg_tl_1;
    // ++v
    ++tcg_i64_1;
    ++c_var_1;
    c_var_1++;
    // v++
    tcg_i32_1++;
    tcg_tl_1++;
    ++tcg_tl_signed;
    // --
    c_var_1--;
    --c_var_1;
    tcg_i32_1--;
    --tcg_i64_1;
    tcg_tl_1--;
    --tcg_tl_signed;
    // unary op
    tcg_tl_1 = +tcg_tl_1;
    tcg_tl_1 = -tcg_i64_2;
    tcg_tl_1 = !tcg_i32_1;
    tcg_tl_1 = ~tcg_tl_signed;


    /* ********* */
    unsigned int int_array[5];

    tcg_array[1] = (tcg_tl_signed * 5 + (tcg_array[5+4] * tcg_i64_2
            * (tcg_array * tcg_array)
            + (tcg_tl_1 + tcg_i64_1 * tcg_i32_1)) + (int_array * 11 + 4));
    c_var_1 = (1 + c_var_1) * 5 + int_array[test1 + 5];

    tcg_tl_1 = (tcg_tl_signed * tcg_tl_1 / (3 + tcg_tl_1)) +
            (tcg_tl_1 * tcg_tl_signed / (c_var_1 / tcg_i32_1));

    tcg_tl_1 = (long tcg)(tcg_i32_1);

    tcg_tl_1 = (tcg)(tcg_i32_1 + tcg_i32_2);

    tcg_tl_1 = (tcg) tcg_i64_1;

    // tcg_tl_1++
    tcg_tl_1[1]++;
    // ++tcg_tl_1[2]
    ++tcg_tl_1[2];
    tcg_tl_1[6][7] = tcg_tl_2[5][6][7];
    // tcg_tl_1 = ++tcg_tl_1 + tcg_tl_1++
    tcg_tl_1 = ++tcg_tl_1 + tcg_tl_1++;
    a = 2;
    tcg_tl_1 = ++(c_var_1 + *c_var_1) + tcg_tl_1;
    c_var_1 = 3;
    tcg_tl_1 = -tcg_tl_1;
    tcg_tl_1 = ~(tcg_tl_1 + tcg_i32_1);
    tcg_tl_1 = ~(-tcg_tl_1 + tcg_i64_1) / tcg_tl_2;
    c_var_1++;
    ++c_var_1;
    c_var_1 = c_var_1++;
    c_var_1 = (long int) (c_var_1 + c_var_2);
    c_var_1 = (long int) c_var_1;
    c_var_1 = !b;
    c_var_1 = ~b;
    c_var_1 = -c_var_1;
    c_var_1 = + c_var_1;
    int shift = sizeof(int) * 8 - sizeof(c_var_1);
}

int if_test(void) {
    tcg tcg_tl;
    int c_var;

    if (c_var == 0)
        if (c_var != 0) {
            tcg tcg_tl_1;
            tcg_tl_1 = tcg_tl_1 + 1;
        } else
            c_var = 5;

    // if (tcg_tl > 0)
    if (tcg_tl > 0) { /* open curly braces */
        tcg tcg_tl_2;
        // if (start < 0)
        if (tcg_tl < 0) {
            tcg tcg_tl_3;
            tcg_tl_2 = 00;
            tcg_tl_3 = UNKNOWN_MACRO + N;
            // wrog return;
            return N;
        // end if (start < 0)
        }
        if (c_var == 0) { /* open curly braces */
            tcg_tl_2 = 0;
        }
    // end if (tcg_tl > 0)
    }

    // if (start < 0)
    if (tcg_tl < 0) {
        tcg tcg_tl_4;
        tcg_tl_4 = tcg_tl;
    } else {
        // else
        tcg tcg_else;
        tcg_else = 2;
        //else end
    }

    //if (start == 0)
    if (tcg_tl) {
        tcg tcg_in_if;
        tcg_tl = tcg_in_if;
    } else if (tcg_tl == 777) {
        // else if (tcg_tl == 777)
        tcg tcg_test;
        tcg_test = tcg_tl;
        test_decl(tcg_tl + tcg_tl);
    } else {
        // else
        tcg_tl = 5;
    }

    signed long tcg boris;

    if (boris == boris + 5) {
        if (boris != boris + 5)
            // if (boris != 0)
            boris = (boris + 5) * 16;
        else if (boris >= boris + 5)
            // else if (boris >= boris + 5)
            boris = (boris + 5) * 16;
        else
            // if (boris != 0) else
            boris = (boris + 5) * 16;
    } else if (boris > boris + 5)
        // else if (boris > 6)
        boris = (boris + 5) * 16;

    c_var = N;
}

int switch_test(void) {
    tcg x;

    //switch(x+x) {
    switch(x+x) {
    //case 10:
    case 10: //case 10:
    {
         tcg z;
         if (z != 0) {
             z = z * 8 + z;
         } else {
             z = z * 8;
             //break;
             break;
         }
    }
    case 22:
    //case6
    case 6:
        x = x * 8 + x;
    //case7
    case 7:
    {
        tcg chech_local_free;
        while_test(chech_local_free);
        tcg y = 0;
        x = x * 8 + y;
        break;
    }
    default:
        //default
        x = x * 8 + x;
        printf("No option selected");
    }


    int a;
    tcg b;
    display_errors(a);
    switch(a) {
    case 10:
        x = x * 8 + x;
        break;
    case 9:
        break;
    case 8:
        x = x + 1;
        break;
    case 11:
    {
        tcg b1;
        tcg chech_local_free;
        while_test(chech_local_free);
        x = x * 8 + b1;
        break;
    }
    case 5:
    case 6:
        printf("WARNING");
    case 7:
        display_errors(a);
        printf("WARNING");
        test_decl(a + 1);
    default  :
        printf("No option selected");
        //test_decl(b + b);
        test_decl(b + b);
        //a = test_decl(a + b);
        a = test_decl(a + b);
        //b += test_decl(b + test_decl(b + a));
        b += test_decl(b + test_decl(b + a));
        //a = 1 + test_decl(a + b);
        a = 1 + test_decl(a + b);
        //b = 1 + test_decl(a + b);
        b = 1 + test_decl(a + b);
    }
}

int do_while_test(int m, const unsigned int check_const) {
    tcg start;
    int a;

    if (a) {
        a = 5;
        //end
    }

    start = start + 5 + start;

    // do_while start
    do {
        if (a) {
            a = 555;
            // return
            start = start + 5 + start;
            a = a + 5;
            //return;
            return;
        }
        do {
            a = 6;
            start = start * 6 + start;
            break;
            continue;
        } while (a);
        // start = start * 6 + start;
        start = start * 6 + start;

        do {
            // start = start * 7 + start;
            start = start * 7 + start;
            // if (start) {
            if (start) {
                start = 0;
                //continue;
                continue;
            }
        } while (start < start + 6);
        // b = start * 6 + start;
        tcg b;
        b = start * 6 + start;
        if (a == 6) {
            // return
            return;
        }
        //end
    } while (start > start + 5);

    if (a) {
        a = 5;
        return;
    //end
    }
}


int while_test(tcg var) {

    tcg test_while;
    int a;

    if (a) {
        return 5;
        // under return
    }

    start = start + 5 + start;

    // while (var > test_while + 5)
    while (var > test_while + 5) {
        if (a) {
            a = 555;
            //end
        }
        while (a) {
            a = 6;
            break;
            continue;
        }
        //  test_while = var * 6 + var
        test_while = var * 6 + var;
        a = 7;
        // while (test_while > 5 * var)
        while (test_while > 5 * var) {
            // test_while = test_while * N + var
            test_while = test_while * N + var;
            // if (start) {
            if (test_while) {
                start = 0;
            }
            //continue;
            continue;
            while (var) {
                var++;
                //end
            }
        }
        // start += N
        start += N;
        //end
    }
}

int test_duplicate_vars(void) {
    const int test[2];

    if(test){
        tcg test;
        if(test) {
            tcg test;
            test = test + test;
            if(test){
                tcg test;
                test = test + 1;
                while(test < test){
                    ++test;
                }
            }
            test_decl(test);
            test = test + test;
        }
        for(tcg test; test < test; test++) {
            tcg test;
            test++;
        }
        test = test + test;
    }

    tcg test_33;
    switch(test_33){
    case 3: {
        tcg i;
        test_decl(i);
    }
    case 4: {
        tcg i;
    }
    default: {
        tcg i;
        ++i;
    }
    }

    tcg i;
}

int for_test(void) {

    tcg stop;

    if (a) {
        a = 5;
        //end
    }

    stop %= stop + 5 + stop;

    // for (tcg start; start > start && stop; start++)
    for (signed long tcg start = 0; start > (start && stop); start++) {
        if (a) {
            a = 555;
            //end
        }
        for (;;) {
            a = 6;
            break;
            continue;
            return (a * 7);
        } while (a);
        // start = start * 6 + start;
        start = start * 6 + start;
        a = 7;
        //break;
        break;

        start = stop * 6 / stop;
        for(int a = 6; a < 7; a++) {
            start = 7 * start + start;
            //return
            return start;
        }
        // for(;start > start + 5;)
        for(;start > start + 5;) {
            // start = start * stop;
            start = start * stop;
            // if (start) {
            if (start) {
                start = 0;
                //continue;
                continue;
            }
        }
        // start = start * 6 + start; second
        start = start * 6 + start;
        tcg chech_local_free;
        while_test(chech_local_free);
        // end for
    }

    stop = END_MACRO;
}

/* test func call */

int helper(short tcg a1, tcg a2) {}

int func_call_test(void) {
    int a = 5;
    tcg b = 6;

    // helper(a + 1, b + b);
    helper(a + 1, b + b);

    // a = helper(a + 1, b + b);
    a = helper(a + 1, b + b);

    if(helper(a, b) > 1) {
        helper(a + 1, b + b);
        a = helper(a, b);
    } else {
        // helper(a, a);
        helper(a, a);
        // a = helper(a, a);
        a = helper(a, a);
        //a = helper(a, b) + helper(a, a);
        a = helper(a, b) + helper(a, a);
        //a = helper(a, b) + helper(a, b);
        a = helper(a, b) + helper(a, b);
        //b = helper(a, a + b) + b;
        b = helper(a, a + b) + b;
    }

    // b = helper(a + 1, b + b);
    b = helper(a + 1, b + b);
    // helper(a + 1, helper(a + 1, b + b));
    helper(a + 1, helper(a + 1, b + b));
}

#endif /* INCLUDE_I3S_C */
