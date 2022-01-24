//
//  helper.h
//  LoopMixer - App
//
//  Created by Zachary Rogers on 24/12/21.
//

#ifndef helper_h
#define helper_h

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

std::string GetModuleFileName() {
  Dl_info module_info;
  if (dladdr(reinterpret_cast<void*>(GetModuleFileName), &module_info) == 0) {
    // Failed to find the symbol we asked for.
    return std::string();
  }
  return std::string(module_info.dli_fname);
}

#define MAX_PATH 260

#endif /* helper_h */
