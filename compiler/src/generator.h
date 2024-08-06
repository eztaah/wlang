#ifndef GENERATOR_H
#define GENERATOR_H

#include "global.h"
#include "lib/lib.h"
#include "node.h"

/**
 * Generate assembly code from a list of nodes.
 */
Char* generate(const List* node_list);

#endif // GENERATOR_H
