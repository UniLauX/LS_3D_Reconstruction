/********************************************************************************
** Form generated from reading UI file 'runconfigdlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RUNCONFIGDLG_H
#define UI_RUNCONFIGDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RunConfigDlg
{
public:
    QGroupBox *GrpBox_General;
    QLabel *lb_Start;
    QLineEdit *lnEdt_Start;
    QLineEdit *lnEdt_End;
    QLabel *lb_End;
    QLabel *lb_Passes;
    QLineEdit *lnEdt_Passes;
    QGroupBox *grpBox_Pipeline;
    QCheckBox *chkBox_Init;
    QCheckBox *chkBox_BO;
    QCheckBox *chkBox_DE;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *hLayout_1;
    QCheckBox *chkBox_OutputTmpDat;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *hSpace_Left;
    QPushButton *pBtn_OK;
    QPushButton *pBtn_Cancle;
    QSpacerItem *hSpace_Right;

    void setupUi(QDialog *RunConfigDlg)
    {
        if (RunConfigDlg->objectName().isEmpty())
            RunConfigDlg->setObjectName(QString::fromUtf8("RunConfigDlg"));
        RunConfigDlg->resize(752, 343);
        GrpBox_General = new QGroupBox(RunConfigDlg);
        GrpBox_General->setObjectName(QString::fromUtf8("GrpBox_General"));
        GrpBox_General->setGeometry(QRect(20, 10, 701, 61));
        lb_Start = new QLabel(GrpBox_General);
        lb_Start->setObjectName(QString::fromUtf8("lb_Start"));
        lb_Start->setGeometry(QRect(10, 20, 41, 21));
        lnEdt_Start = new QLineEdit(GrpBox_General);
        lnEdt_Start->setObjectName(QString::fromUtf8("lnEdt_Start"));
        lnEdt_Start->setGeometry(QRect(40, 20, 101, 20));
        lnEdt_End = new QLineEdit(GrpBox_General);
        lnEdt_End->setObjectName(QString::fromUtf8("lnEdt_End"));
        lnEdt_End->setGeometry(QRect(280, 20, 101, 20));
        lb_End = new QLabel(GrpBox_General);
        lb_End->setObjectName(QString::fromUtf8("lb_End"));
        lb_End->setGeometry(QRect(240, 20, 31, 21));
        lb_Passes = new QLabel(GrpBox_General);
        lb_Passes->setObjectName(QString::fromUtf8("lb_Passes"));
        lb_Passes->setGeometry(QRect(460, 20, 41, 21));
        lnEdt_Passes = new QLineEdit(GrpBox_General);
        lnEdt_Passes->setObjectName(QString::fromUtf8("lnEdt_Passes"));
        lnEdt_Passes->setGeometry(QRect(510, 20, 101, 20));
        grpBox_Pipeline = new QGroupBox(RunConfigDlg);
        grpBox_Pipeline->setObjectName(QString::fromUtf8("grpBox_Pipeline"));
        grpBox_Pipeline->setGeometry(QRect(20, 90, 701, 71));
        chkBox_Init = new QCheckBox(grpBox_Pipeline);
        chkBox_Init->setObjectName(QString::fromUtf8("chkBox_Init"));
        chkBox_Init->setGeometry(QRect(20, 30, 121, 16));
        chkBox_BO = new QCheckBox(grpBox_Pipeline);
        chkBox_BO->setObjectName(QString::fromUtf8("chkBox_BO"));
        chkBox_BO->setGeometry(QRect(250, 30, 141, 16));
        chkBox_DE = new QCheckBox(grpBox_Pipeline);
        chkBox_DE->setObjectName(QString::fromUtf8("chkBox_DE"));
        chkBox_DE->setGeometry(QRect(470, 30, 151, 16));
        horizontalLayoutWidget = new QWidget(RunConfigDlg);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(30, 180, 671, 51));
        hLayout_1 = new QHBoxLayout(horizontalLayoutWidget);
        hLayout_1->setSpacing(6);
        hLayout_1->setContentsMargins(11, 11, 11, 11);
        hLayout_1->setObjectName(QString::fromUtf8("hLayout_1"));
        hLayout_1->setContentsMargins(0, 0, 0, 0);
        chkBox_OutputTmpDat = new QCheckBox(horizontalLayoutWidget);
        chkBox_OutputTmpDat->setObjectName(QString::fromUtf8("chkBox_OutputTmpDat"));
        chkBox_OutputTmpDat->setEnabled(false);
        chkBox_OutputTmpDat->setChecked(true);

        hLayout_1->addWidget(chkBox_OutputTmpDat);

        horizontalLayoutWidget_2 = new QWidget(RunConfigDlg);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(30, 259, 671, 51));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        hSpace_Left = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(hSpace_Left);

        pBtn_OK = new QPushButton(horizontalLayoutWidget_2);
        pBtn_OK->setObjectName(QString::fromUtf8("pBtn_OK"));

        horizontalLayout_2->addWidget(pBtn_OK);

        pBtn_Cancle = new QPushButton(horizontalLayoutWidget_2);
        pBtn_Cancle->setObjectName(QString::fromUtf8("pBtn_Cancle"));

        horizontalLayout_2->addWidget(pBtn_Cancle);

        hSpace_Right = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(hSpace_Right);


        retranslateUi(RunConfigDlg);

        QMetaObject::connectSlotsByName(RunConfigDlg);
    } // setupUi

    void retranslateUi(QDialog *RunConfigDlg)
    {
        RunConfigDlg->setWindowTitle(QApplication::translate("RunConfigDlg", "RunConfigDlg", 0, QApplication::UnicodeUTF8));
        GrpBox_General->setTitle(QApplication::translate("RunConfigDlg", "General", 0, QApplication::UnicodeUTF8));
        lb_Start->setText(QApplication::translate("RunConfigDlg", "Start", 0, QApplication::UnicodeUTF8));
        lb_End->setText(QApplication::translate("RunConfigDlg", "End", 0, QApplication::UnicodeUTF8));
        lb_Passes->setText(QApplication::translate("RunConfigDlg", "Passes", 0, QApplication::UnicodeUTF8));
        grpBox_Pipeline->setTitle(QApplication::translate("RunConfigDlg", "Pipeline", 0, QApplication::UnicodeUTF8));
        chkBox_Init->setText(QApplication::translate("RunConfigDlg", "Initialization", 0, QApplication::UnicodeUTF8));
        chkBox_BO->setText(QApplication::translate("RunConfigDlg", "Bundle Optimization", 0, QApplication::UnicodeUTF8));
        chkBox_DE->setText(QApplication::translate("RunConfigDlg", "Depth Level Expansion", 0, QApplication::UnicodeUTF8));
        chkBox_OutputTmpDat->setText(QApplication::translate("RunConfigDlg", "Output Temporary Data", 0, QApplication::UnicodeUTF8));
        pBtn_OK->setText(QApplication::translate("RunConfigDlg", "OK", 0, QApplication::UnicodeUTF8));
        pBtn_Cancle->setText(QApplication::translate("RunConfigDlg", "Cancle", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RunConfigDlg: public Ui_RunConfigDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RUNCONFIGDLG_H
