
#ifndef HYP_MODULES_H
#define HYP_MODULES_H

#include "common.h"

#define HYPERVISOR_MODULE(mod)     \
extern unsigned char mod[];    \
extern unsigned int mod##_len;

#include "modules/modules.list"
#undef HYPERVISOR_MODULE

#define HYPERVISOR_MODULE(mod)                                      \
  ret = common_add_module((const char*)mod, (uint64_t)mod##_len);   \
  if (ret < 0) {                                                    \
    Print(L"common_add_module returned %a\n", ec_to_str(ret));      \
    goto Abort;                                                   \
  }

uint64_t add_hypervisor_modules()
{
  int64_t ret;
  Print(L"Adding BF modules\n"); 
  #include "modules/modules.list"
  return ret;

Abort:
  return ret;
}
#undef HYPERVISOR_MODULE


#endif