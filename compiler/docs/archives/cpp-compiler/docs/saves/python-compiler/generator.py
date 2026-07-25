from data import NumberNode, BinOpNode, VarDeclNode, VarRefNode, ProgramNode, IfNode


def generate_assembly(node, variables=None, offset=0, label_count=0):
    if variables is None:
        variables = {}
    
    instructions = []
    current_label_count = label_count

    if isinstance(node, ProgramNode):
        instructions.append(";rbp est fixe")
        instructions.append("push rbp")
        instructions.append("mov rbp, rsp\n")
        for stmt in node.statements:
            stmt_instructions, label_count = generate_assembly(stmt, variables, offset, label_count)
            instructions += stmt_instructions
            offset -= 8
        instructions.append("\nmov rsp, rbp")
        instructions.append("pop rbp")
    
    elif isinstance(node, VarDeclNode):
        var_offset = f"[rbp-{abs(offset)}]"
        variables[node.name] = var_offset
        value_instructions, label_count = generate_assembly(node.value, variables, offset, label_count)
        instructions += value_instructions
        instructions.append(f"mov {var_offset}, rax")
    
    elif isinstance(node, NumberNode):
        instructions.append(f"mov rax, {node.value}")
    
    elif isinstance(node, BinOpNode):
        right_instructions, label_count = generate_assembly(node.right, variables, offset, label_count)
        instructions += right_instructions
        instructions.append("push rax")
        left_instructions, label_count = generate_assembly(node.left, variables, offset, label_count)
        instructions += left_instructions
        instructions.append("pop rbx")
        if node.op == 'PLUS':
            instructions.append("add rax, rbx")

    elif isinstance(node, VarRefNode):
        var_offset = variables.get(node.name)
        if var_offset:
            instructions.append(f"mov rax, {var_offset}")
        else:
            raise ValueError(f"Undefined variable {node.name}")

    elif isinstance(node, IfNode):
        condition_instructions, label_count = generate_assembly(node.condition, variables, offset, label_count)
        instructions += condition_instructions
        instructions.append("cmp rax, rbx")
        true_label = f"if_true_{current_label_count}"
        end_label = f"if_end_{current_label_count}"
        instructions.append(f"jne {true_label}")
        # Pas de false branch pour l'instant, donc saute directement à la fin
        instructions.append(f"jmp {end_label}")
        instructions.append(f"{true_label}:")
        for stmt in node.true_block:
            true_block_instructions, label_count = generate_assembly(stmt, variables, offset, label_count + 1)
            instructions += true_block_instructions
        instructions.append(f"{end_label}:")

    return instructions, label_count