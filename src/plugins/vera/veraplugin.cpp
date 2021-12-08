/*
 * VERA Tiled Plugin
 * Copyright 2016, Jones Blunt <mrjonesblunt@gmail.com>
 * Copyright 2016-2020, Thorbjørn Lindeijer <bjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "veraplugin.h"

#include "logginginterface.h"
#include "map.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "savefile.h"
#include "tile.h"
#include "tilelayer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDataStream>

#include "qtcompat_p.h"

using namespace Tiled;
using namespace Vera;

VeraPlugin::VeraPlugin()
{
}

bool VeraPlugin::write(const Map *map, const QString &fileName, Options options)
{
	// Write out tile instances
	LayerIterator iterator(map);

	// only write out the first layer
	const Layer *layer = iterator.next();

	if (!layer) {
		mError = QCoreApplication::translate("File Errors", "No layers found.");
		return false;
	}

	if (layer->layerType() != Layer::TileLayerType) {
		mError = QCoreApplication::translate("File Errors", "Vera files require that layer 0 must be a tile layer.");
		return false;
	}

	// Now that we know this is a valid map, we can open the save file

	SaveFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		mError = QCoreApplication::translate("File Errors", "Could not open file for writing.");
		return false;
	}

	QDataStream stream(file.device());
	stream.setByteOrder(QDataStream::LittleEndian);

	auto tileLayer = static_cast<const TileLayer*>(layer);

	qint32 width = tileLayer->width();
	qint32 height = tileLayer->height();

	if (width != 32 && width != 64 && width != 64 && width != 128 && width != 256) {
		mError = QCoreApplication::translate("File Errors", "Vera maps must be 32/64/128/256 tiles wide");
		return false;
	}

	if (height != 32 && height != 64 && height != 64 && height != 128 && height != 256) {
		mError = QCoreApplication::translate("File Errors", "Vera maps must be 32/64/128/256 tiles high");
		return false;
	}

	// write out 2 byte header
	 stream << (quint8)0 << (quint8)0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const Cell &cell = tileLayer->cellAt(x, y);
			if (const Tile *tile = cell.tile()) {
				// write out two bytes

				// first byte: tile index (7:0)
				stream << (quint8)cell.tileId();

				// second byte: palette offset (4 bits), vflip (1 bit), hflip (1 bit), and tile index (9:8) (2 bits)

				// start off by putting bits 9:8 of the index in the lower two bits
				quint8 secondByte = cell.tileId() >> 8;

				// OR a 1 into bit 2 if flipped horizontally
				if (cell.flippedHorizontally()) {
					secondByte |= 0b00000100;
				}

				// OR a 1 into bit 3 if flipped vertically
				if (cell.flippedVertically()) {
					secondByte |= 0b00001000;
				}

				stream << secondByte;

			} else {
				// write out two bytes of zero, as we assume that a VERA tileset has the "empty" tile in index 0
				stream << (quint8)0;
				stream << (quint8)0;
			}
		}
	}

	if (!file.commit()) {
		mError = file.errorString();
		return false;
	}

	return true;
}

QString VeraPlugin::errorString() const
{
    return mError;
}

QString VeraPlugin::nameFilter() const
{
    return tr("VERA tile map files (*.BIN)");
}

QString VeraPlugin::shortName() const
{
    return QStringLiteral("vera");
}
