using namespace std;

#include "cpp_generator.hpp"
#include <iostream>
#include <string>

string gentab(int tab_offset)
{
  string tabs_output = "";
  for (int i = 0; i < tab_offset; i++) {
    tabs_output += "    ";
  }
  return tabs_output;
}

string generate_cpp_impl(const NodePtr &node, int tab_offset)
{
  string output = "";

  string tabs_output = gentab(tab_offset);

  if (ProgramNode *pnode = dynamic_cast<ProgramNode *>(node.get())) {
    output += "#include <iostream>\n";
    output += "\n";
    output += "int main() {\n";
    for (const NodePtr &stmt : pnode->_statements) {
      output += generate_cpp_impl(stmt, 1);
    }
    output += gentab(1) + "return 0;\n";
    output += "}\n";
  } else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
    output += tabs_output + "int v_" + vnode->_name + " = ";
    output += generate_cpp_impl(vnode->_value, tab_offset);
    output += ";\n";
  } else if (NumberNode *nnode = dynamic_cast<NumberNode *>(node.get())) {
    output += to_string(nnode->_value);
  } else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
    output += "( ";
    output += generate_cpp_impl(bnode->_left, tab_offset);
    if (bnode->_op == PLUS) {
      output += " + ";
    } else if (bnode->_op == MINUS) {
      output += " - ";
    } else if (bnode->_op == TIMES) {
      output += " * ";
    } else if (bnode->_op == DIVIDE) {
      output += " / ";
    } else if (bnode->_op == EQUALS_EQUALS) {
      output += " == ";
    }
    output += generate_cpp_impl(bnode->_right, tab_offset);
    output += " )";
  } else if (VarRefNode *vrefnode = dynamic_cast<VarRefNode *>(node.get())) {
    output += "v_" + vrefnode->_name;
  } else if (IfNode *inode = dynamic_cast<IfNode *>(node.get())) {
    output += tabs_output + "if ( ";
    output += generate_cpp_impl(inode->_condition, tab_offset);
    output += " ) {\n";
    for (const NodePtr &stmt : inode->_true_block) {
      output += generate_cpp_impl(stmt, tab_offset + 1);
    }
    output += tabs_output + "} else {\n";
    for (const NodePtr &stmt : inode->_false_block) {
      output += generate_cpp_impl(stmt, tab_offset + 1);
    }
    output += tabs_output + "}\n";
  } else if (ExpressionStatementNode *esnode =
                 dynamic_cast<ExpressionStatementNode *>(node.get())) {
    output += tabs_output;
    output += generate_cpp_impl(esnode->_expression, tab_offset);
    output += ";\n";
  } else if (FunctionCallNode *fcnode =
                 dynamic_cast<FunctionCallNode *>(node.get())) {
    if (fcnode->_name == "println") {
      output += "(std::cout << ";
      output += generate_cpp_impl(fcnode->_args[0], tab_offset);
      output += " << std::endl)";
    }
    else {
      std::cout << "Unknown function call: " << fcnode->_name << std::endl;
    }
  } else {
    cout << "Unknown node type" << endl;
  }

  return output;
}

string generate_cpp(const NodePtr &node) { return generate_cpp_impl(node, 0); }