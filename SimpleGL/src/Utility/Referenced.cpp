#include "Utility/Referenced.h"

using namespace sgl;

void SGL_DLLCALL ref_counter::Destroy()
{
    delete this;
}
