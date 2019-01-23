/*
*   This file is part of PKSM
*   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "SpeciesSelectionScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "Configuration.hpp"
#include "ClickButton.hpp"

SpeciesSelectionScreen::SpeciesSelectionScreen(std::shared_ptr<PKX> pkm) : SelectionScreen(pkm), hid(40, 8)
{
    searchButton = new ClickButton(75, 30, 170, 23, [this](){ Gui::setNextKeyboardFunc([this](){ this->searchBar(); }); return false; }, ui_sheet_emulated_box_search_idx, "", 0, 0);
    if (TitleLoader::save->generation() != Generation::LGPE)
    {
        for (int i = 1; i <= TitleLoader::save->maxSpecies(); i++)
        {
            dispPkm.push_back(i);
        }
        hid.update(dispPkm.size());
        hid.select(pkm->species() == 0 ? 0 : pkm->species() - 1);
    }
    else
    {
        for (size_t i = 1; i <= 151; i++)
        {
            dispPkm.push_back(i);
        }
        dispPkm.push_back(808);
        dispPkm.push_back(809);
        hid.update(dispPkm.size());
        if (pkm->species() == 808 || pkm->species() == 809)
        {
            hid.select(pkm->species() - 657);
        }
        else
        {
            hid.select(pkm->species() == 0 ? 0 : pkm->species() - 1);
        }
    }
}

void SpeciesSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    searchButton->draw();
    Gui::sprite(ui_sheet_icon_search_idx, 79, 33);
    Gui::dynamicText(searchString, 95, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);

    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);

    int x = (hid.index() % 8) * 50;
    int y = (hid.index() / 8) * 48;
    // Selector
    if (dispPkm.size() > 0)
    {
        C2D_DrawRectSolid(x, y, 0.5f, 49, 47, COLOR_MASKBLACK);
        C2D_DrawRectSolid(x, y, 0.5f, 49, 1, COLOR_YELLOW);
        C2D_DrawRectSolid(x, y, 0.5f, 1, 47, COLOR_YELLOW);
        C2D_DrawRectSolid(x + 48, y, 0.5f, 1, 47, COLOR_YELLOW);
        C2D_DrawRectSolid(x, y + 46, 0.5f, 49, 1, COLOR_YELLOW);
    }

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            size_t pkmIndex = hid.page() * hid.maxVisibleEntries() + x + y * 8;
            if (pkmIndex >= dispPkm.size())
            {
                break;
            }
            size_t species = dispPkm[pkmIndex];
            Gui::pkm(species, 0, TitleLoader::save->generation(), x * 50 + 7, y * 48 + 2);
            Gui::dynamicText(x * 50, y * 48 + 34, 50, std::to_string(species), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
        }
    }
}

void SpeciesSelectionScreen::update(touchPosition* touch)
{
    if (justSwitched && ((hidKeysHeld() | hidKeysDown()) & KEY_TOUCH))
    {
        return;
    }
    else if (justSwitched)
    {
        justSwitched = false;
    }

    if (hidKeysDown() & KEY_X)
    {
        Gui::setNextKeyboardFunc([this](){ this->searchBar(); });
    }
    searchButton->update(touch);
    if (!searchString.empty() && searchString != oldSearchString)
    {
        dispPkm.clear();
        if (TitleLoader::save->generation() != Generation::LGPE)
        {
            for (int i = 1; i <= TitleLoader::save->maxSpecies(); i++)
            {
                std::string speciesName = i18n::species(Configuration::getInstance().language(), i).substr(0, searchString.size());
                StringUtils::toLower(speciesName);
                if (speciesName == searchString)
                {
                    dispPkm.push_back(i);
                }
            }
        }
        else
        {
            std::string speciesName;
            for (size_t i = 1; i <= 151; i++)
            {
                speciesName = i18n::species(Configuration::getInstance().language(), i).substr(0, searchString.size());
                StringUtils::toLower(speciesName);
                if (speciesName == searchString)
                {
                    dispPkm.push_back(i);
                }
            }
            speciesName = i18n::species(Configuration::getInstance().language(), 808).substr(0, searchString.size());
            StringUtils::toLower(speciesName);
            if (speciesName == searchString)
            {
                dispPkm.push_back(808);
            }
            speciesName = i18n::species(Configuration::getInstance().language(), 809).substr(0, searchString.size());
            StringUtils::toLower(speciesName);
            if (speciesName == searchString)
            {
                dispPkm.push_back(809);
            }
        }
        oldSearchString = searchString;
    }
    else if (searchString.empty() && !oldSearchString.empty())
    {
        dispPkm.clear();
        if (TitleLoader::save->generation() != Generation::LGPE)
        {
            for (int i = 1; i <= TitleLoader::save->maxSpecies(); i++)
            {
                dispPkm.push_back(i);
            }
            hid.update(dispPkm.size());
            hid.select(pkm->species() == 0 ? 0 : pkm->species() - 1);
        }
        else
        {
            for (size_t i = 1; i <= 151; i++)
            {
                dispPkm.push_back(i);
            }
            dispPkm.push_back(808);
            dispPkm.push_back(809);
            hid.update(dispPkm.size());
            if (pkm->species() == 808 || pkm->species() == 809)
            {
                hid.select(pkm->species() - 657);
            }
            else
            {
                hid.select(pkm->species() == 0 ? 0 : pkm->species() - 1);
            }
        }

        oldSearchString = searchString = "";
    }
    if (hid.fullIndex() >= dispPkm.size())
    {
        hid.select(0);
    }
    hid.update(dispPkm.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A && dispPkm.size() > 0)
    {
        int species = dispPkm[hid.fullIndex()];
        if (pkm->species() == 0 || !pkm->nicknamed())
        {
            pkm->nickname(i18n::species(Configuration::getInstance().language(), species).c_str());
        }
        pkm->species((u16) species);
        pkm->alternativeForm(0);
        pkm->setAbility(0);
        pkm->PID(PKX::getRandomPID(pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(), pkm->abilityNumber(), pkm->PID(), pkm->generation()));
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        done = true;
        return;
    }
}

void SpeciesSelectionScreen::searchBar()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
    swkbdSetHintText(&state, i18n::localize("SPECIES").c_str());
    swkbdSetValidation(&state, SWKBD_ANYTHING, 0, 0);
    char input[25] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        searchString = input;
        StringUtils::toLower(searchString);
    }
}