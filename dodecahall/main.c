#include "dodecahall.h"

#include "gles2_harness.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    
    dodecahall_init();
    
    gles2_harness_main();
    
    return 0;
}
