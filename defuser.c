#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    uint32_t low = (uint32_t)(challenge);
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);

    uint32_t step1 = high ^ (middle + 0x7a3);
    uint32_t step0 = (low + high * 2) ^ 0xfbe2;
    uint32_t step2 = (step0 - step1) * 3 +0x49b8;;

    values[0] = step2;
    count = 1;
    
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    values[0] = (((challenge >> 22) & 0xFF) + 15);

    uint32_t shift = 18;
    uint32_t add = 11;
    count = 1;

    for(uint32_t i = 0; i < 5; ++i) {
        values[i + 1] = (((challenge >> shift) & 0x3F) ^ (values[i] * 2)) + add;
        count++;
        shift++;
        add += 11;
    }

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    values[0] = ((uint32_t)(challenge) & 0x7);

    uint32_t mixed = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    uint32_t answer = 0;

    switch (values[0])
    {
    case 0:
        answer = (mixed * 2) ^ 0x58d2;
        break;
    
    case 1:
        answer = (mixed ^ 0x7845) + (mixed >> 1);;
        break;

    case 2:
        answer = mixed * 5 + 0x380b3;
        break;

    case 3:
        answer = mixed * 3 + 0x945e;
        break;

    case 4:
        answer = (mixed ^ 0x5591) + (mixed >> 1);
        break;

    case 5:
        answer = mixed * 5 + 0x1acf7;
        break;

    case 6:
        answer = (mixed * 5) + 0x683f;
        break;

    case 7:
        answer = (mixed * 4) ^ 0xc0fc;
        break;

    default:
        break;
    }

    values[1] = answer;
    count = 2;

    return write_answer(out, cap, values, count);
}

int p4_walk(uint32_t target, uint32_t low, uint32_t high, uint32_t salt){
    uint32_t mid = (low + high) / 2;
    if(mid == target){
        return (mid + 0x27) ^ salt;
    }
    uint32_t saved = mid + salt;
    if(mid > target){
        uint32_t result = p4_walk(target, low, mid - 1, salt);
        result = (result * 3) ^ saved;
        return result ^ 0x19;
    }
    else{
        uint32_t result = p4_walk(target, mid + 1, high, salt);
        result = (result * 3) ^ saved;
        return result ^ 0x17;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    uint32_t target = (uint32_t)(challenge & 0x7F);
    uint32_t low = 0;
    uint32_t high = 0x7f;
    uint32_t salt = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);

    values[0] = p4_walk(target, low, high, salt);
    count = 1;

    return write_answer(out, cap, values, count);
}

typedef struct Phase5Node{
    const struct Phase5Node *next;
    uint32_t value;
    uint32_t extra;
} Phase5Node;

static uint32_t p5_traverse(uint64_t challenge, uint32_t *out){
    uint32_t index = (uint32_t)(challenge) & 0x7;
    uint32_t steps = (uint32_t)(challenge >> 3) & 0x3;

    static const Phase5Node node_bd0 = {
    .next = NULL,
    .value = 0x083fu,
    .extra = 0x89fbu,
};

    static const Phase5Node node_c50 = {
        .next = &node_bd0,
        .value = 0x0106u,
        .extra = 0x8511u,
    };

    static const Phase5Node node_c00 = {
        .next = &node_c50,
        .value = 0x0b5du,
        .extra = 0x8027u,
    };

    static const Phase5Node node_c30 = {
        .next = &node_c00,
        .value = 0x0279u,
        .extra = 0x8651u,
    };

    static const Phase5Node node_be0 = {
        .next = &node_c30,
        .value = 0x0306u,
        .extra = 0x5f43u,
    };

    static const Phase5Node node_ba0 = {
        .next = &node_be0,
        .value = 0x060au,
        .extra = 0xcd36u,
    };

    static const Phase5Node node_bf0 = {
        .next = &node_ba0,
        .value = 0x0724u,
        .extra = 0xaafeu,
    };

    static const Phase5Node node_c10 = {
        .next = &node_bf0,
        .value = 0x0041u,
        .extra = 0x88bcu,
    };

    static const Phase5Node node_c20 = {
        .next = &node_c10,
        .value = 0x0a2fu,
        .extra = 0x78cfu,
    };

    static const Phase5Node node_bc0 = {
        .next = &node_c20,
        .value = 0x0920u,
        .extra = 0x4a68u,
    };

    static const Phase5Node node_c40 = {
        .next = &node_bc0,
        .value = 0x0466u,
        .extra = 0x35f6u,
    };

    static const Phase5Node node_bb0 = {
        .next = &node_c40,
        .value = 0x0555u,
        .extra = 0xbac6u,
    };
    
    static const Phase5Node *const heads[8] = {
    &node_bb0,
    &node_c40,
    &node_bc0,
    &node_c20,
    &node_c10,
    &node_bf0,
    &node_ba0,
    &node_be0
};

    const Phase5Node *node = heads[index];

    uint32_t acc = (uint32_t)(challenge >>32) ^ (uint32_t)(challenge >> 6) ^ 0xdfd;
    
    for(uint32_t i = 0; i <= steps; ++i){
        if(i != 0){
            node = node->next;
        }

        acc = (acc * 3 + node->extra) ^ node->value;
        *out = node->value;
    }

    return acc;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    uint32_t final_value = 0;
    values[0] = p5_traverse(challenge, &final_value);
    values[1] = final_value;
    count = 2;

    return write_answer(out, cap, values, count);
}

static uint32_t p6_operations(uint32_t index, uint32_t value){
    switch(index){
        case 6:
            return (value + 0x7823) ^ (value >> 0x3);
        case 3:
        case 7:
            return ((value >> 0x1) ^ value) + 0xb618;
        case 1:
        case 4:
            return ((value << 1) | (value >> 31)) ^ 0x7667;
        case 2:
            return (value ^ 0xa53f) + (value * 8);
        case 0:
            return ((value << 0x7) + 0x863d) ^ (value * 5);
        case 5:
            return (value >> 1) | (value <<31);
        default:
            return value;
    }
}

static uint32_t p6_dispatch(uint64_t challenge){
    uint32_t first_index = (uint32_t)(challenge & 0x7);
    uint32_t second_index = (uint32_t)(challenge >> 3) & 0x7;

    uint32_t argument = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    uint32_t result = p6_operations(first_index, argument);
    result ^= 0x23a0;

    result = p6_operations(second_index, result);
    result += 0x61d0;

    return result;
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    
    values[0] = p6_dispatch(challenge);
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    
    uint32_t selector = (uint32_t)(context & 0x3);

    uint32_t challenge_mix = (uint32_t)(challenge) ^ (uint32_t)(challenge >> 32);

    uint32_t context_mix = (uint32_t)(context) ^ (uint32_t)(context >> 32);

    uint32_t extra_mix = (selector * 0x6d92) ^ (uint32_t)(context >> 16);
    
    uint32_t mix_input = challenge_mix ^ context_mix;

    uint32_t mixed = (mix_input * 7 + 0x6d92u) ^ 0x4539u;
    
    uint32_t first = selector;

    uint32_t second = mixed ^ extra_mix;

    uint32_t tail_mix = context_mix + selector + 0x4539 + (challenge_mix >> 1);

    uint32_t third = tail_mix ^ second;

    static const char prefix[] = 
    "................................";

    int written = snprintf(out, cap, "%sOPEN%" PRIu32 " %" PRIu32 " %" PRIu32, prefix, first, second, third);
    
    if(written < 0 || (size_t)written >= cap) {
        return 1;
    }

    return 0;
}
