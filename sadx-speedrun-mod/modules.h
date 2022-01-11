#pragma once
#include <string>

__declspec(dllexport) extern char gammaFrames;
__declspec(dllexport) extern char gammaSeconds;
__declspec(dllexport) extern char gammaMinutes;

void init_quick_save_reload(std::string savepath, std::string filepath, int savenum);
void onFrame_quick_save_reload();

void init_gamma_timer(char* injectedMemory);
void run_gamma_timer();