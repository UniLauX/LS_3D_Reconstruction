/********************************************************************************
** Form generated from reading UI file 'lsparawidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LSPARAWIDGET_H
#define UI_LSPARAWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LSParaWidget
{
public:

    void setupUi(QWidget *LSParaWidget)
    {
        if (LSParaWidget->objectName().isEmpty())
            LSParaWidget->setObjectName(QString::fromUtf8("LSParaWidget"));
        LSParaWidget->resize(400, 300);

        retranslateUi(LSParaWidget);

        QMetaObject::connectSlotsByName(LSParaWidget);
    } // setupUi

    void retranslateUi(QWidget *LSParaWidget)
    {
        LSParaWidget->setWindowTitle(QApplication::translate("LSParaWidget", "LSParaWidget", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LSParaWidget: public Ui_LSParaWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSPARAWIDGET_H
