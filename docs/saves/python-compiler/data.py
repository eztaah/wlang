class NumberNode:
    def __init__(self, value):
        self.value = value

class VarRefNode:
    def __init__(self, name):
        self.name = name

class BinOpNode:
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class VarDeclNode:
    def __init__(self, name, value=None):
        self.name = name
        self.value = value

class IfNode:
    def __init__(self, condition, true_block, false_block=None):
        self.condition = condition
        self.true_block = true_block
        self.false_block = false_block


class ProgramNode:
    def __init__(self, statements):
        self.statements = statements