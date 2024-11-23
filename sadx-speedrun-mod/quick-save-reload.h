#pragma once

void init_quick_save_reload(std::string savepath, std::string filepath, int savenum, bool forceload);
void onFrame_quick_save_reload();

void reload_save_file();
bool load_save_file_data();
bool write_to_dest_save_file();

void ScaleDebugFontQSR(int scale, bool enableFontScaling);

