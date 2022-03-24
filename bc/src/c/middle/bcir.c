#include "middle/bcir.h"

/*
block scratch pad:

BLOCK main(%1 : i32, %2 : []String)

%3 = Const "Hello, World"
%4 = StaticFuncCall "println" %3
%5 = Const 0
%6 = return %5


next
%1 = Const "true"
%2 = If %1 {
    %3 = Const 0
    %4 = return %3
}

*/

Bc_IrTu *bcir_tu_new() {
    Bc_IrTu *tu = malloc(sizeof(Bc_IrTu));
    tu->mem = bc_arena_new(1024);
    bc_list_new(sizeof(BcIr_Block *), 1, &tu->blocks);
    return tu;
}

void bcir_tu_free(Bc_IrTu *tu) {
    // nightmare function :)
}

BcIr_Block *bcir_block_new(Bc_IrTu *tu, char *name) {
    BcIr_Block *block = bc_arena_alloc(tu->mem, sizeof(BcIr_Block));
    block->name = name;
    bc_list_new(sizeof(BcIr_Node), 1, &block->nodes);
    return block;
}

void bcir_block_free(BcIr_Block *b) {
    // bad
}

BcIr_Node *bcir_node_new(BcIr_Block *block) {
    return bc_list_append_ptr(&block->nodes);
}

void bcir_node_add_arg(BcIr_Node *node, BcIr_Arg arg) {
    if (arg.kind == BcIr_ArgKind_Node) {
        BcIr_Node **append_parent = bc_list_append_ptr(&node->parents);
        *append_parent = arg.node;

        BcIr_Node **append_child = bc_list_append_ptr(&arg.node->children);
        *append_child = node;
    }
    BcIr_Arg *arg_append = bc_list_append_ptr(&node->op->arguments);
    *arg_append = arg;
}

size_t bcir_node_child_count(BcIr_Node *node, BcIr_OpKind kind) {
    size_t count = 0;
    for (size_t i = 0; i < node->children.length; i++) {
        BcIr_Node *child = bc_list_get(&node->children, i);
        if (child->op->kind == kind) {
            count += 1;
        }
    }
    return count;
}

size_t bcir_span_length(BcIr_Span span) { return span.end - span.start; }

void bcir_block_apply_rewrite_rule(Bc_IrTu *tu, BcIr_Block *block,
                                   BcIr_Pattern pattern,
                                   BcIr_RewriteRule rule) {

    BcIr_Span span;
    BcIr_Block *working_block = block;
    do {
        span = pattern(block);
        BcIr_BlockRewriter rewriter = {
            .old_block = working_block,
            .new_block = bcir_block_new(tu, working_block->name),
            .span = span,
        };
        rule(&rewriter);
    } while (bcir_span_length(span) > 0);
}