/*
 * This file is used to test the conversion from I3S to C.
 *
 * Written by Oleg Goremykin <goremykin@ispras.ru>
*/

#ifndef INCLUDE_I3S_C
#define INCLUDE_I3S_C

// test include
#include <stdio.h>

extern TCGv tcg_array[5];

// test extern
extern TCGv my_extern;

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

int test_decl(TCGv_i32 param) {
    //const int
    const signed int mysadf;

    TCGv array_test[2][5][7];
    for (int i_0 = 0; i_0 < 2; ++i_0) {
        for (int i_1 = 0; i_1 < 5; ++i_1) {
            for (int i_2 = 0; i_2 < 7; ++i_2) {
                array_test[i_0][i_1][i_2]= tcg_temp_new();
            }
        }
    }
    TCGv array_test2[12];
    for (int i_0 = 0; i_0 < 12; ++i_0) {
        array_test2[i_0]= tcg_temp_new();
    }

    int a_init[2] = {1, 2};
    // declare TCGv
    TCGv tcg_tl_1 = tcg_temp_new();
    tcg_gen_extu_i32_tl(tcg_tl_1, param);
    TCGv tcg_tl_2 = tcg_temp_new();
    tcg_gen_movi_tl(tcg_tl_2, N);
    TCGv a1 = tcg_temp_new();
    TCGv a2 = tcg_temp_new();
    tcg_gen_movi_tl(a2, 5);
    TCGv a3 = tcg_temp_new();
    TCGv tcg_tl = tcg_temp_new();
    TCGv **pointer = tcg_temp_new();
    TCGv tcg_v;
    TCGv_i32 tcgv_i32;
    TCGv_i64 tcgv_i64;
    // signed TCGv
    TCGv tcg_tl_signed = tcg_temp_new();
    tcg_gen_movi_tl(tcg_tl_signed, -21);
    /* declare TCGv_i32 */;
    TCGv_i32 tcg_i32_1 = tcg_temp_new_i32();
    TCGv_i32 tcg_i32_2 = tcg_temp_new_i32();
    tcg_gen_movi_i32(tcg_i32_2, -N);
    // declare TCGv_i64
    TCGv_i64 tcg_i64_1 = tcg_temp_new_i64();
    TCGv_i64 tcg_i64_2 = tcg_temp_new_i64();
    tcg_gen_extu_i32_i64(tcg_i64_2, param);

    struct A struct_a;
    int b;
    int  q, w, e, * const qwe;
    tcg_temp_free(tcg_tl_1);
    tcg_temp_free(tcg_tl_2);
    tcg_temp_free(a1);
    tcg_temp_free(a2);
    tcg_temp_free(a3);
    tcg_temp_free(tcg_tl);
    tcg_temp_free(pointer);
    tcg_temp_free(tcg_tl_signed);
    tcg_temp_free_i32(tcg_i32_1);
    tcg_temp_free_i32(tcg_i32_2);
    tcg_temp_free_i64(tcg_i64_1);
    tcg_temp_free_i64(tcg_i64_2);
    for (int i_0 = 0; i_0 < 2; ++i_0) {
        for (int i_1 = 0; i_1 < 5; ++i_1) {
            for (int i_2 = 0; i_2 < 7; ++i_2) {
                tcg_temp_free_tl(array_test[i_0][i_1][i_2]);
            }
        }
    }
    for (int i_0 = 0; i_0 < 12; ++i_0) {
        tcg_temp_free_tl(array_test2[i_0]);
    }
}


int test_expr(void) {
    // declare TCGv
    TCGv tcg_tl_1 = tcg_temp_new();
    TCGv tcg_tl_2 = tcg_temp_new();
    // signed TCGv
    TCGv tcg_tl_signed = tcg_temp_new();
    /* declare TCGv_i32 */;
    TCGv_i32 tcg_i32_1 = tcg_temp_new_i32();
    TCGv_i32 tcg_i32_2 = tcg_temp_new_i32();
    // declare TCGv_i64
    TCGv_i64 tcg_i64_1 = tcg_temp_new_i64();
    TCGv_i64 tcg_i64_2 = tcg_temp_new_i64();

    const int c_var_1, c_var_2;

    //test cast
    /* src: tcg_tl_1 = tcg_i32_1 */
    tcg_gen_extu_i32_tl(tcg_tl_1, tcg_i32_1);
    /* src: tcg_tl_1 = tcg_i64_1 */
    tcg_gen_trunc_i64_tl(tcg_tl_1, tcg_i64_1);
    /* src: tcg_i64_1 = tcg_i32_1 */
    tcg_gen_extu_i32_i64(tcg_i64_1, tcg_i32_1);
    /* src: tcg_i32_1 = tcg_i64_1 */
    tcg_gen_trunc_i64_i32(tcg_i32_1, tcg_i64_1);
    /* src: tcg_i64_1 = tcg_tl_1 */
    tcg_gen_extu_tl_i64(tcg_i64_1, tcg_tl_1);
    /* src: tcg_i32_1 = tcg_tl_1 */
    tcg_gen_trunc_tl_i32(tcg_i32_1, tcg_tl_1);

    /*
     * test expr
     */

    // +
    /* src: tcg_tl_1 = c_var_1 + c_var_2 */
    tcg_gen_movi_tl(tcg_tl_1, c_var_1 + c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 + c_var_2 */
    tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 + tcg_tl_2 */
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 + tcg_i64_2 */
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 + tcg_i64_2 */
    TCGv_i64 i3s_t1_i64 = tcg_temp_new_i64();
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_add_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 + tcg_tl_signed */
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed + tcg_tl_1 */
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 += tcg_tl_1 */
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // -
    /* src: tcg_tl_1 = tcg_tl_1 - c_var_2 */
    tcg_gen_subi_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 - tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_sub_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 - tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_sub_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 - tcg_tl_2 */
    tcg_gen_sub_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 - tcg_tl_signed */
    tcg_gen_sub_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed - tcg_tl_1 */
    tcg_gen_sub_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 -= tcg_tl_1 */
    tcg_gen_sub_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // *
    /* src: tcg_tl_1 = tcg_tl_1 * c_var_2 */
    tcg_gen_muli_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 * tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_mul_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 * tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_mul_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 * tcg_tl_2 */
    tcg_gen_mul_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 * tcg_tl_signed */
    tcg_gen_mul_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed * tcg_tl_1 */
    tcg_gen_mul_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 *= tcg_tl_1 */
    tcg_gen_mul_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // /
    /* src: tcg_tl_1 = tcg_tl_1 / c_var_2 */
    tcg_gen_movi_tl(i3s_t0_tl, c_var_2);
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_tl_1 / tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 / tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_divu_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 / tcg_tl_2 */
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 / tcg_tl_signed */
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed / tcg_tl_1 */
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 /= tcg_tl_1 */
    tcg_gen_divu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // %
    /* src: tcg_tl_1 = tcg_tl_1 % c_var_2 */
    tcg_gen_movi_tl(i3s_t0_tl, c_var_2);
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_tl_1 % tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 % tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_remu_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 % tcg_tl_2 */
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 % tcg_tl_signed */
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed % tcg_tl_1 */
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 %= tcg_tl_1 */
    tcg_gen_remu_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // >>
    /* src: tcg_tl_1 = tcg_tl_1 >> c_var_2 */
    tcg_gen_shri_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 >> tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_shr_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 >> tcg_i64_2 */
    TCGv_i32 i3s_t2_i32 = tcg_temp_new_i32();
    tcg_gen_trunc_i64_i32(i3s_t2_i32, tcg_i64_2);
    tcg_gen_shr_i32(i3s_t2_i32, tcg_i32_1, i3s_t2_i32);
    tcg_gen_extu_i32_tl(tcg_tl_1, i3s_t2_i32);
    /* src: tcg_tl_1 = tcg_tl_1 >> tcg_tl_2 */
    tcg_gen_shr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 >> tcg_tl_signed */
    tcg_gen_shr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed >> tcg_tl_1 */
    tcg_gen_sar_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 >>= tcg_tl_1 */
    tcg_gen_shr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // <<
    /* src: tcg_tl_1 = tcg_tl_1 << c_var_2 */
    tcg_gen_shli_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 << tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_shl_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 << tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_shl_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 << tcg_tl_2 */
    tcg_gen_shl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 << tcg_tl_signed */
    tcg_gen_shl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed << tcg_tl_1 */
    tcg_gen_shl_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 <<= tcg_tl_1 */
    tcg_gen_shl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // >>>
    /* src: tcg_tl_1 = tcg_tl_1 >>> c_var_2 */
    tcg_gen_rotri_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 >>> tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_rotr_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 >>> tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_rotr_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 >>> tcg_tl_2 */
    tcg_gen_rotr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 >>> tcg_tl_signed */
    tcg_gen_rotr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed >>> tcg_tl_1 */
    tcg_gen_rotr_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 >>>= tcg_tl_1 */
    tcg_gen_rotr_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // <<<
    /* src: tcg_tl_1 = tcg_tl_1 <<< c_var_2 */
    tcg_gen_rotli_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 <<< tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_rotl_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 <<< tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_rotl_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 <<< tcg_tl_2 */
    tcg_gen_rotl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 <<< tcg_tl_signed */
    tcg_gen_rotl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed <<< tcg_tl_1 */
    tcg_gen_rotl_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 <<<= tcg_tl_1 */
    tcg_gen_rotl_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // ^
    /* src: tcg_tl_1 = tcg_tl_1 ^ c_var_2 */
    tcg_gen_xori_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 ^ tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_xor_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 ^ tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_xor_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 ^ tcg_tl_2 */
    tcg_gen_xor_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 ^ tcg_tl_signed */
    tcg_gen_xor_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed ^ tcg_tl_1 */
    tcg_gen_xor_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 ^= tcg_tl_1 */
    tcg_gen_xor_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // |
    /* src: tcg_tl_1 = tcg_tl_1 | c_var_2 */
    tcg_gen_ori_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 | tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 | tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_or_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 | tcg_tl_2 */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 | tcg_tl_signed */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed | tcg_tl_1 */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* comment before compound assignment */
    /* src: tcg_tl_1 |= tcg_tl_1 */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // &
    /* src: tcg_tl_1 = tcg_tl_1 & c_var_2 */
    tcg_gen_andi_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 & tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 & tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_and_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 & tcg_tl_2 */
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 & tcg_tl_signed */
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed & tcg_tl_1 */
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    /* src: tcg_tl_1 &= tcg_tl_1 */
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, tcg_tl_1);
    // &&
    /* src: tcg_tl_1 = tcg_tl_1 && c_var_2 */
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    tcg_gen_andi_tl(tcg_tl_1, tcg_tl_1, c_var_2 != 0);
    /* src: tcg_tl_1 = tcg_tl_1 && tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t3_tl, i3s_t0_tl, 0);
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, i3s_t3_tl);
    /* src: tcg_tl_1 = tcg_i32_1 && tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_setcondi_i64(TCG_COND_NE, i3s_t1_i64, i3s_t1_i64, 0);
    TCGv_i64 i3s_t4_i64 = tcg_temp_new_i64();
    tcg_gen_setcondi_i64(TCG_COND_NE, i3s_t4_i64, tcg_i64_2, 0);
    tcg_gen_and_i64(i3s_t1_i64, i3s_t1_i64, i3s_t4_i64);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 && tcg_tl_2 */
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t0_tl, tcg_tl_2, 0);
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_tl_1 && tcg_tl_signed */
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t0_tl, tcg_tl_signed, 0);
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_tl_signed && tcg_tl_1 */
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_signed, 0);
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t0_tl, tcg_tl_1, 0);
    tcg_gen_and_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    // ||
    /* src: tcg_tl_1 = tcg_tl_1 || c_var_2 */
    tcg_gen_ori_tl(tcg_tl_1, tcg_tl_1, c_var_2);
    tcg_gen_setcondi_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    /* src: tcg_tl_1 = tcg_tl_1 || tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    tcg_gen_setcond_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    /* src: tcg_tl_1 = tcg_i32_1 || tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_or_i64(i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_setcond_i64(TCG_COND_NE, i3s_t1_i64, i3s_t1_i64, 0);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 || tcg_tl_2 */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, tcg_tl_2);
    tcg_gen_setcond_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    /* src: tcg_tl_1 = tcg_tl_1 || tcg_tl_signed */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    tcg_gen_setcond_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    /* src: tcg_tl_1 = tcg_tl_signed || tcg_tl_1 */
    tcg_gen_or_tl(tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    tcg_gen_setcond_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_1, 0);
    // >, <, >=, <=, ==, !=
    /* src: tcg_tl_1 = tcg_tl_1 > c_var_2 */
    tcg_gen_setcondi_tl(TCG_COND_GTU, tcg_tl_1, tcg_tl_1, c_var_2);
    /* src: tcg_tl_1 = tcg_tl_1 < tcg_i64_2 */
    tcg_gen_trunc_i64_tl(i3s_t0_tl, tcg_i64_2);
    tcg_gen_setcond_tl(TCG_COND_LTU, tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = tcg_i32_1 >= tcg_i64_2 */
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_setcond_i64(TCG_COND_GEU, i3s_t1_i64, i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = tcg_tl_1 <= tcg_tl_2 */
    tcg_gen_setcond_tl(TCG_COND_LEU, tcg_tl_1, tcg_tl_1, tcg_tl_2);
    /* src: tcg_tl_1 = tcg_tl_1 == tcg_tl_signed */
    tcg_gen_setcond_tl(TCG_COND_EQ, tcg_tl_1, tcg_tl_1, tcg_tl_signed);
    /* src: tcg_tl_1 = tcg_tl_signed != tcg_tl_1 */
    tcg_gen_setcond_tl(TCG_COND_NE, tcg_tl_1, tcg_tl_signed, tcg_tl_1);
    // ++v
    /* src: ++tcg_i64_1 */
    tcg_gen_addi_i64(tcg_i64_1, tcg_i64_1, 1);
    ++c_var_1;
    c_var_1++;
    // v++
    /* src: tcg_i32_1++ */
    tcg_gen_mov_i32(i3s_t2_i32, tcg_i32_1);
    tcg_gen_addi_i32(tcg_i32_1, tcg_i32_1, 1);
    /* src: tcg_tl_1++ */
    tcg_gen_mov_tl(i3s_t0_tl, tcg_tl_1);
    tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, 1);
    /* src: ++tcg_tl_signed */
    tcg_gen_addi_tl(tcg_tl_signed, tcg_tl_signed, 1);
    // --
    c_var_1--;
    --c_var_1;
    TCGv_i32 i3s_t5_i32 = tcg_temp_new_i32();
    /* src: tcg_i32_1-- */
    tcg_gen_mov_i32(i3s_t5_i32, tcg_i32_1);
    tcg_gen_subi_i32(tcg_i32_1, tcg_i32_1, 1);
    /* src: --tcg_i64_1 */
    tcg_gen_subi_i64(tcg_i64_1, tcg_i64_1, 1);
    /* src: tcg_tl_1-- */
    tcg_gen_mov_tl(i3s_t3_tl, tcg_tl_1);
    tcg_gen_subi_tl(tcg_tl_1, tcg_tl_1, 1);
    /* src: --tcg_tl_signed */
    tcg_gen_subi_tl(tcg_tl_signed, tcg_tl_signed, 1);
    // unary op
    /* src: tcg_tl_1 = +tcg_tl_1 */
    tcg_gen_mov_tl(tcg_tl_1, tcg_tl_1);
    /* src: tcg_tl_1 = -tcg_i64_2 */
    tcg_gen_neg_i64(i3s_t1_i64, tcg_i64_2);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t1_i64);
    /* src: tcg_tl_1 = !tcg_i32_1 */
    TCGv_i32 i3s_t6_i32 = tcg_temp_new_i32();
    tcg_gen_setcondi_i32(TCG_COND_EQ, i3s_t6_i32, tcg_i32_1, 0);
    tcg_gen_extu_i32_tl(tcg_tl_1, i3s_t6_i32);
    /* src: tcg_tl_1 = ~tcg_tl_signed */
    tcg_gen_not_tl(tcg_tl_1, tcg_tl_signed);


    /* ********* */
    unsigned int int_array[5];

    /* src: tcg_array[1] = (tcg_tl_signed * 5 + (tcg_array[5+4] * tcg_i64_2
            * (tcg_array * tcg_array)
            + (tcg_tl_1 + tcg_i64_1 * tcg_i32_1)) + (int_array * 11 + 4)) */
    tcg_gen_muli_tl(i3s_t0_tl, tcg_tl_signed, 5);
    tcg_gen_trunc_i64_tl(i3s_t3_tl, tcg_i64_2);
    tcg_gen_mul_tl(i3s_t3_tl, tcg_array[5+4], i3s_t3_tl);
    TCGv i3s_t7_tl = tcg_temp_new();
    tcg_gen_mul_tl(i3s_t7_tl, tcg_array, tcg_array);
    tcg_gen_mul_tl(i3s_t3_tl, i3s_t3_tl, i3s_t7_tl);
    tcg_gen_extu_i32_i64(i3s_t1_i64, tcg_i32_1);
    tcg_gen_mul_i64(i3s_t1_i64, tcg_i64_1, i3s_t1_i64);
    tcg_gen_trunc_i64_tl(i3s_t7_tl, i3s_t1_i64);
    tcg_gen_add_tl(i3s_t7_tl, tcg_tl_1, i3s_t7_tl);
    tcg_gen_add_tl(i3s_t3_tl, i3s_t3_tl, i3s_t7_tl);
    tcg_gen_add_tl(i3s_t0_tl, i3s_t0_tl, i3s_t3_tl);
    tcg_gen_addi_tl(tcg_array[1], i3s_t0_tl, int_array * 11 + 4);
    c_var_1 = (1 + c_var_1) * 5 + int_array[test1 + 5];

    /* src: tcg_tl_1 = (tcg_tl_signed * tcg_tl_1 / (3 + tcg_tl_1)) +
            (tcg_tl_1 * tcg_tl_signed / (c_var_1 / tcg_i32_1)) */
    tcg_gen_mul_tl(i3s_t0_tl, tcg_tl_signed, tcg_tl_1);
    tcg_gen_addi_tl(i3s_t3_tl, tcg_tl_1, 3);
    tcg_gen_divu_tl(i3s_t0_tl, i3s_t0_tl, i3s_t3_tl);
    tcg_gen_mul_tl(i3s_t3_tl, tcg_tl_1, tcg_tl_signed);
    tcg_gen_movi_i32(i3s_t2_i32, c_var_1);
    tcg_gen_divu_i32(i3s_t2_i32, i3s_t2_i32, tcg_i32_1);
    tcg_gen_extu_i32_tl(i3s_t7_tl, i3s_t2_i32);
    tcg_gen_divu_tl(i3s_t3_tl, i3s_t3_tl, i3s_t7_tl);
    tcg_gen_add_tl(tcg_tl_1, i3s_t0_tl, i3s_t3_tl);

    /* src: tcg_tl_1 = (long tcg)(tcg_i32_1) */
    tcg_gen_extu_i32_i64(i3s_t4_i64, tcg_i32_1);
    tcg_gen_trunc_i64_tl(tcg_tl_1, i3s_t4_i64);

    /* src: tcg_tl_1 = (tcg)(tcg_i32_1 + tcg_i32_2) */
    TCGv_i32 i3s_t8_i32 = tcg_temp_new_i32();
    tcg_gen_add_i32(i3s_t8_i32, tcg_i32_1, tcg_i32_2);
    tcg_gen_extu_i32_tl(tcg_tl_1, i3s_t8_i32);

    /* src: tcg_tl_1 = (tcg) tcg_i64_1 */
    tcg_gen_trunc_i64_tl(tcg_tl_1, tcg_i64_1);

    // tcg_tl_1++
    /* src: tcg_tl_1[1]++ */
    tcg_gen_mov_tl(i3s_t0_tl, tcg_tl_1[1]);
    tcg_gen_addi_tl(tcg_tl_1[1], tcg_tl_1[1], 1);
    // ++tcg_tl_1[2]
    /* src: ++tcg_tl_1[2] */
    tcg_gen_addi_tl(tcg_tl_1[2], tcg_tl_1[2], 1);
    /* src: tcg_tl_1[6][7] = tcg_tl_2[5][6][7] */
    tcg_gen_mov_tl(tcg_tl_1[6][7], tcg_tl_2[5][6][7]);
    // tcg_tl_1 = ++tcg_tl_1 + tcg_tl_1++
    /* src: tcg_tl_1 = ++tcg_tl_1 + tcg_tl_1++ */
    tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, 1);
    tcg_gen_mov_tl(i3s_t0_tl, tcg_tl_1);
    tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, 1);
    tcg_gen_add_tl(tcg_tl_1, tcg_tl_1, i3s_t0_tl);
    a = 2;
    /* src: tcg_tl_1 = ++(c_var_1 + *c_var_1) + tcg_tl_1 */
    tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, ++(c_var_1 + *c_var_1));
    c_var_1 = 3;
    /* src: tcg_tl_1 = -tcg_tl_1 */
    tcg_gen_neg_tl(tcg_tl_1, tcg_tl_1);
    /* src: tcg_tl_1 = ~(tcg_tl_1 + tcg_i32_1) */
    tcg_gen_extu_i32_tl(i3s_t0_tl, tcg_i32_1);
    tcg_gen_add_tl(i3s_t0_tl, tcg_tl_1, i3s_t0_tl);
    tcg_gen_not_tl(tcg_tl_1, i3s_t0_tl);
    /* src: tcg_tl_1 = ~(-tcg_tl_1 + tcg_i64_1) / tcg_tl_2 */
    tcg_gen_neg_tl(i3s_t0_tl, tcg_tl_1);
    tcg_gen_trunc_i64_tl(i3s_t3_tl, tcg_i64_1);
    tcg_gen_add_tl(i3s_t0_tl, i3s_t0_tl, i3s_t3_tl);
    tcg_gen_not_tl(i3s_t0_tl, i3s_t0_tl);
    tcg_gen_divu_tl(tcg_tl_1, i3s_t0_tl, tcg_tl_2);
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
    tcg_temp_free(i3s_t0_tl);
    tcg_temp_free_i64(i3s_t1_i64);
    tcg_temp_free_i32(i3s_t2_i32);
    tcg_temp_free(i3s_t3_tl);
    tcg_temp_free_i64(i3s_t4_i64);
    tcg_temp_free_i32(i3s_t5_i32);
    tcg_temp_free_i32(i3s_t6_i32);
    tcg_temp_free(i3s_t7_tl);
    tcg_temp_free_i32(i3s_t8_i32);
    tcg_temp_free(tcg_tl_1);
    tcg_temp_free(tcg_tl_2);
    tcg_temp_free(tcg_tl_signed);
    tcg_temp_free_i32(tcg_i32_1);
    tcg_temp_free_i32(tcg_i32_2);
    tcg_temp_free_i64(tcg_i64_1);
    tcg_temp_free_i64(tcg_i64_2);
}

int if_test(void) {
    TCGv tcg_tl = tcg_temp_local_new();
    int c_var;

    if (c_var == 0)
        if (c_var != 0) {
            TCGv tcg_tl_1 = tcg_temp_new();
            /* src: tcg_tl_1 = tcg_tl_1 + 1 */
            tcg_gen_addi_tl(tcg_tl_1, tcg_tl_1, 1);
            tcg_temp_free(tcg_tl_1);
        } else
            c_var = 5;

    // if (tcg_tl > 0)
    /* src: if (tcg_tl > 0) */
    TCGLabel *i3s_label_0 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_LEU, tcg_tl, 0, i3s_label_0); /* open curly braces */
    TCGv tcg_tl_2 = tcg_temp_local_new();
    // if (start < 0)
    /* src: if (tcg_tl < 0) */
    TCGLabel *i3s_label_1 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_GEU, tcg_tl, 0, i3s_label_1);
    TCGv tcg_tl_3 = tcg_temp_new();
    /* src: tcg_tl_2 = 00 */
    tcg_gen_movi_tl(tcg_tl_2, 00);
    /* src: tcg_tl_3 = UNKNOWN_MACRO + N */
    tcg_gen_movi_tl(tcg_tl_3, UNKNOWN_MACRO + N);
    tcg_temp_free(tcg_tl_3);
    tcg_temp_free(tcg_tl);
    tcg_temp_free(tcg_tl_2);
    // wrog return;
    return N;
    // end if (start < 0)
    gen_set_label(i3s_label_1);
    if (c_var == 0) { /* open curly braces */
        /* src: tcg_tl_2 = 0 */
        tcg_gen_movi_tl(tcg_tl_2, 0);
    }
    // end if (tcg_tl > 0)
    gen_set_label(i3s_label_0);

    // if (start < 0)
    /* src: if (tcg_tl < 0) */
    TCGLabel *i3s_label_2 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_GEU, tcg_tl, 0, i3s_label_2);
    TCGv tcg_tl_4 = tcg_temp_new();
    /* src: tcg_tl_4 = tcg_tl */
    tcg_gen_mov_tl(tcg_tl_4, tcg_tl);
    tcg_temp_free(tcg_tl_4);
    TCGLabel *i3s_label_3 = gen_new_label();
    tcg_gen_br(i3s_label_3);
    /* src: end of the "if" */
    gen_set_label(i3s_label_2);
    // else
    /* src: else */
    TCGv tcg_else = tcg_temp_new();
    /* src: tcg_else = 2 */
    tcg_gen_movi_tl(tcg_else, 2);
    tcg_temp_free(tcg_else);
    //else end
    /* src: end of the "else" */
    gen_set_label(i3s_label_3);

    //if (start == 0)
    /* src: if (tcg_tl) */
    TCGLabel *i3s_label_4 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, tcg_tl, 0, i3s_label_4);
    TCGv tcg_in_if = tcg_temp_new();
    /* src: tcg_tl = tcg_in_if */
    tcg_gen_mov_tl(tcg_tl, tcg_in_if);
    tcg_temp_free(tcg_in_if);
    TCGLabel *i3s_label_5 = gen_new_label();
    tcg_gen_br(i3s_label_5);
    /* src: end of the "if" */
    gen_set_label(i3s_label_4);
    /* src: else if (tcg_tl == 777) */
    TCGLabel *i3s_label_6 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_NE, tcg_tl, 777, i3s_label_6);
    // else if (tcg_tl == 777)
    TCGv tcg_test = tcg_temp_new();
    /* src: tcg_test = tcg_tl */
    tcg_gen_mov_tl(tcg_test, tcg_tl);
    /* src: test_decl(tcg_tl + tcg_tl) */
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t0_tl, tcg_tl, tcg_tl);
    TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
    tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t0_tl);
    tcg_temp_free(i3s_t0_tl);
    tcg_temp_free(tcg_test);
    test_decl(i3s_t0_local_i32);
    tcg_gen_br(i3s_label_5);
    /* src: end of the "if" */
    gen_set_label(i3s_label_6);
    // else
    /* src: else */
    /* src: tcg_tl = 5 */
    tcg_gen_movi_tl(tcg_tl, 5);
    /* src: end of the "else" */
    gen_set_label(i3s_label_5);

    TCGv_i64 boris = tcg_temp_local_new_i64();

    /* src: if (boris == boris + 5) */
    TCGv_i64 i3s_t1_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t1_i64, boris, 5);
    TCGLabel *i3s_label_7 = gen_new_label();
    tcg_gen_brcond_i64(TCG_COND_NE, boris, i3s_t1_i64, i3s_label_7);
    tcg_temp_free_i64(i3s_t1_i64);
    /* src: if (boris != boris + 5) */
    TCGv_i64 i3s_t2_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t2_i64, boris, 5);
    TCGLabel *i3s_label_8 = gen_new_label();
    tcg_gen_brcond_i64(TCG_COND_EQ, boris, i3s_t2_i64, i3s_label_8);
    tcg_temp_free_i64(i3s_t2_i64);
    // if (boris != 0)
    /* src: boris = (boris + 5) * 16 */
    TCGv_i64 i3s_t3_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t3_i64, boris, 5);
    tcg_gen_muli_i64(boris, i3s_t3_i64, 16);
    tcg_temp_free_i64(i3s_t3_i64);
    TCGLabel *i3s_label_9 = gen_new_label();
    tcg_gen_br(i3s_label_9);
    /* src: end of the "if" */
    gen_set_label(i3s_label_8);
    /* src: else if (boris >= boris + 5) */
    TCGv_i64 i3s_t4_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t4_i64, boris, 5);
    TCGLabel *i3s_label_10 = gen_new_label();
    tcg_gen_brcond_i64(TCG_COND_LT, boris, i3s_t4_i64, i3s_label_10);
    tcg_temp_free_i64(i3s_t4_i64);
    // else if (boris >= boris + 5)
    /* src: boris = (boris + 5) * 16 */
    TCGv_i64 i3s_t5_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t5_i64, boris, 5);
    tcg_gen_muli_i64(boris, i3s_t5_i64, 16);
    tcg_temp_free_i64(i3s_t5_i64);
    tcg_gen_br(i3s_label_9);
    /* src: end of the "if" */
    gen_set_label(i3s_label_10);
    // if (boris != 0) else
    /* src: else */
    /* src: boris = (boris + 5) * 16 */
    TCGv_i64 i3s_t6_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t6_i64, boris, 5);
    tcg_gen_muli_i64(boris, i3s_t6_i64, 16);
    tcg_temp_free_i64(i3s_t6_i64);
    /* src: end of the "else" */
    gen_set_label(i3s_label_9);
    TCGLabel *i3s_label_11 = gen_new_label();
    tcg_gen_br(i3s_label_11);
    /* src: end of the "if" */
    gen_set_label(i3s_label_7);
    /* src: else if (boris > boris + 5) */
    TCGv_i64 i3s_t7_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t7_i64, boris, 5);
    tcg_gen_brcond_i64(TCG_COND_LE, boris, i3s_t7_i64, i3s_label_11);
    tcg_temp_free_i64(i3s_t7_i64);
    // else if (boris > 6)
    /* src: boris = (boris + 5) * 16 */
    TCGv_i64 i3s_t8_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t8_i64, boris, 5);
    tcg_gen_muli_i64(boris, i3s_t8_i64, 16);
    tcg_temp_free_i64(i3s_t8_i64);
    gen_set_label(i3s_label_11);

    c_var = N;
    tcg_temp_free_i64(boris);
    tcg_temp_free_i32(i3s_t0_local_i32);
}

int switch_test(void) {
    TCGv x = tcg_temp_local_new();

    //switch(x+x) {
    /* src: switch (x+x) */
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t0_tl, x, x);
    TCGv i3s_t0_local_tl = tcg_temp_local_new();
    tcg_gen_mov_tl(i3s_t0_local_tl, i3s_t0_tl);
    tcg_temp_free(i3s_t0_tl);
    TCGLabel *i3s_label_0 = gen_new_label();
    TCGLabel *i3s_label_1 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, i3s_t0_local_tl, 10, i3s_label_1);
    TCGLabel *i3s_label_2 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, i3s_t0_local_tl, 22, i3s_label_2);
    TCGLabel *i3s_label_3 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, i3s_t0_local_tl, 6, i3s_label_3);
    TCGLabel *i3s_label_4 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, i3s_t0_local_tl, 7, i3s_label_4);
    TCGLabel *i3s_label_5 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_ALWAYS, 0, 0, i3s_label_5);
    //case 10:
    /* src: case 10: */
    gen_set_label(i3s_label_1);
    TCGv z = tcg_temp_local_new();
    /* src: if (z != 0) */
    TCGLabel *i3s_label_6 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, z, 0, i3s_label_6);
    /* src: z = z * 8 + z */
    TCGv i3s_t1_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t1_tl, z, 8);
    tcg_gen_add_tl(z, i3s_t1_tl, z);
    tcg_temp_free(i3s_t1_tl);
    TCGLabel *i3s_label_7 = gen_new_label();
    tcg_gen_br(i3s_label_7);
    /* src: end of the "if" */
    gen_set_label(i3s_label_6);
    /* src: else */
    /* src: z = z * 8 */
    tcg_gen_muli_tl(z, z, 8);
    //break;
    /* src: break */
    tcg_gen_br(i3s_label_0);
    /* src: end of the "else" */
    gen_set_label(i3s_label_7);
    /* src: case 22: */
    gen_set_label(i3s_label_2);
    //case6
    /* src: case 6: */
    gen_set_label(i3s_label_3);
    /* src: x = x * 8 + x */
    TCGv i3s_t2_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t2_tl, x, 8);
    tcg_gen_add_tl(x, i3s_t2_tl, x);
    tcg_temp_free(i3s_t2_tl);
    //case7
    /* src: case 7: */
    gen_set_label(i3s_label_4);
    TCGv chech_local_free = tcg_temp_local_new();
    while_test(chech_local_free);
    TCGv y = tcg_temp_new();
    tcg_gen_movi_tl(y, 0);
    /* src: x = x * 8 + y */
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t3_tl, x, 8);
    tcg_gen_add_tl(x, i3s_t3_tl, y);
    tcg_temp_free(i3s_t3_tl);
    tcg_temp_free(y);
    /* src: break */
    tcg_gen_br(i3s_label_0);
    /* src: default: */
    gen_set_label(i3s_label_5);
    //default
    /* src: x = x * 8 + x */
    TCGv i3s_t4_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t4_tl, x, 8);
    tcg_gen_add_tl(x, i3s_t4_tl, x);
    printf("No option selected");
    tcg_temp_free(i3s_t4_tl);
    /* src: end of the "switch" */
    /* src: default: */
    gen_set_label(i3s_label_0);


    int a;
    TCGv b = tcg_temp_local_new();
    display_errors(a);
    switch (a) {
    case 10:
    {
        /* src: x = x * 8 + x */
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_muli_tl(i3s_t0_tl, x, 8);
        tcg_gen_add_tl(x, i3s_t0_tl, x);
        tcg_temp_free(i3s_t0_tl);
        break;
    }
    case 9:
        break;
    case 8:
        /* src: x = x + 1 */
        tcg_gen_addi_tl(x, x, 1);
        break;
    case 11:
    {
        TCGv b1 = tcg_temp_local_new();
        TCGv chech_local_free = tcg_temp_local_new();
        while_test(chech_local_free);
        /* src: x = x * 8 + b1 */
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_muli_tl(i3s_t0_tl, x, 8);
        tcg_gen_add_tl(x, i3s_t0_tl, b1);
        tcg_temp_free(i3s_t0_tl);
        tcg_temp_free(b1);
        tcg_temp_free(chech_local_free);
        break;
    }
    case 5:
    case 6:
        printf("WARNING");
    case 7:
    {
        display_errors(a);
        printf("WARNING");
        /* src: test_decl(a + 1) */
        TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
        tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
        test_decl(i3s_t0_local_i32);
        tcg_temp_free_i32(i3s_t0_local_i32);
    }
    default:
    {
        printf("No option selected");
        //test_decl(b + b);
        /* src: test_decl(b + b) */
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_add_tl(i3s_t0_tl, b, b);
        TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t0_tl);
        tcg_temp_free(i3s_t0_tl);
        test_decl(i3s_t0_local_i32);
        TCGv i3s_t1_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t1_tl, b, a);
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t1_tl);
        tcg_temp_free(i3s_t1_tl);
        //a = test_decl(a + b);
        a = test_decl(i3s_t0_local_i32);
        //b += test_decl(b + test_decl(b + a));
        /* src: b += test_decl(b + test_decl(b + a)) */
        TCGv i3s_t2_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t2_tl, b, a);
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t2_tl);
        tcg_temp_free(i3s_t2_tl);
        TCGv i3s_t3_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t3_tl, b, test_decl(i3s_t0_local_i32));
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t3_tl);
        tcg_temp_free(i3s_t3_tl);
        tcg_gen_addi_tl(b, b, test_decl(i3s_t0_local_i32));
        TCGv i3s_t4_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t4_tl, b, a);
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t4_tl);
        tcg_temp_free(i3s_t4_tl);
        //a = 1 + test_decl(a + b);
        a = 1 + test_decl(i3s_t0_local_i32);
        //b = 1 + test_decl(a + b);
        /* src: b = 1 + test_decl(a + b) */
        TCGv i3s_t5_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t5_tl, b, a);
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i3s_t5_tl);
        tcg_temp_free(i3s_t5_tl);
        tcg_gen_movi_tl(b, 1 + test_decl(i3s_t0_local_i32));
        tcg_temp_free_i32(i3s_t0_local_i32);
    }
    }
    tcg_temp_free(x);
    tcg_temp_free(z);
    tcg_temp_free(chech_local_free);
    tcg_temp_free(b);
    tcg_temp_free(i3s_t0_local_tl);
}

int do_while_test(int m, const unsigned int check_const) {
    TCGv start = tcg_temp_local_new();
    int a;

    if (a) {
        a = 5;
        //end
    }

    /* src: start = start + 5 + start */
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_addi_tl(i3s_t0_tl, start, 5);
    tcg_gen_add_tl(start, i3s_t0_tl, start);

    // do_while start
    /* src: start of the "DoWhile" */
    tcg_temp_free(i3s_t0_tl);
    TCGLabel *i3s_label_0 = gen_new_label();
    TCGLabel *i3s_label_1 = gen_new_label();
    gen_set_label(i3s_label_0);
    if (a) {
        a = 555;
        // return
        /* src: start = start + 5 + start */
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t0_tl, start, 5);
        tcg_gen_add_tl(start, i3s_t0_tl, start);
        a = a + 5;
        tcg_temp_free(i3s_t0_tl);
        tcg_temp_free(start);
        //return;
        return;
    }
    do {
        a = 6;
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_muli_tl(i3s_t0_tl, start, 6);
        tcg_gen_add_tl(start, i3s_t0_tl, start);
        tcg_temp_free(i3s_t0_tl);
        break;
        continue;
    } while (a);
    // start = start * 6 + start;
    /* src: start = start * 6 + start */
    TCGv i3s_t2_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t2_tl, start, 6);
    tcg_gen_add_tl(start, i3s_t2_tl, start);

    /* src: start of the "DoWhile" */
    tcg_temp_free(i3s_t2_tl);
    TCGLabel *i3s_label_3 = gen_new_label();
    TCGLabel *i3s_label_4 = gen_new_label();
    TCGLabel *i3s_label_5 = gen_new_label();
    gen_set_label(i3s_label_3);
    // start = start * 7 + start;
    /* src: start = start * 7 + start */
    TCGv i3s_t4_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t4_tl, start, 7);
    tcg_gen_add_tl(start, i3s_t4_tl, start);
    // if (start) {
    /* src: if (start) */
    TCGLabel *i3s_label_6 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, start, 0, i3s_label_6);
    tcg_temp_free(i3s_t4_tl);
    /* src: start = 0 */
    tcg_gen_movi_tl(start, 0);
    //continue;
    /* src: continue */
    tcg_gen_br(i3s_label_5);
    gen_set_label(i3s_label_6);
    gen_set_label(i3s_label_5);
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_addi_tl(i3s_t3_tl, start, 6);
    tcg_gen_brcond_tl(TCG_COND_GEU, start, i3s_t3_tl, i3s_label_4);
    tcg_temp_free(i3s_t3_tl);
    tcg_gen_br(i3s_label_3);
    /* src: do {...} while ( start < start + 6) */
    gen_set_label(i3s_label_4);
    // b = start * 6 + start;
    TCGv b = tcg_temp_new();
    /* src: b = start * 6 + start */
    TCGv i3s_t5_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t5_tl, start, 6);
    tcg_gen_add_tl(b, i3s_t5_tl, start);
    if (a == 6) {
        tcg_temp_free(i3s_t5_tl);
        tcg_temp_free(b);
        tcg_temp_free(start);
        // return
        return;
    }
    TCGv i3s_t1_tl = tcg_temp_new();
    tcg_gen_addi_tl(i3s_t1_tl, start, 5);
    tcg_gen_brcond_tl(TCG_COND_LEU, start, i3s_t1_tl, i3s_label_1);
    tcg_temp_free(i3s_t1_tl);
    tcg_gen_br(i3s_label_0);
    //end
    /* src: do {...} while ( start > start + 5) */
    gen_set_label(i3s_label_1);
    tcg_temp_free(i3s_t5_tl);
    tcg_temp_free(b);

    if (a) {
        a = 5;
        tcg_temp_free(start);
        return;
    //end
    }
    tcg_temp_free(start);
}


int while_test(TCGv var) {

    TCGv test_while = tcg_temp_local_new();
    int a;

    if (a) {
        tcg_temp_free(test_while);
        return 5;
        // under return
    }

    start = start + 5 + start;

    // while (var > test_while + 5)
    /* src: while (var > test_while + 5) */
    TCGLabel *i3s_label_0 = gen_new_label();
    TCGLabel *i3s_label_1 = gen_new_label();
    gen_set_label(i3s_label_0);
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_addi_tl(i3s_t0_tl, test_while, 5);
    tcg_gen_brcond_tl(TCG_COND_LEU, var, i3s_t0_tl, i3s_label_1);
    tcg_temp_free(i3s_t0_tl);
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
    /* src: test_while = var * 6 + var */
    TCGv i3s_t1_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t1_tl, var, 6);
    tcg_gen_add_tl(test_while, i3s_t1_tl, var);
    a = 7;
    // while (test_while > 5 * var)
    /* src: while (test_while > 5 * var) */
    TCGLabel *i3s_label_2 = gen_new_label();
    TCGLabel *i3s_label_3 = gen_new_label();
    gen_set_label(i3s_label_2);
    tcg_temp_free(i3s_t1_tl);
    TCGv i3s_t2_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t2_tl, var, 5);
    tcg_gen_brcond_tl(TCG_COND_LEU, test_while, i3s_t2_tl, i3s_label_3);
    tcg_temp_free(i3s_t2_tl);
    // test_while = test_while * N + var
    /* src: test_while = test_while * N + var */
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t3_tl, test_while, N);
    tcg_gen_add_tl(test_while, i3s_t3_tl, var);
    // if (start) {
    /* src: if (test_while) */
    TCGLabel *i3s_label_4 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, test_while, 0, i3s_label_4);
    tcg_temp_free(i3s_t3_tl);
    start = 0;
    gen_set_label(i3s_label_4);
    //continue;
    /* src: continue */
    tcg_gen_br(i3s_label_2);
    /* src: while (var) */
    TCGLabel *i3s_label_5 = gen_new_label();
    TCGLabel *i3s_label_6 = gen_new_label();
    gen_set_label(i3s_label_5);
    tcg_gen_brcondi_tl(TCG_COND_EQ, var, 0, i3s_label_6);
    TCGv i3s_t4_tl = tcg_temp_new();
    /* src: var++ */
    tcg_gen_mov_tl(i3s_t4_tl, var);
    tcg_gen_addi_tl(var, var, 1);
    tcg_temp_free(i3s_t4_tl);
    tcg_gen_br(i3s_label_5);
    /* src: end of the "while" */
    //end
    gen_set_label(i3s_label_6);
    tcg_gen_br(i3s_label_2);
    /* src: end of the "while" */
    gen_set_label(i3s_label_3);
    // start += N
    start += N;
    tcg_gen_br(i3s_label_0);
    /* src: end of the "while" */
    //end
    gen_set_label(i3s_label_1);
    tcg_temp_free(test_while);
}

int test_duplicate_vars(void) {
    const int test[2];

    if (test){
        TCGv test = tcg_temp_new();
        /* src: if (test) */
        TCGLabel *i3s_label_0 = gen_new_label();
        tcg_gen_brcondi_tl(TCG_COND_EQ, test, 0, i3s_label_0);
        tcg_temp_free(test);
        TCGv test_0 = tcg_temp_local_new();
        /* src: test_0 = test + test */
        tcg_gen_add_tl(test_0, test_0, test_0);
        /* src: if (test) */
        TCGLabel *i3s_label_1 = gen_new_label();
        tcg_gen_brcondi_tl(TCG_COND_EQ, test_0, 0, i3s_label_1);
        TCGv test_1 = tcg_temp_local_new();
        /* src: test_1 = test + 1 */
        tcg_gen_addi_tl(test_1, test_1, 1);
        /* src: while (test < test) */
        TCGLabel *i3s_label_2 = gen_new_label();
        TCGLabel *i3s_label_3 = gen_new_label();
        gen_set_label(i3s_label_2);
        tcg_gen_brcond_tl(TCG_COND_GEU, test_1, test_1, i3s_label_3);
        /* src: ++test_1 */
        tcg_gen_addi_tl(test_1, test_1, 1);
        tcg_gen_br(i3s_label_2);
        /* src: end of the "while" */
        gen_set_label(i3s_label_3);
        gen_set_label(i3s_label_1);
        /* src: test_decl(test) */
        TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
        tcg_gen_trunc_tl_i32(i3s_t0_local_i32, test_0);
        test_decl(i3s_t0_local_i32);
        /* src: test_0 = test + test */
        tcg_gen_add_tl(test_0, test_0, test_0);
        gen_set_label(i3s_label_0);
        /* src: for (tcg test; test < test; test++) */
        TCGv test = tcg_temp_local_new();
        TCGLabel *i3s_label_4 = gen_new_label();
        TCGLabel *i3s_label_5 = gen_new_label();
        gen_set_label(i3s_label_4);
        tcg_gen_brcond_tl(TCG_COND_GEU, test, test, i3s_label_5);
        TCGv test_2 = tcg_temp_new();
        TCGv i3s_t0_tl = tcg_temp_new();
        /* src: test_2++ */
        tcg_gen_mov_tl(i3s_t0_tl, test_2);
        tcg_gen_addi_tl(test_2, test_2, 1);
        tcg_temp_free(i3s_t0_tl);
        tcg_temp_free(test_2);
        TCGv i3s_t1_tl = tcg_temp_new();
        tcg_gen_mov_tl(i3s_t1_tl, test);
        tcg_gen_addi_tl(test, test, 1);
        tcg_temp_free(i3s_t1_tl);
        tcg_gen_br(i3s_label_4);
        /* src: end of the "for" */
        gen_set_label(i3s_label_5);
        /* src: test = test + test */
        tcg_gen_add_tl(test, test, test);
        tcg_temp_free(test_0);
        tcg_temp_free(test_1);
        tcg_temp_free(test);
        tcg_temp_free_i32(i3s_t0_local_i32);
    }

    TCGv test_33 = tcg_temp_new();
    /* src: switch (test_33) */
    tcg_temp_free(test_33);
    TCGLabel *i3s_label_0 = gen_new_label();
    TCGLabel *i3s_label_1 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, test_33, 3, i3s_label_1);
    TCGLabel *i3s_label_2 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_EQ, test_33, 4, i3s_label_2);
    TCGLabel *i3s_label_3 = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_ALWAYS, 0, 0, i3s_label_3);
    /* src: case 3: */
    gen_set_label(i3s_label_1);
    TCGv i = tcg_temp_local_new();
    /* src: test_decl(i) */
    TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
    tcg_gen_trunc_tl_i32(i3s_t0_local_i32, i);
    test_decl(i3s_t0_local_i32);
    /* src: case 4: */
    gen_set_label(i3s_label_2);
    TCGv i_0 = tcg_temp_new();
    tcg_temp_free(i_0);
    /* src: default: */
    gen_set_label(i3s_label_3);
    TCGv i_1 = tcg_temp_new();
    /* src: ++i_1 */
    tcg_gen_addi_tl(i_1, i_1, 1);
    tcg_temp_free(i_1);
    /* src: end of the "switch" */
    /* src: default: */
    gen_set_label(i3s_label_0);

    TCGv i_2 = tcg_temp_new();
    tcg_temp_free(i_2);
    tcg_temp_free(i);
    tcg_temp_free_i32(i3s_t0_local_i32);
}

int for_test(void) {

    TCGv stop = tcg_temp_local_new();

    if (a) {
        a = 5;
        //end
    }

    /* src: stop %= stop + 5 + stop */
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_addi_tl(i3s_t0_tl, stop, 5);
    tcg_gen_add_tl(i3s_t0_tl, i3s_t0_tl, stop);
    tcg_gen_remu_tl(stop, stop, i3s_t0_tl);

    // for (tcg start; start > start && stop; start++)
    /* src: for (signed long tcg start = 0; start > (start && stop); start++) */
    TCGv_i64 start = tcg_temp_local_new_i64();
    tcg_gen_movi_i64(start, 0);
    TCGLabel *i3s_label_0 = gen_new_label();
    TCGLabel *i3s_label_1 = gen_new_label();
    gen_set_label(i3s_label_0);
    tcg_temp_free(i3s_t0_tl);
    TCGv i3s_t1_tl = tcg_temp_new();
    tcg_gen_trunc_i64_tl(i3s_t1_tl, start);
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t1_tl, i3s_t1_tl, 0);
    TCGv i3s_t2_tl = tcg_temp_new();
    tcg_gen_setcondi_tl(TCG_COND_NE, i3s_t2_tl, stop, 0);
    tcg_gen_and_tl(i3s_t1_tl, i3s_t1_tl, i3s_t2_tl);
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_trunc_i64_tl(i3s_t3_tl, start);
    tcg_gen_brcond_tl(TCG_COND_LEU, i3s_t3_tl, i3s_t1_tl, i3s_label_1);
    tcg_temp_free(i3s_t1_tl);
    tcg_temp_free(i3s_t2_tl);
    tcg_temp_free(i3s_t3_tl);
    if (a) {
        a = 555;
    //end
    }
    for (;;) {
        a = 6;
        break;
        continue;
        tcg_temp_free(stop);
        tcg_temp_free_i64(start);
        return (a * 7);
    } while (a);
    // start = start * 6 + start;
    /* src: start = start * 6 + start */
    TCGv_i64 i3s_t4_i64 = tcg_temp_new_i64();
    tcg_gen_muli_i64(i3s_t4_i64, start, 6);
    tcg_gen_add_i64(start, i3s_t4_i64, start);
    a = 7;
    tcg_temp_free_i64(i3s_t4_i64);
    //break;
    /* src: break */
    tcg_gen_br(i3s_label_1);

    /* src: start = stop * 6 / stop */
    TCGv i3s_t5_tl = tcg_temp_new();
    tcg_gen_muli_tl(i3s_t5_tl, stop, 6);
    tcg_gen_divu_tl(i3s_t5_tl, i3s_t5_tl, stop);
    tcg_gen_ext_tl_i64(start, i3s_t5_tl);
    for (int a = 6; a < 7; a++) {
        TCGv_i64 i3s_t0_i64 = tcg_temp_new_i64();
        tcg_gen_muli_i64(i3s_t0_i64, start, 7);
        tcg_gen_add_i64(start, i3s_t0_i64, start);
        tcg_temp_free_i64(i3s_t0_i64);
        tcg_temp_free(i3s_t5_tl);
        tcg_temp_free(stop);
        tcg_temp_free_i64(start);
        //return
        return start;
    }
    // for(;start > start + 5;)
    /* src: for (;start > start + 5;) */
    TCGLabel *i3s_label_3 = gen_new_label();
    TCGLabel *i3s_label_4 = gen_new_label();
    TCGLabel *i3s_label_5 = gen_new_label();
    gen_set_label(i3s_label_3);
    tcg_temp_free(i3s_t5_tl);
    TCGv_i64 i3s_t6_i64 = tcg_temp_new_i64();
    tcg_gen_addi_i64(i3s_t6_i64, start, 5);
    tcg_gen_brcond_i64(TCG_COND_LE, start, i3s_t6_i64, i3s_label_4);
    tcg_temp_free_i64(i3s_t6_i64);
    // start = start * stop;
    /* src: start = start * stop */
    TCGv i3s_t7_tl = tcg_temp_new();
    tcg_gen_trunc_i64_tl(i3s_t7_tl, start);
    tcg_gen_mul_tl(i3s_t7_tl, i3s_t7_tl, stop);
    tcg_gen_ext_tl_i64(start, i3s_t7_tl);
    // if (start) {
    /* src: if (start) */
    TCGLabel *i3s_label_6 = gen_new_label();
    tcg_gen_brcondi_i64(TCG_COND_EQ, start, 0, i3s_label_6);
    tcg_temp_free(i3s_t7_tl);
    /* src: start = 0 */
    tcg_gen_movi_i64(start, 0);
    //continue;
    /* src: continue */
    tcg_gen_br(i3s_label_5);
    gen_set_label(i3s_label_6);
    gen_set_label(i3s_label_5);
    tcg_gen_br(i3s_label_3);
    /* src: end of the "for" */
    gen_set_label(i3s_label_4);
    // start = start * 6 + start; second
    /* src: start = start * 6 + start */
    TCGv_i64 i3s_t8_i64 = tcg_temp_new_i64();
    tcg_gen_muli_i64(i3s_t8_i64, start, 6);
    tcg_gen_add_i64(start, i3s_t8_i64, start);
    TCGv chech_local_free = tcg_temp_local_new();
    /* src: while_test(chech_local_free) */
    tcg_temp_free_i64(i3s_t8_i64);
    while_test(chech_local_free);
    TCGv_i64 i3s_t9_i64 = tcg_temp_new_i64();
    tcg_gen_mov_i64(i3s_t9_i64, start);
    tcg_gen_addi_i64(start, start, 1);
    tcg_temp_free_i64(i3s_t9_i64);
    tcg_gen_br(i3s_label_0);
    /* src: end of the "for" */
    // end for
    gen_set_label(i3s_label_1);

    /* src: stop = END_MACRO */
    tcg_gen_movi_tl(stop, END_MACRO);
    tcg_temp_free(stop);
    tcg_temp_free_i64(start);
    tcg_temp_free(chech_local_free);
}

/* test func call */

int helper(TCGv_i32 a1, TCGv a2) {}

int func_call_test(void) {
    int a = 5;
    TCGv b = tcg_temp_local_new();
    tcg_gen_movi_tl(b, 6);

    // helper(a + 1, b + b);
    /* src: helper(a + 1, b + b) */
    TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
    tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
    TCGv i3s_t0_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t0_tl, b, b);
    TCGv i3s_t1_local_tl = tcg_temp_local_new();
    tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t0_tl);
    tcg_temp_free(i3s_t0_tl);
    helper(i3s_t0_local_i32, i3s_t1_local_tl);
    tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
    TCGv i3s_t1_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t1_tl, b, b);
    tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t1_tl);
    tcg_temp_free(i3s_t1_tl);

    // a = helper(a + 1, b + b);
    a = helper(i3s_t0_local_i32, i3s_t1_local_tl);
    tcg_gen_movi_i32(i3s_t0_local_i32, a);

    if (helper(i3s_t0_local_i32, b) > 1) {
        /* src: helper(a + 1, b + b) */
        TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
        tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_add_tl(i3s_t0_tl, b, b);
        TCGv i3s_t1_local_tl = tcg_temp_local_new();
        tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t0_tl);
        tcg_temp_free(i3s_t0_tl);
        helper(i3s_t0_local_i32, i3s_t1_local_tl);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        a = helper(i3s_t0_local_i32, b);
        tcg_temp_free_i32(i3s_t0_local_i32);
        tcg_temp_free(i3s_t1_local_tl);
    } else {
        // helper(a, a);
        /* src: helper(a, a) */
        TCGv_i32 i3s_t0_local_i32 = tcg_temp_local_new_i32();
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        TCGv i3s_t1_local_tl = tcg_temp_local_new();
        tcg_gen_movi_tl(i3s_t1_local_tl, a);
        helper(i3s_t0_local_i32, i3s_t1_local_tl);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        tcg_gen_movi_tl(i3s_t1_local_tl, a);
        // a = helper(a, a);
        a = helper(i3s_t0_local_i32, i3s_t1_local_tl);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        tcg_gen_movi_tl(i3s_t1_local_tl, a);
        //a = helper(a, b) + helper(a, a);
        a = helper(i3s_t0_local_i32, b) + helper(i3s_t0_local_i32, i3s_t1_local_tl);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        //a = helper(a, b) + helper(a, b);
        a = helper(i3s_t0_local_i32, b) + helper(i3s_t0_local_i32, b);
        //b = helper(a, a + b) + b;
        /* src: b = helper(a, a + b) + b */
        tcg_gen_movi_i32(i3s_t0_local_i32, a);
        TCGv i3s_t0_tl = tcg_temp_new();
        tcg_gen_addi_tl(i3s_t0_tl, b, a);
        tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t0_tl);
        tcg_temp_free(i3s_t0_tl);
        tcg_gen_addi_tl(b, b, helper(i3s_t0_local_i32, i3s_t1_local_tl));
        tcg_temp_free_i32(i3s_t0_local_i32);
        tcg_temp_free(i3s_t1_local_tl);
    }

    // b = helper(a + 1, b + b);
    /* src: b = helper(a + 1, b + b) */
    tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
    TCGv i3s_t2_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t2_tl, b, b);
    tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t2_tl);
    tcg_temp_free(i3s_t2_tl);
    tcg_gen_movi_tl(b, helper(i3s_t0_local_i32, i3s_t1_local_tl));
    // helper(a + 1, helper(a + 1, b + b));
    /* src: helper(a + 1, helper(a + 1, b + b)) */
    tcg_gen_movi_i32(i3s_t0_local_i32, a + 1);
    TCGv_i32 i3s_t2_local_i32 = tcg_temp_local_new_i32();
    tcg_gen_movi_i32(i3s_t2_local_i32, a + 1);
    TCGv i3s_t3_tl = tcg_temp_new();
    tcg_gen_add_tl(i3s_t3_tl, b, b);
    tcg_gen_mov_tl(i3s_t1_local_tl, i3s_t3_tl);
    tcg_temp_free(i3s_t3_tl);
    helper(i3s_t0_local_i32, helper(i3s_t2_local_i32, i3s_t1_local_tl));
    tcg_temp_free(b);
    tcg_temp_free_i32(i3s_t0_local_i32);
    tcg_temp_free(i3s_t1_local_tl);
    tcg_temp_free_i32(i3s_t2_local_i32);
}

#endif /* INCLUDE_I3S_C */
