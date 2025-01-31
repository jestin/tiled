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

#pragma once

#include "mapformat.h"

#include "vera_global.h"

namespace Vera {

class VERASHARED_EXPORT VeraPlugin : public Tiled::WritableMapFormat
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.mapeditor.MapFormat" FILE "plugin.json")

public:
    VeraPlugin();

    bool write(const Tiled::Map *map, const QString &fileName, Options options) override;
    QString errorString() const override;
    QString shortName() const override;

protected:
    QString nameFilter() const override;

private:
    QString mError;

	bool writeLayerToFile(const Tiled::Layer* layer, const int layerNum, const QString& fileName);
};

} // namespace Vera
