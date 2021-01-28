/********************************************************************************
** Form generated from reading UI file 'LSRunConfigDlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LSRUNCONFIGDLG_H
#define UI_LSRUNCONFIGDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LSRunConfigDlg
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *LsBtnCancle;

    void setupUi(QDialog *LSRunConfigDlg)
    {
        if (LSRunConfigDlg->objectName().isEmpty())
            LSRunConfigDlg->setObjectName(QString::fromUtf8("LSRunConfigDlg"));
        LSRunConfigDlg->resize(400, 300);
        layoutWidget = new QWidget(LSRunConfigDlg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 250, 351, 33));
        hboxLayout = new QHBoxLayout(layoutWidget);
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(layoutWidget);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        hboxLayout->addWidget(okButton);

        LsBtnCancle = new QPushButton(layoutWidget);
        LsBtnCancle->setObjectName(QString::fromUtf8("LsBtnCancle"));

        hboxLayout->addWidget(LsBtnCancle);


        retranslateUi(LSRunConfigDlg);
        QObject::connect(okButton, SIGNAL(clicked()), LSRunConfigDlg, SLOT(accept()));
        QObject::connect(LsBtnCancle, SIGNAL(clicked()), LSRunConfigDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(LSRunConfigDlg);
    } // setupUi

    void retranslateUi(QDialog *LSRunConfigDlg)
    {
        LSRunConfigDlg->setWindowTitle(QApplication::translate("LSRunConfigDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("LSRunConfigDlg", "OK", 0, QApplication::UnicodeUTF8));
        LsBtnCancle->setText(QApplication::translate("LSRunConfigDlg", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LSRunConfigDlg: public Ui_LSRunConfigDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSRUNCONFIGDLG_H
