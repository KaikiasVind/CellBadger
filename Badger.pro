QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11 console

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
    BioModels/Celltype.cpp \
    BioModels/Feature.cpp \
    BioModels/FeatureCollection.cpp \
    ExportDialog.cpp \
    Graphics/qcustomplot.cpp \
    StartDialog.cpp \
    Statistics/Correlator.cpp \
    Statistics/Expressioncomparator.cpp \
    System/ConfigFile.cpp \
    System/Coordinator.cpp \
    System/InformationCenter.cpp \
    TabWidget.cpp \
    Test.cpp \
    Utils/FileOperators/CSVReader.cpp \
    Utils/FileOperators/ConfigFileOperator.cpp \
    Utils/Helper.cpp \
    Utils/Math.cpp \
    Utils/Plots.cpp \
    Utils/Sorter.cpp \
    main.cpp \
    Mainwindow.cpp

HEADERS += \
    BioModels/Celltype.h \
    BioModels/Feature.h \
    BioModels/FeatureCollection.h \
    ExportDialog.h \
    Graphics/qcustomplot.h \
    Mainwindow.h \
    StartDialog.h \
    Statistics/Correlator.h \
    Statistics/Expressioncomparator.h \
    System/ConfigFile.h \
    System/Coordinator.h \
    System/InformationCenter.h \
    TabWidget.h \
    Test.h \
    Utils/FileOperators/CSVReader.h \
    Utils/FileOperators/ConfigFileOperator.h \
    Utils/Helper.h \
    Utils/Math.h \
    Utils/Plots.h \
    Utils/Sorter.h

FORMS += \
    ExportDialog.ui \
    Mainwindow.ui \
    StartDialog.ui \
    TabWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
