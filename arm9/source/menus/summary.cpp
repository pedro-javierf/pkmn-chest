#include "summary.h"
#include "colors.h"
#include "flashcard.h"
#include "langStrings.h"
#include "loader.h"
#include "input.h"
#include "manager.h"
#include "misc.h"
#include "PK5.hpp"
#include "sound.h"

struct Text {
	int x;
	int y;
	char text[32];
};

Text textC1[] {
	{64, 4},
	{64, 20},
	{64, 36},
	{64, 52},
	{64, 68},
	{64, 84},
	{64, 100},
	{96, 124},
	{96, 140},
	{96, 156},
	{96, 172},
};

Text textC2[] = {
	{164, 8},
	{230, 30},
	{170, 102},
	{170, 132},
	{170, 162},
};

int summaryPage = 0;

std::pair<int, int> getPokeballPosition(u8 ball) {
	if(ball > 25)	return {0, 0};
	std::pair<int, int> xy;
	xy.second = (ball/9)*15;
	xy.first = (ball-((ball/9)*9))*15;
	return xy;
}

void changeAbility(std::shared_ptr<PKX> &pkm) {
	if(pkm->gen4()) {
		u8 setAbility = pkm->ability();
		if(pkm->abilities(0) != setAbility && pkm->abilities(0) != 0) {
			pkm->setAbility(0);
		} else if(pkm->abilities(1) != 0) {
			pkm->setAbility(1);
		} else { // Just in case
			pkm->setAbility(0);
		}
	} else if(pkm->generation() == Generation::FIVE) {
		PK5* pk5 = (PK5*)pkm.get();
		switch (pkm->abilityNumber() >> 1) {
			case 0:
				if(pkm->abilities(1) != pkm->ability() && pkm->abilities(1) != 0) {
					pkm->ability(pkm->abilities(1));
					if(pkm->abilities(1) == pkm->abilities(2)) {
						pk5->hiddenAbility(true);
					}
				} else if(pkm->abilities(2) != 0) {
					pkm->ability(pkm->abilities(2));
					pk5->hiddenAbility(true);
				}
				break;
			case 1:
				if(pkm->abilities(2) != pkm->ability() && pkm->abilities(2) != 0) {
					pkm->ability(pkm->abilities(2));
					pk5->hiddenAbility(true);
				} else if(pkm->abilities(0) != 0) {
					pkm->ability(pkm->abilities(0));
					pk5->hiddenAbility(false);
				}
				break;
			case 2:
				if(pkm->abilities(0) != pkm->ability() && pkm->abilities(0) != 0) {
					pkm->ability(pkm->abilities(0));
					pk5->hiddenAbility(false);
				} else if(pkm->abilities(1) != 0) {
					pkm->ability(pkm->abilities(1));
					if(pkm->abilities(1) == pkm->abilities(2)) {
						pk5->hiddenAbility(true);
					} else {
						pk5->hiddenAbility(false);
					}
				}
				break;
		}
	}
}

void drawSummaryPage(std::shared_ptr<PKX> pkm) {
	// Draw background
	if(sdFound())	drawImage(0, 0, summaryBgData.width, summaryBgData.height, summaryBg, false);
	else {
		drawRectangle(0, 0, 256, 192, DARK_GRAY, false);
		drawOutline(145, -1, 111, 88, LIGHT_GRAY, false);
	}
	// Draw lines
	drawOutline(0, 124, 160, 69, LIGHT_GRAY, false);

	// Hide sprites
	for(int i=0;i<30;i++) {
		setSpriteVisibility(i, false);
	}
	updateOam();

	// Print Pokémon name
	printTextTintedMaxW(Lang::species[pkm->species()], 90, 1, (pkm->gender() ? (pkm->gender() == 1 ? RGB::RED : GRAY) : RGB::BLUE), 165, 8, false, true);

	// Draw Pokémon, Pokéball, types, and shiny star (if shiny)
	std::pair<int, int> xy = getPokeballPosition(pkm->ball());
	drawImageFromSheet(148, 8, 15, 15, ballSheet, ballSheetData.width, xy.first, xy.second, false);

	std::vector<u16> bmp;
	ImageData bmpData = loadPokemonSprite(getPokemonIndex(getPokemonIndex(pkm)), bmp);
	drawImageScaled(169, 22, bmpData.width, bmpData.height, 2, bmp, false);

	drawImageFromSheet(150, 26, 32, 12, types, 32, 0, (((pkm->generation() == Generation::FOUR && pkm->type1() > 8) ? pkm->type1()-1 : pkm->type1())*12), false);
	if(pkm->type1() != pkm->type2())
		drawImageFromSheet(185, 26, 32, 12, types, 32, 0, (((pkm->generation() == Generation::FOUR && pkm->type2() > 8) ? pkm->type2()-1 : pkm->type2())*12), false);
	if(pkm->shiny())	drawImage(150, 45, shinyData.width, shinyData.height, shiny, false);

	// Print Pokémon and trainer info labels
	for(unsigned i=0;i<Lang::summaryLabels.size();i++) {
		printTextMaxW(Lang::summaryLabels[i], textC1[i].x-8, 1, 4, textC1[i].y, false);

	}


	// Print Pokémon and trainer info
	snprintf(textC1[0].text,  sizeof(textC1[0].text), "%.3i", pkm->species());
	if(pkm->nicknamed())	snprintf(textC1[1].text,  sizeof(textC1[1].text), "%s", pkm->nickname().c_str());
	else	snprintf(textC1[1].text,  sizeof(textC1[1].text), "%s", Lang::species[pkm->species()].c_str());
	snprintf(textC1[2].text,  sizeof(textC1[2].text), "%i", pkm->level());
	snprintf(textC1[3].text,  sizeof(textC1[3].text), "%s", Lang::abilities[pkm->ability()].c_str());
	snprintf(textC1[4].text,  sizeof(textC1[4].text), "%s", Lang::natures[pkm->nature()].c_str());
	snprintf(textC1[5].text,  sizeof(textC1[5].text), "%s", pkm->shiny() ? Lang::yes.c_str() : Lang::no.c_str());
	snprintf(textC1[6].text,  sizeof(textC1[6].text), "%s", pkm->pkrs() ? Lang::yes.c_str() : Lang::no.c_str());
	snprintf(textC1[7].text,  sizeof(textC1[7].text), "%s", pkm->otName().c_str());
	snprintf(textC1[8].text,  sizeof(textC1[8].text), "%.5i", pkm->TID());
	snprintf(textC1[9].text,  sizeof(textC1[9].text), "%.5i", pkm->SID());
	snprintf(textC1[10].text, sizeof(textC1[10].text),"%i", pkm->otFriendship());
	for(unsigned i=0;i<(sizeof(textC1)/sizeof(textC1[0]));i++) {
		if(i!=7)	// OT Name is colored
			printTextMaxW(textC1[i].text, 80, 1, textC1[i].x, textC1[i].y, false);
	}
	printTextTinted(textC1[7].text, (pkm->otGender() ? RGB::RED : RGB::BLUE), textC1[7].x, textC1[7].y, false);

	// Draw buttons // The first 2 don't have buttons
	for(unsigned i=2;i<sizeof(textC2)/sizeof(textC2[0]);i++) {
		drawImage(textC2[i].x-4, textC2[i].y-4, boxButtonData.width, boxButtonData.height, boxButton, false);
	}
	snprintf(textC2[2].text, sizeof(textC2[2].text),"%s", Lang::movesString.c_str());
	snprintf(textC2[3].text, sizeof(textC2[3].text),"%s", Lang::stats.c_str());
	snprintf(textC2[4].text, sizeof(textC2[4].text),"%s", Lang::origin.c_str());
	for(unsigned i=0;i<(sizeof(textC2)/sizeof(textC2[0]));i++) {
		printTextMaxW(textC2[i].text, 80, 1, textC2[i].x, textC2[i].y, false);
	}
}

std::shared_ptr<PKX> showPokemonSummary(std::shared_ptr<PKX> pkm) {
	// Draw the Pokémon's info
	drawSummaryPage(pkm);

	// Move arrow to first option
	setSpriteVisibility(bottomArrowID, true);
	setSpritePosition(bottomArrowID, textC1[0].x+getTextWidthMaxW(textC1[0].text, 80), textC1[0].y-6);
	updateOam();

	bool optionSelected = false;
	int held, pressed, selection = 0, column = 0;
	touchPosition touch;
	while(1) {
		do {
			swiWaitForVBlank();
			scanKeys();
			pressed = keysDown();
			held = keysDownRepeat();
		} while(!held);

		if(held & KEY_UP) {
			if(selection > 0)	selection--;
		} else if(held & KEY_DOWN) {
			if(selection < (int)(column == 0 ? (sizeof(textC1)/sizeof(textC1[0])) : (sizeof(textC2)/sizeof(textC2[0])))-1)	selection++;
		} else if(pressed & KEY_LEFT) {
			if(column > 0)	column--;
			selection = 0;
		} else if(held & KEY_RIGHT) {
			if(column < 1)	column++;
			selection = 0;
		} else if(pressed & KEY_A) {
			optionSelected = true;
		} else if(pressed & KEY_B) {
			Sound::play(Sound::back);
			pkm->refreshChecksum();
			return pkm;
		} else if(pressed & KEY_TOUCH) {
			touchRead(&touch);
			for(unsigned i=0;i<(sizeof(textC1)/sizeof(textC1[0]));i++) {
				if(touch.px >= textC1[i].x && touch.px <= textC1[i].x+getTextWidthMaxW(textC1[i].text, 80) && touch.py >= textC1[i].y && touch.py <= textC1[i].y+16) {
					column = 0;
					selection = i;
					optionSelected = true;
					break;
				}
			}
			if(touch.px >= textC2[0].x-15 && touch.px <= textC2[0].x && touch.py >= textC2[0].y && touch.py <= textC2[0].y+15) { // Ball
				column = 1;
				selection = 0;
				optionSelected = true;
			} else if(touch.px >= 146 && touch.py >= 16 && touch.py <= 80) { // Pokémon
				column = 1;
				selection = 1;
				optionSelected = true;
			}
			for(unsigned i=2;i<(sizeof(textC2)/sizeof(textC2[0]));i++) { // Buttons
				if(touch.px >= textC2[i].x-4 && touch.px <= textC2[i].x-4+boxButtonData.width && touch.py >= textC2[i].y-4 && touch.py <= textC2[i].y-4+boxButtonData.height) {
					column = 1;
					selection = i;
					optionSelected = true;
					break;
				}
			}
		}

		if(optionSelected) {
			Sound::play(Sound::click);
			optionSelected = false;
			setSpriteVisibility(bottomArrowID, false);
			updateOam();
			if(column == 0) {
				switch(selection) {
					case 0: {
						int num = Input::getInt(save->maxSpecies());
						if(num > 0) {
							pkm->species(num);
							if(!pkm->nicknamed())	pkm->nickname(Lang::species[num]);
							pkm->setAbility(0);
							pkm->alternativeForm(0);
						}
						break;
					} case 1: {
						std::string name = Input::getLine(10);
						if(name != "") {
							pkm->nickname(name);
							pkm->nicknamed(name != Lang::species[pkm->species()]);
						}
						if(pkm->gender() != 2) {
							pkm->gender(Input::getBool(Lang::female, Lang::male));
							pkm->PID(PKX::getRandomPID(pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(), pkm->abilityNumber(), pkm->PID(), pkm->generation()));
						}
						break;
					} case 2: {
						int num = Input::getInt(100);
						if(num > 0)	pkm->level(num);
						break;
					} case 3: {
						changeAbility(pkm);
						break;
					} case 4: {
						int num = selectNature(pkm->nature());
						if(num != -1)	pkm->nature(num);
						break;
					} case 5: {
						pkm->shiny(!pkm->shiny());
						break;
					} case 6: {
						pkm->pkrs(pkm->pkrs() ? 0 : 0xF4);
						break;
					} case 7: {
						std::string name = Input::getLine(7);
						if(name != "")	pkm->otName(name);
						pkm->otGender(Input::getBool(Lang::female, Lang::male));
						break;
					} case 8: {
						int num = Input::getInt(65535);
						if(num != -1)	pkm->TID(num);
						break;
					} case 9: {
						int num = Input::getInt(65535);
						if(num != -1)	pkm->SID(num);
						break;
					} case 10: {
						int num = Input::getInt(255);
						if(num != -1)	pkm->otFriendship(num);
						break;
					}
				}
			} else {
				switch(selection) {
					case 0: {
						int num = selectPokeball(pkm->ball());
						if(num > 0)	pkm->ball(num);
						break;
					} case 1: {
						int num = selectForm(pkm->species(), pkm->alternativeForm());
						if(num != -1)	pkm->alternativeForm(num);
						break;
					} case 2: {
						selectMoves(pkm);
						break;
					} case 3: {
						selectStats(pkm);
						break;
					} case 4: {
						selectOrigin(pkm);
					}
				}
			}
			drawSummaryPage(pkm);
			setSpriteVisibility(bottomArrowID, true);
		}

		if(column == 0) {
			setSpritePosition(bottomArrowID, textC1[selection].x+getTextWidthMaxW(textC1[selection].text, 80), textC1[selection].y-6);
		} else {
			setSpritePosition(bottomArrowID, textC2[selection].x+getTextWidthMaxW(textC2[selection].text, 80), textC2[selection].y-6);
		}
		updateOam();
	}
}