enum NodeKind {
    // Statements
    block, if_stmt, while_stmt, for_stmt, 
    return_stmt, break_stmt, continue_stmt, 
    expr_stmt, decl_stmt, empty_stmt,

    // Expressions
    binary_expr, unary_expr, postfix_expr, 
    prefix_expr, call_expr, array_expr,
    member_expr, ident_expr, literal_expr,
    number_expr, string_expr, boolean_expr,
};

#define DEPTH_RATE 2

struct Node {
    NodeKind kind;
    Node(NodeKind kind): kind(kind) {}
    virtual void debug (int depth) = 0;
};

struct stmt_node: Node {
    stmt_node(NodeKind kind): Node(kind) {}
};

struct expr_node: Node {
    expr_node(NodeKind kind): Node(kind) {}
};

// Statements
struct block_stmt: stmt_node {
    std::vector<std::shared_ptr<stmt_node>> stmts;
    block_stmt(): stmt_node(NodeKind::block) {}

    void debug (int depth) {
        std::cout << std::string(depth * DEPTH_RATE, ' ') << "block_stmt" << std::endl;
        for (auto stmt: stmts) {
            stmt->debug(depth + 1);
        }
    }
};