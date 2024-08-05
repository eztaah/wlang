#ifndef GENERATOR_H
#define GENERATOR_H

#include "global.h"
#include "node.h"
#include "lib/lib.h"

/**
 * Generate assembly code from a list of nodes.
 */
Char* generate(List* node_list);

#endif // GENERATOR_H
