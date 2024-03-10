from data import NumberNode, BinOpNode, VarDeclNode, VarRefNode, ProgramNode, IfNode


def parse(tokens):
    index = 0

    # incremente l'index qui pointe sur la liste des tokens
    def consume(expected_type=None):
        nonlocal index
        if index < len(tokens):
            current_token = tokens[index]
            index += 1
            if expected_type and current_token[0] != expected_type:
                raise ValueError(f"Expected token type {expected_type}, got ({current_token[0]}, {current_token[1]})")
            return current_token
        raise ValueError("No more tokens")

    def parse_term():
        if tokens[index][0] == 'NUMBER':
            token = consume('NUMBER')
            return NumberNode(token[1])
        if tokens[index][0] == 'IDENTIFIER':
            token = consume('IDENTIFIER')
            return VarRefNode(token[1]) 


    def parse_expr():
        node = parse_term()
        while index < len(tokens) and tokens[index][0] in ('PLUS', 'TIMES', 'EQUALS_EQUALS'):
            op = consume()[0]
            right = parse_term()
            node = BinOpNode(node, op, right)
        return node

    def parse_block():
        consume('LBRACE')
        statements = []
        while index < len(tokens) and tokens[index][0] != 'RBRACE':
            statements.append(parse_stmt())
        consume('RBRACE')
        return statements

    def parse_if():
        consume('IF')
        consume('LPAREN')
        condition = parse_expr()
        consume('RPAREN')
        true_branch = parse_block()
        # Pas de prise en charge de else pour le moment
        return IfNode(condition, true_branch)

    def parse_stmt():
        if tokens[index][0] == 'IF':
            return parse_if()
        if tokens[index][0] == 'IDENTIFIER' and tokens[index + 1][0] == 'EQUALS':
            var_name = consume('IDENTIFIER')[1]
            consume('EQUALS')
            value = parse_expr()
            consume('SEMICOLON')
            return VarDeclNode(var_name, value)

    def parse_prog():
        # on met tout le resultat dans une liste
        statements = []
        while index < len(tokens) and tokens[index][0] != 'EOF':
            statements.append(parse_stmt())
        return ProgramNode(statements)

    return parse_prog()


def print_ast(node, indent="", last=True, is_value=False):
    # Choix du symbole de branchement
    branch = "" if is_value else ("└─ " if last else "├─ ")
    
    # Choix de l'indentation pour la ligne suivante
    next_indent = indent + ("   " if last else "│  ")
    
    if isinstance(node, ProgramNode):
        print(f"{indent}ProgramNode")
        for i, statement in enumerate(node.statements):
            print_ast(statement, next_indent, last=(i == len(node.statements) - 1))
    elif isinstance(node, IfNode):
        print(f"{indent}{branch}IfNode")
        # Ajustement pour inclure une barre verticale continue pour la condition
        print(f"{next_indent}├─ condition: ")
        # Ajout d'une barre verticale pour aligner avec le bloc suivant
        print_ast(node.condition, next_indent + "│  ", is_value=True)
        # Utilisation de la même indentation pour le true_block, mais avec une correction pour l'alignement
        if len(node.true_block) > 0:
            print(f"{next_indent}└─ true_block: ")
            for i, stmt in enumerate(node.true_block):
                print_ast(stmt, next_indent + "   ", last=(i == len(node.true_block) - 1))
        else:
            print(f"{next_indent}└── true_block: ")
    elif isinstance(node, BinOpNode):
        print(f"{indent}{branch}BinOpNode")
        print(f"{next_indent}├─ op: {node.op}")
        print(f"{next_indent}├─ left: ")
        print_ast(node.left, next_indent + "│  ", last=False)
        print(f"{next_indent}└─ right: ")
        print_ast(node.right, next_indent + "   ")
    elif isinstance(node, NumberNode):
        print(f"{indent}{branch}NumberNode(value={node.value})")
    elif isinstance(node, VarDeclNode):
        print(f"{indent}{branch}VarDeclNode")
        print(f"{next_indent}├─ name: {node.name}")
        print(f"{next_indent}└─ value: ")
        if node.value:
            print_ast(node.value, next_indent + "   ", is_value=True)
    elif isinstance(node, VarRefNode):
        print(f"{indent}{branch}VarRefNode(name={node.name})")