#include "summary.hpp"
#include "colors.hpp"
#include "config.hpp"
#include "flashcard.hpp"
#include "lang.hpp"
#include "loader.hpp"
#include "input.hpp"
#include "manager.hpp"
#include "misc.hpp"
#include "PK5.hpp"
#include "sound.hpp"

struct Text {
	int x;
	int y;
	char text[32];
};

Text textC1[] {
	{64, 0},
	{64, 16},
	{64, 32},
	{64, 48},
	{64, 64},
	{64, 80},
	{64, 96},
	{64, 112},
	{96, 128},
	{96, 144},
	{96, 160},
	{96, 176},
};

Text textC2[] = {
	{164, 8},
	{230, 30},
	{170, 102},
	{170, 132},
	{170, 162},
};

int summaryPage = 0;
std::vector<std::string> summaryLabels = {"species", "name", "level", "ability", "nature", "item", "shiny", "pokerus", "origTrainer", "trainerID", "secretID", "friendship"};

std::pair<int, int> getPokeballPosition(u8 ball) {
	if(ball > 25)	return {0, 0};
	std::pair<int, int> xy;
	xy.second = (ball/9)*15;
	xy.first = (ball-((ball/9)*9))*15;
	return xy;
}

void changeAbility(std::shared_ptr<PKX> &pkm) {
	if(pkm->generation() == Generation::FOUR) {
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

void drawSummaryPage(std::shared_ptr<PKX> pkm, bool background) {
	// Hide sprites
	for(int i=0;i<30;i++) {
		setSpriteVisibility(i, false, false);
	}
	updateOam();

	if(background) {
		// Draw background
		drawImageDMA(0, 0, listBg, false, false);
		drawImageScaled(145, 1, 1.2, 1, infoBox, false, false);
		// Draw lines
		drawOutline(0, 128, 160, 65, LIGHT_GRAY, false, false);
		// Draw Pokémon
		Image image = loadPokemonSprite(getPokemonIndex(pkm));
		drawImageScaled(169, 22, 2, 2, image, false, false, 0xC0);
		// Draw Poké ball
		std::pair<int, int> xy = getPokeballPosition(pkm->ball());
		drawImageSegment(148, 8, 15, 15, ballSheet, xy.first, xy.second, false, false);
		// Draw types
		int type = (pkm->generation() == Generation::FOUR && pkm->type1() > 8) ? pkm->type1()-1 : pkm->type1();
		drawImage(150, 25-((types[type].height-12)/2), types[type], false, false);
		if(pkm->type1() != pkm->type2()) {
			type = (pkm->generation() == Generation::FOUR && pkm->type2() > 8) ? pkm->type2()-1 : pkm->type2();
			drawImage(186, 25-((types[type].height-12)/2), types[type], false, false, 4);
		}
	}
	drawRectangle(0, 0, 256, 192, 0, false, true);

	// Print Pokémon name
	printTextTintedMaxW(Lang::species[pkm->species()], 90, 1, (pkm->gender() ? (pkm->gender() == 1 ? RED_TEXT : GRAY_TEXT) : BLUE_TEXT), 165, 8, false, true);

	// Draw/clear shiny star
	if(pkm->shiny())	drawImage(150, 45, shiny, false, true);
	else	drawRectangle(150, 45, 8, 8, 0, false, true);

	// Print Pokémon and trainer info labels
	for(unsigned i=0;i<summaryLabels.size();i++) {
		printTextMaxW(Lang::get(summaryLabels[i]), textC1[i].x-8, 1, 4, textC1[i].y, false, true);
	}

	// Print Pokémon and trainer info
	snprintf(textC1[0].text,  sizeof(textC1[0].text), "%.3i", pkm->species());
	if(pkm->nicknamed())	snprintf(textC1[1].text,  sizeof(textC1[1].text), "%s", pkm->nickname().c_str());
	else	snprintf(textC1[1].text,  sizeof(textC1[1].text), "%s", Lang::species[pkm->species()].c_str());
	snprintf(textC1[2].text,  sizeof(textC1[2].text), "%i", pkm->level());
	snprintf(textC1[3].text,  sizeof(textC1[3].text), "%s", Lang::abilities[pkm->ability()].c_str());
	snprintf(textC1[4].text,  sizeof(textC1[4].text), "%s", Lang::natures[pkm->nature()].c_str());
	snprintf(textC1[5].text,  sizeof(textC1[5].text), "%s", Lang::items[pkm->heldItem()].c_str());
	snprintf(textC1[6].text,  sizeof(textC1[6].text), "%s", pkm->shiny() ? Lang::get("yes").c_str() : Lang::get("no").c_str());
	snprintf(textC1[7].text,  sizeof(textC1[7].text), "%s", pkm->pkrs() ? Lang::get("yes").c_str() : Lang::get("no").c_str());
	snprintf(textC1[8].text,  sizeof(textC1[8].text), "%s", pkm->otName().c_str());
	for(unsigned i=0;i<(sizeof(textC1)/sizeof(textC1[0]));i++) {
	snprintf(textC1[9].text,  sizeof(textC1[9].text), "%.5i", pkm->TID());
	snprintf(textC1[10].text, sizeof(textC1[10].text),"%.5i", pkm->SID());
	snprintf(textC1[11].text, sizeof(textC1[11].text),"%i", pkm->otFriendship());
		// OT Name is colored
		if(i != 8)	printTextMaxW(textC1[i].text, 80, 1, textC1[i].x, textC1[i].y, false, true);
	}
	printTextTinted(textC1[8].text, (pkm->otGender() ? RED_TEXT : BLUE_TEXT), textC1[8].x, textC1[8].y, false, true);

	// Draw buttons // The first 2 don't have buttons
	for(unsigned i=2;i<sizeof(textC2)/sizeof(textC2[0]);i++) {
		drawImage(textC2[i].x-4, textC2[i].y-4, boxButton, false, true);
	}
	snprintf(textC2[2].text, sizeof(textC2[2].text),"%s", Lang::get("moves").c_str());
	snprintf(textC2[3].text, sizeof(textC2[3].text),"%s", Lang::get("stats").c_str());
	snprintf(textC2[4].text, sizeof(textC2[4].text),"%s", Lang::get("origin").c_str());
	for(unsigned i=0;i<(sizeof(textC2)/sizeof(textC2[0]));i++) {
		printTextMaxW(textC2[i].text, 80, 1, textC2[i].x, textC2[i].y, false, true);
	}
}

std::shared_ptr<PKX> showPokemonSummary(std::shared_ptr<PKX> pkm) {
	// Draw the Pokémon's info
	drawSummaryPage(pkm, true);

	// Move arrow to first option
	setSpriteVisibility(arrowID, false, true);
	setSpritePosition(arrowID, false, textC1[0].x+getTextWidthMaxW(textC1[0].text, 80), textC1[0].y-6);
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
				if(touch.px >= textC2[i].x-4 && touch.px <= textC2[i].x-4+boxButton.width && touch.py >= textC2[i].y-4 && touch.py <= textC2[i].y-4+boxButton.height) {
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
			setSpriteVisibility(arrowID, false, false);
			updateOam();
			if(column == 0) {
				switch(selection) {
					case 0: {
						int num = selectItem(pkm->species(), 1, save->maxSpecies()+1, Lang::species);
						if(num > 0) {
							pkm->species(num);
							if(!pkm->nicknamed())	pkm->nickname(Lang::species[num]);
							pkm->setAbility(0);
							pkm->alternativeForm(0);
							if(pkm->genderType() == 255)	pkm->gender(2);
							else if(pkm->genderType() == 0)	pkm->gender(0);
							else if(pkm->genderType() == 254)	pkm->gender(1);
							else if(pkm->gender() == 2)	pkm->gender(0);
						}
						drawSummaryPage(pkm, true);
						break;
					} case 1: {
						std::string name = Input::getLine(10);
						if(name != "") {
							pkm->nickname(name);
							pkm->nicknamed(name != Lang::species[pkm->species()]);
						}
						if(pkm->genderType() == 255)	pkm->gender(2);
						else if(pkm->genderType() == 0)	pkm->gender(0);
						else if(pkm->genderType() == 254)	pkm->gender(1);
						else {
							drawSummaryPage(pkm, false);
							pkm->gender(Input::getBool(Lang::get("female"), Lang::get("male")));
							pkm->PID(PKX::getRandomPID(pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(), pkm->abilityNumber(), pkm->PID(), pkm->generation()));
						}
						drawSummaryPage(pkm, false);
						break;
					} case 2: {
						int num = Input::getInt(100);
						if(num > 0)	pkm->level(num);
						drawSummaryPage(pkm, false);
						break;
					} case 3: {
						changeAbility(pkm);
						drawSummaryPage(pkm, false);
						break;
					} case 4: {
						int num = selectNature(pkm->nature());
						if(num != -1)	pkm->nature(num);
						drawSummaryPage(pkm, true);
						break;
					} case 5: {
						int num = selectItem(pkm->heldItem(), 0, save->maxItem()+1, Lang::items);
						if(num != -1)	pkm->heldItem(num);
						drawSummaryPage(pkm, true);
						break;
					} case 6: {
						pkm->shiny(!pkm->shiny());
						drawSummaryPage(pkm, false);
						break;
					} case 7: {
						pkm->pkrs(pkm->pkrs() ? 0 : 0xF4);
						drawSummaryPage(pkm, false);
						break;
					} case 8: {
						std::string name = Input::getLine(7);
						if(name != "")	pkm->otName(name);
						drawSummaryPage(pkm, false);
						pkm->otGender(Input::getBool(Lang::get("female"), Lang::get("male")));
						drawSummaryPage(pkm, false);
						break;
					} case 9: {
						int num = Input::getInt(65535);
						if(num != -1)	pkm->TID(num);
						drawSummaryPage(pkm, false);
						break;
					} case 10: {
						int num = Input::getInt(65535);
						if(num != -1)	pkm->SID(num);
						drawSummaryPage(pkm, false);
						break;
					} case 11: {
						int num = Input::getInt(255);
						if(num != -1)	pkm->otFriendship(num);
						drawSummaryPage(pkm, false);
						break;
					}
				}
			} else {
				switch(selection) {
					case 0: {
						int num = selectPokeball(pkm->ball());
						if(num > 0)	pkm->ball(num);
						drawSummaryPage(pkm, true);
						break;
					} case 1: {
						int num = selectForm(pkm->species(), pkm->alternativeForm());
						if(num != -1)	pkm->alternativeForm(num);
						drawSummaryPage(pkm, false);
						break;
					} case 2: {
						selectMoves(pkm);
						drawSummaryPage(pkm, true);
						break;
					} case 3: {
						selectStats(pkm);
						drawSummaryPage(pkm, true);
						break;
					} case 4: {
						selectOrigin(pkm);
						drawSummaryPage(pkm, true);
					}
				}
			}
			setSpriteVisibility(arrowID, false, true);
		}

		if(column == 0) {
			setSpritePosition(arrowID, false, textC1[selection].x+getTextWidthMaxW(textC1[selection].text, 80), textC1[selection].y-6);
		} else {
			setSpritePosition(arrowID, false, textC2[selection].x+getTextWidthMaxW(textC2[selection].text, 80), textC2[selection].y-6);
		}
		updateOam();
	}
}