#include "parser_rules.h"

#include "ast.h"
#include "bc.h"

CREATE_RULE(bc_module_rule) {
    START_ALTERNATIVE(module)
    AST_ALLOC(Bc_Module, module){
        .decls = NULL,
        .length = 0,
    };

    EXPECT_LIST(Bc_Decl, bc_decl_rule, module->decls, module->length);

    return BC_PACKRAT_SUCCESS(module);

    END_ALTERNATIVE()
}