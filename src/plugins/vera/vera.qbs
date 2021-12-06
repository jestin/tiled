import qbs 1.0

TiledPlugin {
    cpp.defines: base.concat(["VERA_LIBRARY"])

    files: [
        "vera_global.h",
        "veraplugin.cpp",
        "veraplugin.h",
        "plugin.json",
    ]
}
