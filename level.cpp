/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#include "level.h"
#include "game.h"
#include "objects.h"

Level::Level(Game *game, int world, int level, int area)
{
    this->game = game;

    this->world = world;
    this->level = level;

    QString arcpath = QString("/Course/%1-%2.sarc").arg(world).arg(level);
    qDebug(arcpath.toStdString().c_str());
    archive = new SarcFilesystem(game->fs->openFile(arcpath));
    this->area = area;


    QString headerfile = QString("/course/course%1.bin").arg(area);
    if (!archive->fileExists(headerfile))
        throw std::runtime_error("Level has no header!!");

    // read header blocks
    FileBase* header = archive->openFile(headerfile);
    header->open();
    header->seek(0);

    quint32 blockOffsets[17];
    quint32 blockSizes[17];
    for (int b = 0; b < 17; b++)
    {
        blockOffsets[b] = header->read32();
        blockSizes[b] = header->read32();
    }

    // Block 1: Tilesets
    for (int t = 0; t < 4; t++)
    {
        header->seek(blockOffsets[0] + (t*32));

        QString tilesetname;
        header->readStringASCII(tilesetname, 32);

        if (tilesetname.isEmpty())
        {
            tilesets[t] = NULL;
            continue;
        }

        try
        {
            tilesets[t] = game->getTileset(tilesetname);
        }
        catch ( const std::exception & e )
        {
            qDebug("Tileset %s not found", tilesetname.toStdString().c_str());
            tilesets[t] = NULL;
        }
    }
    // Block 2: Area Options

    // Block 7: Entrances
    header->seek(blockOffsets[6]);
    for (int e = 0; e < (int)(blockSizes[6]/24); e++)
    {
        Entrance* entr = new Entrance(header->read16(), header->read16(), e);
        qDebug("Found Entrance with x: %d, y: %d", entr->getx(), entr->gety());
        entrances.append(*entr);

        header->skip(20); // data we don't care about right now (id is in here, too. For now simply e)
    }

    // Block 8: Sprites
    header->seek(blockOffsets[7]);
    for (;;)
    {
        quint16 id = header->read16();
        if (id == 0xFFFF) break;

        Sprite* spr = new Sprite(header->read16(), header->read16(), id);
        qDebug("Found Sprite with ID %d, x: %d, y: %d", spr->getid(), spr->getx(), spr->gety());
        sprites.append(*spr);

        header->skip(18); // Spritedata
    }

    // Block 10: Zones
    header->seek(blockOffsets[9]);
    for (int z = 0; z < (int)(blockSizes[9]/24); z++)
    {
        Zone* zone = new Zone(header->read16(), header->read16(), header->read16(), header->read16(), z);
        qDebug("Found Zone with x: %d, y: %d, width: %d, height: %d", zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());
        zones.append(*zone);

        header->skip(20); // data we don't care about right now (id is in here, too. For now simply z)
    }

    // Block 11: Locations
    // Block 12/13: Paths
    // Block: 16/17. Progess Paths


    header->close();
    delete header;


    // read bgdat
    QString bgdatfiletemp = QString("/course/course%1_bgdatL%2.bin").arg(area);
    for (int l = 0; l < 2; l++)
    {
        QString bgdatfile = bgdatfiletemp.arg(l+1);
        if (!archive->fileExists(bgdatfile)) continue;

        FileBase* bgdat = archive->openFile(bgdatfile);
        bgdat->open();
        bgdat->seek(0);
        for (;;)
        {
            if (bgdat->pos() >= bgdat->size()) break;
            quint16 id = bgdat->read16();
            if (id == 0xFFFF) break;

            BgdatObject* obj = new BgdatObject(bgdat->read16(), bgdat->read16(), bgdat->read16(), bgdat->read16(), id);
            objects[l].append(*obj);

            bgdat->skip(6);
        }
        bgdat->close();
        delete bgdat;
    }
}

Level::~Level()
{
    for (int t = 0; t < 4; t++)
    {
        if (tilesets[t])
            delete tilesets[t];
    }

    /*for (int l = 0; l < 3; l++)
    {
        for (int o = 0; o < objects[l].size(); o++)
            //
    }*/

    delete archive;
}

