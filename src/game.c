#include "tools.h"

static void
init(void)
{
}

static void
update(void)
{

}

static void 
render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(fabs(sinf(global_platform.current_time/8.f)), fabs(sinf(global_platform.current_time) / 13.f),fabs(cosf(global_platform.current_time / 7.f)), 1);
}
