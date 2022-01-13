#pragma once
#include <string>


void init_quick_save_reload(std::string savepath, std::string filepath, int savenum);
void onFrame_quick_save_reload();

void init_gamma_timer(char* injectedMemory, char* accessibleMemory);
void run_gamma_timer();