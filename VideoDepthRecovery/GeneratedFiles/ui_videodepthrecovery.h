/********************************************************************************
** Form generated from reading UI file 'videodepthrecovery.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEODEPTHRECOVERY_H
#define UI_VIDEODEPTHRECOVERY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VideoDepthRecoveryClass
{
public:
    QAction *actionOpen;
    QAction *actionSaveAs;
    QAction *actionExit;
    QAction *actionDepthParameters;
    QAction *actionRun_Initialization;
    QAction *actionRun_BundleOptimization;
    QAction *actionRun_DepthExpansion;
    QAction *actionLoadProject;
    QAction *actionRun_VisualHull;
    QAction *actionLS_Parameters;
    QAction *actionRun_Init;
    QAction *actionLSRun_Init;
    QAction *actionLSRun_Refine;
    QAction *actionLSRun_Mesh;
    QAction *actionLSRun_DE;
    QAction *actionLSRun_All;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuParameters;
    QMenu *menuLightstage;
    QMenu *menuVideoDepth;
    QMenu *menuAbout;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidget_DepthPara;
    QWidget *dockWidgetContents;

    void setupUi(QMainWindow *VideoDepthRecoveryClass)
    {
        if (VideoDepthRecoveryClass->objectName().isEmpty())
            VideoDepthRecoveryClass->setObjectName(QString::fromUtf8("VideoDepthRecoveryClass"));
        VideoDepthRecoveryClass->resize(725, 483);
        actionOpen = new QAction(VideoDepthRecoveryClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionSaveAs = new QAction(VideoDepthRecoveryClass);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        actionExit = new QAction(VideoDepthRecoveryClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionDepthParameters = new QAction(VideoDepthRecoveryClass);
        actionDepthParameters->setObjectName(QString::fromUtf8("actionDepthParameters"));
        actionRun_Initialization = new QAction(VideoDepthRecoveryClass);
        actionRun_Initialization->setObjectName(QString::fromUtf8("actionRun_Initialization"));
        actionRun_BundleOptimization = new QAction(VideoDepthRecoveryClass);
        actionRun_BundleOptimization->setObjectName(QString::fromUtf8("actionRun_BundleOptimization"));
        actionRun_DepthExpansion = new QAction(VideoDepthRecoveryClass);
        actionRun_DepthExpansion->setObjectName(QString::fromUtf8("actionRun_DepthExpansion"));
        actionLoadProject = new QAction(VideoDepthRecoveryClass);
        actionLoadProject->setObjectName(QString::fromUtf8("actionLoadProject"));
        actionRun_VisualHull = new QAction(VideoDepthRecoveryClass);
        actionRun_VisualHull->setObjectName(QString::fromUtf8("actionRun_VisualHull"));
        actionLS_Parameters = new QAction(VideoDepthRecoveryClass);
        actionLS_Parameters->setObjectName(QString::fromUtf8("actionLS_Parameters"));
        actionRun_Init = new QAction(VideoDepthRecoveryClass);
        actionRun_Init->setObjectName(QString::fromUtf8("actionRun_Init"));
        actionLSRun_Init = new QAction(VideoDepthRecoveryClass);
        actionLSRun_Init->setObjectName(QString::fromUtf8("actionLSRun_Init"));
        actionLSRun_Refine = new QAction(VideoDepthRecoveryClass);
        actionLSRun_Refine->setObjectName(QString::fromUtf8("actionLSRun_Refine"));
        actionLSRun_Mesh = new QAction(VideoDepthRecoveryClass);
        actionLSRun_Mesh->setObjectName(QString::fromUtf8("actionLSRun_Mesh"));
        actionLSRun_DE = new QAction(VideoDepthRecoveryClass);
        actionLSRun_DE->setObjectName(QString::fromUtf8("actionLSRun_DE"));
        actionLSRun_All = new QAction(VideoDepthRecoveryClass);
        actionLSRun_All->setObjectName(QString::fromUtf8("actionLSRun_All"));
        centralWidget = new QWidget(VideoDepthRecoveryClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        VideoDepthRecoveryClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(VideoDepthRecoveryClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 725, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuParameters = new QMenu(menuBar);
        menuParameters->setObjectName(QString::fromUtf8("menuParameters"));
        menuLightstage = new QMenu(menuBar);
        menuLightstage->setObjectName(QString::fromUtf8("menuLightstage"));
        menuVideoDepth = new QMenu(menuBar);
        menuVideoDepth->setObjectName(QString::fromUtf8("menuVideoDepth"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        VideoDepthRecoveryClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(VideoDepthRecoveryClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        VideoDepthRecoveryClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(VideoDepthRecoveryClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        VideoDepthRecoveryClass->setStatusBar(statusBar);
        dockWidget_DepthPara = new QDockWidget(VideoDepthRecoveryClass);
        dockWidget_DepthPara->setObjectName(QString::fromUtf8("dockWidget_DepthPara"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        dockWidget_DepthPara->setWidget(dockWidgetContents);
        VideoDepthRecoveryClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget_DepthPara);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuParameters->menuAction());
        menuBar->addAction(menuLightstage->menuAction());
        menuBar->addAction(menuVideoDepth->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSaveAs);
        menuFile->addAction(actionExit);
        menuParameters->addAction(actionDepthParameters);
        menuParameters->addAction(actionLS_Parameters);
        menuLightstage->addAction(actionLoadProject);
        menuLightstage->addAction(actionRun_VisualHull);
        menuLightstage->addAction(actionLSRun_Init);
        menuLightstage->addAction(actionLSRun_Refine);
        menuLightstage->addAction(actionLSRun_DE);
        menuLightstage->addAction(actionLSRun_Mesh);
        menuLightstage->addAction(actionLSRun_All);
        menuVideoDepth->addAction(actionRun_Initialization);
        menuVideoDepth->addAction(actionRun_BundleOptimization);
        menuVideoDepth->addAction(actionRun_DepthExpansion);

        retranslateUi(VideoDepthRecoveryClass);

        QMetaObject::connectSlotsByName(VideoDepthRecoveryClass);
    } // setupUi

    void retranslateUi(QMainWindow *VideoDepthRecoveryClass)
    {
        VideoDepthRecoveryClass->setWindowTitle(QApplication::translate("VideoDepthRecoveryClass", "VideoDepthRecovery", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("VideoDepthRecoveryClass", "Open", 0, QApplication::UnicodeUTF8));
        actionSaveAs->setText(QApplication::translate("VideoDepthRecoveryClass", "SaveAs", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("VideoDepthRecoveryClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionDepthParameters->setText(QApplication::translate("VideoDepthRecoveryClass", "DepthParameters", 0, QApplication::UnicodeUTF8));
        actionRun_Initialization->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_Initialization", 0, QApplication::UnicodeUTF8));
        actionRun_BundleOptimization->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_BundleOptimization", 0, QApplication::UnicodeUTF8));
        actionRun_DepthExpansion->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_DepthExpansion", 0, QApplication::UnicodeUTF8));
        actionLoadProject->setText(QApplication::translate("VideoDepthRecoveryClass", "LoadProject", 0, QApplication::UnicodeUTF8));
        actionRun_VisualHull->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_VisualHull", 0, QApplication::UnicodeUTF8));
        actionLS_Parameters->setText(QApplication::translate("VideoDepthRecoveryClass", "LS_Parameters", 0, QApplication::UnicodeUTF8));
        actionRun_Init->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_Init", 0, QApplication::UnicodeUTF8));
        actionLSRun_Init->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_Init", 0, QApplication::UnicodeUTF8));
        actionLSRun_Refine->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_refine", 0, QApplication::UnicodeUTF8));
        actionLSRun_Mesh->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_Mesh", 0, QApplication::UnicodeUTF8));
        actionLSRun_DE->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_DE", 0, QApplication::UnicodeUTF8));
        actionLSRun_All->setText(QApplication::translate("VideoDepthRecoveryClass", "Run_All", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("VideoDepthRecoveryClass", "File", 0, QApplication::UnicodeUTF8));
        menuParameters->setTitle(QApplication::translate("VideoDepthRecoveryClass", "Parameters", 0, QApplication::UnicodeUTF8));
        menuLightstage->setTitle(QApplication::translate("VideoDepthRecoveryClass", "Lightstage", 0, QApplication::UnicodeUTF8));
        menuVideoDepth->setTitle(QApplication::translate("VideoDepthRecoveryClass", "VideoDepth", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("VideoDepthRecoveryClass", "About", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VideoDepthRecoveryClass: public Ui_VideoDepthRecoveryClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEODEPTHRECOVERY_H
