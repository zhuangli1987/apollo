QT.webkit.VERSION = 5.5.1
QT.webkit.MAJOR_VERSION = 5
QT.webkit.MINOR_VERSION = 5
QT.webkit.PATCH_VERSION = 1
QT.webkit.name = QtWebKit
QT.webkit.libs = $$QT_MODULE_LIB_BASE
QT.webkit.rpath = /home/work/baidu/adu-lab/cybertron-apollo/third-party/Qt5.5.1/5.5/gcc_64/lib
QT.webkit.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtWebKit
QT.webkit.bins = $$QT_MODULE_BIN_BASE
QT.webkit.libexecs = $$QT_MODULE_LIBEXEC_BASE
QT.webkit.plugins = $$QT_MODULE_PLUGIN_BASE
QT.webkit.imports = $$QT_MODULE_IMPORT_BASE
QT.webkit.qml = $$QT_MODULE_QML_BASE
QT.webkit.depends = core gui network
QT.webkit.run_depends = sensors positioning qml quick webchannel sql core_private gui_private
QT.webkit.module_config =
QT.webkit.DEFINES = QT_WEBKIT_LIB
QT_MODULES += webkit
