/*
main

Кучуков Ридаль Радикович
МК-101
*/

#include "passgen/passgen.h"

int main(int argc, char *argv[])
{
    PassgenError err = passgen_run(argc, argv);
    if (err != PASSGEN_OK) {
        return 1;
    }
    return 0;
}