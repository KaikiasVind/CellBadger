QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AnalysisTab.cpp \
    BioModels/Feature.cpp \
    BioModels/FeatureCollection.cpp \
    ExportDialog.cpp \
    StartDialog.cpp \
    Statistics/Correlator.cpp \
    Statistics/Expressioncomparator.cpp \
    System/ConfigFile.cpp \
    System/Coordinator.cpp \
    System/InformationCenter.cpp \
    TSNEDialog.cpp \
    TabWidget.cpp \
    Utils/FileOperators/CSVReader.cpp \
    Utils/FileOperators/ConfigFileOperator.cpp \
    Utils/Helper.cpp \
    Utils/Math.cpp \
    Utils/Models/AnalysisConfigModel.cpp \
    Utils/Models/GeneTableModel.cpp \
    Utils/Models/ProxyModel.cpp \
    Utils/Plots.cpp \
    Utils/Sorter.cpp \
    main.cpp \
    Mainwindow.cpp

HEADERS += \
    AnalysisTab.h \
    BioModels/Feature.h \
    BioModels/FeatureCollection.h \
    ExportDialog.h \
    Mainwindow.h \
    StartDialog.h \
    Statistics/Correlator.h \
    Statistics/Expressioncomparator.h \
    System/ConfigFile.h \
    System/Coordinator.h \
    System/InformationCenter.h \
    TSNEDialog.h \
    TabWidget.h \
    Utils/Definitions.h \
    Utils/FileOperators/CSVReader.h \
    Utils/FileOperators/ConfigFileOperator.h \
    Utils/Helper.h \
    Utils/Math.h \
    Utils/Models/AnalysisConfigModel.h \
    Utils/Models/GeneTableModel.h \
    Utils/Models/ProxyModel.h \
    Utils/Plots.h \
    Utils/Sorter.h

FORMS += \
    AnalysisTab.ui \
    ExportDialog.ui \
    Mainwindow.ui \
    StartDialog.ui \
    TSNEDialog.ui \
    TabWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
