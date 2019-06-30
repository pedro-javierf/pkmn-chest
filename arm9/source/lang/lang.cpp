#include "lang.h"
#include <fstream>
#include "inifile.h"
#include "langStrings.h"

std::string langs[] = { "de", "en", "es", "fr", "it", "jp"};

void Lang::loadLangStrings(int lang) {
	// Clear vectors
	Lang::balls.clear();
	Lang::items.clear();
	Lang::locations4.clear();
	Lang::locations5.clear();
	Lang::moves.clear();
	Lang::natures.clear();
	Lang::species.clear();
	
	// Declare variables
	char path[32];
	std::string line;

	// Fill vectors from files
	snprintf(path, sizeof(path), "nitro:/lang/%s/balls", langs[lang].c_str());
	std::ifstream in(path);
	while(std::getline(in, line)) {
		Lang::balls.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/items", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::items.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/locations4", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::locations4.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/locations5", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::locations5.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/moves", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::moves.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/natures", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::natures.push_back(line);
	}
	in.close();

	snprintf(path, sizeof(path), "nitro:/lang/%s/species", langs[lang].c_str());
	in.open(path);
	while(std::getline(in, line)) {
		Lang::species.push_back(line);
	}
	in.close();

	// Load app strings
	snprintf(path, sizeof(path), "nitro:/lang/%s/app.ini", langs[lang].c_str());
	CIniFile ini(path);

	// [main]
	Lang::loading = ini.GetString("main", "loading", Lang::loading);
	Lang::invalidSave = ini.GetString("main", "invalidSave", Lang::invalidSave);

	// [options]
	Lang::optionsTextLabels[0] = ini.GetString("options", "chestFile", Lang::optionsTextLabels[0]);
	Lang::optionsTextLabels[1] = ini.GetString("options", "chestSize", Lang::optionsTextLabels[1]);
	Lang::optionsTextLabels[2] = ini.GetString("options", "language", Lang::optionsTextLabels[2]);
	Lang::optionsText[0] = ini.GetString("options", "new", Lang::optionsText[0]);
	Lang::optionsText[1] = ini.GetString("options", "rename", Lang::optionsText[1]);
	Lang::optionsText[2] = ini.GetString("options", "delete", Lang::optionsText[2]);
	Lang::optionsText[3] = ini.GetString("options", "change", Lang::optionsText[3]);

	// [xMenu]
	Lang::xMenuText[0] = ini.GetString("xMenu", "party", Lang::xMenuText[0]);
	Lang::xMenuText[1] = ini.GetString("xMenu", "options", Lang::xMenuText[1]);
	Lang::xMenuText[4] = ini.GetString("xMenu", "save", Lang::xMenuText[4]);
	Lang::xMenuText[5] = ini.GetString("xMenu", "exit", Lang::xMenuText[5]);
}