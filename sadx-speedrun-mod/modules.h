#pragma once
#include <string>
#include <chrono>

// Quick Save Reload
void init_quick_save_reload(std::string savepath, std::string filepath, int savenum, bool isForceReloadEnabled);
void onFrame_quick_save_reload();

// Gamma Timer
void init_gamma_timer(char* accessibleMemory);
void run_gamma_timer();

// New IGT
void init_new_igt(char* accessibleMemory);
void get_RTA_time();
void start_RTA_timer();
void stop_RTA_timer();

// Chao Stat Grade Viewer
void DisplayChaoStatsOnFrame();
void displayChaoStatsInit();
