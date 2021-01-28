/********************************************************************************
** Form generated from reading UI file 'depthparawidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEPTHPARAWIDGET_H
#define UI_DEPTHPARAWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DepthParaWidget
{
public:
    QGroupBox *groupBox_Block;
    QLabel *lb_width;
    QLabel *lb_height;
    QLineEdit *lineEdit_BlockWidth;
    QLineEdit *lineEdit_BlockHeight;
    QCheckBox *checkBox_AutoPartition;
    QLabel *lb_overlap;
    QLabel *lb_Resample;
    QLineEdit *lineEdit_BlockOverlap;
    QLineEdit *lineEdit_Resample;
    QGroupBox *groupBox;
    QLabel *lable_StartDisp;
    QLabel *lable_InitStep;
    QLabel *lable_InitCount;
    QLineEdit *lineEdit_RefStartDisp;
    QLineEdit *lineEdit_RefInitStep;
    QLineEdit *lineEdit_RefInitCount;
    QLabel *lable_StartDisp_2;
    QLineEdit *lineEdit_RefNormalStep;
    QLabel *lable_StartDisp_5;
    QLineEdit *lineEdit_RefMaxCount;
    QCheckBox *checkBox_Unordered;
    QCheckBox *checkBox_Ordered_Select;
    QGroupBox *groupBox_2;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *lb_width_2;
    QLineEdit *lineEdit_SegSpatial;
    QLabel *lb_width_4;
    QLineEdit *lineEdit_SegColor;
    QLabel *lb_width_5;
    QLineEdit *lineEdit_SegMinSize;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lb_width_6;
    QLineEdit *lineEdit_SegPlanFittingSize;
    QLabel *lb_width_7;
    QLineEdit *lineEdit_SegAcceptableThreshold;
    QGroupBox *groupBox_3;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lb_width_10;
    QLineEdit *lineEdit_DspMax;
    QCheckBox *checkBox_AutoDspMax;
    QLabel *lb_width_8;
    QLineEdit *lineEdit_DspMin;
    QLabel *lb_width_9;
    QLineEdit *lineEdit_DspColorSigma;
    QLabel *lb_width_11;
    QLineEdit *lineEdit_DspLevelNum;
    QLabel *lb_width_12;
    QLineEdit *lineEdit_DspDspSigma;
    QLineEdit *lineEdit_DspProjectSigma;
    QLabel *lable_InitCount_2;
    QGroupBox *groupBox_Block_2;
    QLabel *lb_overlap_2;
    QLabel *lb_Resample_2;
    QLineEdit *lineEdit_DESubLevels;
    QLineEdit *lineEdit_DESubIterator;
    QGroupBox *groupBox_Block_3;
    QLabel *lb_overlap_3;
    QLabel *lb_Resample_3;
    QLineEdit *lineEdit_SmoothTruncated;
    QLineEdit *lineEdit_DataCostWeight;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QPushButton *applyButton;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *lineEdit_ThreadsCount;
    QLabel *lb_Resample_4;
    QGroupBox *groupBox_Block_4;
    QRadioButton *radioButton_ImageSpace;
    QRadioButton *radioButton_DspSpace;

    void setupUi(QWidget *DepthParaWidget)
    {
        if (DepthParaWidget->objectName().isEmpty())
            DepthParaWidget->setObjectName(QString::fromUtf8("DepthParaWidget"));
        groupBox_Block = new QGroupBox(DepthParaWidget);
        groupBox_Block->setObjectName(QString::fromUtf8("groupBox_Block"));
        groupBox_Block->setGeometry(QRect(30, 20, 511, 51));
        lb_width = new QLabel(groupBox_Block);
        lb_width->setObjectName(QString::fromUtf8("lb_width"));
        lb_width->setGeometry(QRect(10, 20, 54, 12));
        lb_height = new QLabel(groupBox_Block);
        lb_height->setObjectName(QString::fromUtf8("lb_height"));
        lb_height->setGeometry(QRect(110, 20, 54, 12));
        lineEdit_BlockWidth = new QLineEdit(groupBox_Block);
        lineEdit_BlockWidth->setObjectName(QString::fromUtf8("lineEdit_BlockWidth"));
        lineEdit_BlockWidth->setEnabled(false);
        lineEdit_BlockWidth->setGeometry(QRect(50, 20, 51, 20));
        lineEdit_BlockHeight = new QLineEdit(groupBox_Block);
        lineEdit_BlockHeight->setObjectName(QString::fromUtf8("lineEdit_BlockHeight"));
        lineEdit_BlockHeight->setEnabled(false);
        lineEdit_BlockHeight->setGeometry(QRect(160, 20, 51, 20));
        checkBox_AutoPartition = new QCheckBox(groupBox_Block);
        checkBox_AutoPartition->setObjectName(QString::fromUtf8("checkBox_AutoPartition"));
        checkBox_AutoPartition->setGeometry(QRect(220, 20, 71, 16));
        checkBox_AutoPartition->setAcceptDrops(false);
        checkBox_AutoPartition->setAutoFillBackground(false);
        checkBox_AutoPartition->setChecked(true);
        lb_overlap = new QLabel(groupBox_Block);
        lb_overlap->setObjectName(QString::fromUtf8("lb_overlap"));
        lb_overlap->setGeometry(QRect(280, 20, 41, 16));
        lb_Resample = new QLabel(groupBox_Block);
        lb_Resample->setObjectName(QString::fromUtf8("lb_Resample"));
        lb_Resample->setGeometry(QRect(400, 20, 54, 16));
        lineEdit_BlockOverlap = new QLineEdit(groupBox_Block);
        lineEdit_BlockOverlap->setObjectName(QString::fromUtf8("lineEdit_BlockOverlap"));
        lineEdit_BlockOverlap->setGeometry(QRect(330, 20, 51, 20));
        lineEdit_Resample = new QLineEdit(groupBox_Block);
        lineEdit_Resample->setObjectName(QString::fromUtf8("lineEdit_Resample"));
        lineEdit_Resample->setGeometry(QRect(450, 20, 51, 20));
        groupBox = new QGroupBox(DepthParaWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(30, 80, 511, 91));
        lable_StartDisp = new QLabel(groupBox);
        lable_StartDisp->setObjectName(QString::fromUtf8("lable_StartDisp"));
        lable_StartDisp->setEnabled(false);
        lable_StartDisp->setGeometry(QRect(20, 20, 61, 16));
        lable_InitStep = new QLabel(groupBox);
        lable_InitStep->setObjectName(QString::fromUtf8("lable_InitStep"));
        lable_InitStep->setEnabled(false);
        lable_InitStep->setGeometry(QRect(180, 20, 61, 16));
        lable_InitCount = new QLabel(groupBox);
        lable_InitCount->setObjectName(QString::fromUtf8("lable_InitCount"));
        lable_InitCount->setEnabled(false);
        lable_InitCount->setGeometry(QRect(340, 20, 61, 16));
        lineEdit_RefStartDisp = new QLineEdit(groupBox);
        lineEdit_RefStartDisp->setObjectName(QString::fromUtf8("lineEdit_RefStartDisp"));
        lineEdit_RefStartDisp->setEnabled(false);
        lineEdit_RefStartDisp->setGeometry(QRect(90, 20, 51, 20));
        lineEdit_RefInitStep = new QLineEdit(groupBox);
        lineEdit_RefInitStep->setObjectName(QString::fromUtf8("lineEdit_RefInitStep"));
        lineEdit_RefInitStep->setEnabled(false);
        lineEdit_RefInitStep->setGeometry(QRect(240, 20, 51, 20));
        lineEdit_RefInitCount = new QLineEdit(groupBox);
        lineEdit_RefInitCount->setObjectName(QString::fromUtf8("lineEdit_RefInitCount"));
        lineEdit_RefInitCount->setEnabled(false);
        lineEdit_RefInitCount->setGeometry(QRect(420, 20, 51, 20));
        lable_StartDisp_2 = new QLabel(groupBox);
        lable_StartDisp_2->setObjectName(QString::fromUtf8("lable_StartDisp_2"));
        lable_StartDisp_2->setEnabled(false);
        lable_StartDisp_2->setGeometry(QRect(20, 60, 71, 16));
        lineEdit_RefNormalStep = new QLineEdit(groupBox);
        lineEdit_RefNormalStep->setObjectName(QString::fromUtf8("lineEdit_RefNormalStep"));
        lineEdit_RefNormalStep->setEnabled(false);
        lineEdit_RefNormalStep->setGeometry(QRect(90, 60, 51, 20));
        lable_StartDisp_5 = new QLabel(groupBox);
        lable_StartDisp_5->setObjectName(QString::fromUtf8("lable_StartDisp_5"));
        lable_StartDisp_5->setEnabled(true);
        lable_StartDisp_5->setGeometry(QRect(180, 60, 71, 16));
        lineEdit_RefMaxCount = new QLineEdit(groupBox);
        lineEdit_RefMaxCount->setObjectName(QString::fromUtf8("lineEdit_RefMaxCount"));
        lineEdit_RefMaxCount->setGeometry(QRect(240, 60, 51, 20));
        checkBox_Unordered = new QCheckBox(groupBox);
        checkBox_Unordered->setObjectName(QString::fromUtf8("checkBox_Unordered"));
        checkBox_Unordered->setGeometry(QRect(330, 60, 71, 16));
        checkBox_Unordered->setAcceptDrops(false);
        checkBox_Unordered->setAutoFillBackground(false);
        checkBox_Unordered->setChecked(true);
        checkBox_Ordered_Select = new QCheckBox(groupBox);
        checkBox_Ordered_Select->setObjectName(QString::fromUtf8("checkBox_Ordered_Select"));
        checkBox_Ordered_Select->setEnabled(false);
        checkBox_Ordered_Select->setGeometry(QRect(430, 60, 71, 16));
        checkBox_Ordered_Select->setAcceptDrops(false);
        checkBox_Ordered_Select->setAutoFillBackground(false);
        checkBox_Ordered_Select->setChecked(true);
        groupBox_2 = new QGroupBox(DepthParaWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(30, 180, 521, 101));
        horizontalLayoutWidget = new QWidget(groupBox_2);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(30, 20, 481, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lb_width_2 = new QLabel(horizontalLayoutWidget);
        lb_width_2->setObjectName(QString::fromUtf8("lb_width_2"));

        horizontalLayout->addWidget(lb_width_2);

        lineEdit_SegSpatial = new QLineEdit(horizontalLayoutWidget);
        lineEdit_SegSpatial->setObjectName(QString::fromUtf8("lineEdit_SegSpatial"));

        horizontalLayout->addWidget(lineEdit_SegSpatial);

        lb_width_4 = new QLabel(horizontalLayoutWidget);
        lb_width_4->setObjectName(QString::fromUtf8("lb_width_4"));

        horizontalLayout->addWidget(lb_width_4);

        lineEdit_SegColor = new QLineEdit(horizontalLayoutWidget);
        lineEdit_SegColor->setObjectName(QString::fromUtf8("lineEdit_SegColor"));

        horizontalLayout->addWidget(lineEdit_SegColor);

        lb_width_5 = new QLabel(horizontalLayoutWidget);
        lb_width_5->setObjectName(QString::fromUtf8("lb_width_5"));

        horizontalLayout->addWidget(lb_width_5);

        lineEdit_SegMinSize = new QLineEdit(horizontalLayoutWidget);
        lineEdit_SegMinSize->setObjectName(QString::fromUtf8("lineEdit_SegMinSize"));

        horizontalLayout->addWidget(lineEdit_SegMinSize);

        horizontalLayoutWidget_2 = new QWidget(groupBox_2);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(30, 60, 481, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lb_width_6 = new QLabel(horizontalLayoutWidget_2);
        lb_width_6->setObjectName(QString::fromUtf8("lb_width_6"));

        horizontalLayout_2->addWidget(lb_width_6);

        lineEdit_SegPlanFittingSize = new QLineEdit(horizontalLayoutWidget_2);
        lineEdit_SegPlanFittingSize->setObjectName(QString::fromUtf8("lineEdit_SegPlanFittingSize"));

        horizontalLayout_2->addWidget(lineEdit_SegPlanFittingSize);

        lb_width_7 = new QLabel(horizontalLayoutWidget_2);
        lb_width_7->setObjectName(QString::fromUtf8("lb_width_7"));

        horizontalLayout_2->addWidget(lb_width_7);

        lineEdit_SegAcceptableThreshold = new QLineEdit(horizontalLayoutWidget_2);
        lineEdit_SegAcceptableThreshold->setObjectName(QString::fromUtf8("lineEdit_SegAcceptableThreshold"));

        horizontalLayout_2->addWidget(lineEdit_SegAcceptableThreshold);

        groupBox_3 = new QGroupBox(DepthParaWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(30, 290, 491, 111));
        horizontalLayoutWidget_3 = new QWidget(groupBox_3);
        horizontalLayoutWidget_3->setObjectName(QString::fromUtf8("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(200, 10, 201, 31));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        lb_width_10 = new QLabel(horizontalLayoutWidget_3);
        lb_width_10->setObjectName(QString::fromUtf8("lb_width_10"));

        horizontalLayout_3->addWidget(lb_width_10);

        lineEdit_DspMax = new QLineEdit(horizontalLayoutWidget_3);
        lineEdit_DspMax->setObjectName(QString::fromUtf8("lineEdit_DspMax"));

        horizontalLayout_3->addWidget(lineEdit_DspMax);

        checkBox_AutoDspMax = new QCheckBox(horizontalLayoutWidget_3);
        checkBox_AutoDspMax->setObjectName(QString::fromUtf8("checkBox_AutoDspMax"));
        checkBox_AutoDspMax->setAcceptDrops(false);
        checkBox_AutoDspMax->setAutoFillBackground(false);
        checkBox_AutoDspMax->setChecked(true);

        horizontalLayout_3->addWidget(checkBox_AutoDspMax);

        lb_width_8 = new QLabel(groupBox_3);
        lb_width_8->setObjectName(QString::fromUtf8("lb_width_8"));
        lb_width_8->setGeometry(QRect(20, 10, 42, 29));
        lineEdit_DspMin = new QLineEdit(groupBox_3);
        lineEdit_DspMin->setObjectName(QString::fromUtf8("lineEdit_DspMin"));
        lineEdit_DspMin->setGeometry(QRect(80, 20, 91, 20));
        lb_width_9 = new QLabel(groupBox_3);
        lb_width_9->setObjectName(QString::fromUtf8("lb_width_9"));
        lb_width_9->setGeometry(QRect(10, 80, 71, 29));
        lineEdit_DspColorSigma = new QLineEdit(groupBox_3);
        lineEdit_DspColorSigma->setObjectName(QString::fromUtf8("lineEdit_DspColorSigma"));
        lineEdit_DspColorSigma->setGeometry(QRect(80, 80, 111, 20));
        lb_width_11 = new QLabel(groupBox_3);
        lb_width_11->setObjectName(QString::fromUtf8("lb_width_11"));
        lb_width_11->setGeometry(QRect(10, 40, 51, 29));
        lineEdit_DspLevelNum = new QLineEdit(groupBox_3);
        lineEdit_DspLevelNum->setObjectName(QString::fromUtf8("lineEdit_DspLevelNum"));
        lineEdit_DspLevelNum->setGeometry(QRect(80, 50, 101, 20));
        lb_width_12 = new QLabel(groupBox_3);
        lb_width_12->setObjectName(QString::fromUtf8("lb_width_12"));
        lb_width_12->setGeometry(QRect(240, 80, 61, 29));
        lineEdit_DspDspSigma = new QLineEdit(groupBox_3);
        lineEdit_DspDspSigma->setObjectName(QString::fromUtf8("lineEdit_DspDspSigma"));
        lineEdit_DspDspSigma->setGeometry(QRect(340, 80, 111, 20));
        lineEdit_DspProjectSigma = new QLineEdit(groupBox_3);
        lineEdit_DspProjectSigma->setObjectName(QString::fromUtf8("lineEdit_DspProjectSigma"));
        lineEdit_DspProjectSigma->setEnabled(false);
        lineEdit_DspProjectSigma->setGeometry(QRect(330, 50, 121, 20));
        lable_InitCount_2 = new QLabel(groupBox_3);
        lable_InitCount_2->setObjectName(QString::fromUtf8("lable_InitCount_2"));
        lable_InitCount_2->setEnabled(false);
        lable_InitCount_2->setGeometry(QRect(220, 50, 101, 20));
        groupBox_Block_2 = new QGroupBox(DepthParaWidget);
        groupBox_Block_2->setObjectName(QString::fromUtf8("groupBox_Block_2"));
        groupBox_Block_2->setGeometry(QRect(30, 410, 491, 51));
        lb_overlap_2 = new QLabel(groupBox_Block_2);
        lb_overlap_2->setObjectName(QString::fromUtf8("lb_overlap_2"));
        lb_overlap_2->setGeometry(QRect(20, 20, 61, 16));
        lb_Resample_2 = new QLabel(groupBox_Block_2);
        lb_Resample_2->setObjectName(QString::fromUtf8("lb_Resample_2"));
        lb_Resample_2->setGeometry(QRect(243, 20, 71, 20));
        lineEdit_DESubLevels = new QLineEdit(groupBox_Block_2);
        lineEdit_DESubLevels->setObjectName(QString::fromUtf8("lineEdit_DESubLevels"));
        lineEdit_DESubLevels->setGeometry(QRect(90, 20, 91, 20));
        lineEdit_DESubIterator = new QLineEdit(groupBox_Block_2);
        lineEdit_DESubIterator->setObjectName(QString::fromUtf8("lineEdit_DESubIterator"));
        lineEdit_DESubIterator->setGeometry(QRect(320, 20, 101, 20));
        groupBox_Block_3 = new QGroupBox(DepthParaWidget);
        groupBox_Block_3->setObjectName(QString::fromUtf8("groupBox_Block_3"));
        groupBox_Block_3->setGeometry(QRect(30, 460, 491, 51));
        lb_overlap_3 = new QLabel(groupBox_Block_3);
        lb_overlap_3->setObjectName(QString::fromUtf8("lb_overlap_3"));
        lb_overlap_3->setGeometry(QRect(0, 20, 121, 20));
        lb_Resample_3 = new QLabel(groupBox_Block_3);
        lb_Resample_3->setObjectName(QString::fromUtf8("lb_Resample_3"));
        lb_Resample_3->setGeometry(QRect(243, 20, 101, 20));
        lineEdit_SmoothTruncated = new QLineEdit(groupBox_Block_3);
        lineEdit_SmoothTruncated->setObjectName(QString::fromUtf8("lineEdit_SmoothTruncated"));
        lineEdit_SmoothTruncated->setGeometry(QRect(130, 20, 91, 20));
        lineEdit_DataCostWeight = new QLineEdit(groupBox_Block_3);
        lineEdit_DataCostWeight->setObjectName(QString::fromUtf8("lineEdit_DataCostWeight"));
        lineEdit_DataCostWeight->setGeometry(QRect(360, 20, 101, 20));
        horizontalLayoutWidget_5 = new QWidget(DepthParaWidget);
        horizontalLayoutWidget_5->setObjectName(QString::fromUtf8("horizontalLayoutWidget_5"));
        horizontalLayoutWidget_5->setGeometry(QRect(30, 610, 481, 41));
        horizontalLayout_5 = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        applyButton = new QPushButton(horizontalLayoutWidget_5);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));

        horizontalLayout_5->addWidget(applyButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        lineEdit_ThreadsCount = new QLineEdit(DepthParaWidget);
        lineEdit_ThreadsCount->setObjectName(QString::fromUtf8("lineEdit_ThreadsCount"));
        lineEdit_ThreadsCount->setGeometry(QRect(380, 550, 133, 20));
        lb_Resample_4 = new QLabel(DepthParaWidget);
        lb_Resample_4->setObjectName(QString::fromUtf8("lb_Resample_4"));
        lb_Resample_4->setGeometry(QRect(290, 550, 78, 16));
        groupBox_Block_4 = new QGroupBox(DepthParaWidget);
        groupBox_Block_4->setObjectName(QString::fromUtf8("groupBox_Block_4"));
        groupBox_Block_4->setGeometry(QRect(30, 530, 251, 51));
        radioButton_ImageSpace = new QRadioButton(groupBox_Block_4);
        radioButton_ImageSpace->setObjectName(QString::fromUtf8("radioButton_ImageSpace"));
        radioButton_ImageSpace->setGeometry(QRect(20, 30, 89, 16));
        radioButton_DspSpace = new QRadioButton(groupBox_Block_4);
        radioButton_DspSpace->setObjectName(QString::fromUtf8("radioButton_DspSpace"));
        radioButton_DspSpace->setGeometry(QRect(110, 30, 111, 16));
        radioButton_ImageSpace->raise();
        radioButton_DspSpace->raise();
        lb_Resample_4->raise();

        retranslateUi(DepthParaWidget);

        QMetaObject::connectSlotsByName(DepthParaWidget);
    } // setupUi

    void retranslateUi(QWidget *DepthParaWidget)
    {
        DepthParaWidget->setWindowTitle(QApplication::translate("DepthParaWidget", "DepthParaWidget", 0, QApplication::UnicodeUTF8));
        groupBox_Block->setTitle(QApplication::translate("DepthParaWidget", "Block|Resample", 0, QApplication::UnicodeUTF8));
        lb_width->setText(QApplication::translate("DepthParaWidget", "Width", 0, QApplication::UnicodeUTF8));
        lb_height->setText(QApplication::translate("DepthParaWidget", "Height", 0, QApplication::UnicodeUTF8));
        checkBox_AutoPartition->setText(QApplication::translate("DepthParaWidget", "Auto", 0, QApplication::UnicodeUTF8));
        lb_overlap->setText(QApplication::translate("DepthParaWidget", "Overlap", 0, QApplication::UnicodeUTF8));
        lb_Resample->setText(QApplication::translate("DepthParaWidget", "Resample", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DepthParaWidget", "Reference Frames Selection", 0, QApplication::UnicodeUTF8));
        lable_StartDisp->setText(QApplication::translate("DepthParaWidget", "Start Disp", 0, QApplication::UnicodeUTF8));
        lable_InitStep->setText(QApplication::translate("DepthParaWidget", "Init Step", 0, QApplication::UnicodeUTF8));
        lable_InitCount->setText(QApplication::translate("DepthParaWidget", "Init Count", 0, QApplication::UnicodeUTF8));
        lable_StartDisp_2->setText(QApplication::translate("DepthParaWidget", "Normal Step", 0, QApplication::UnicodeUTF8));
        lable_StartDisp_5->setText(QApplication::translate("DepthParaWidget", "Max Count", 0, QApplication::UnicodeUTF8));
        checkBox_Unordered->setText(QApplication::translate("DepthParaWidget", "Unorder", 0, QApplication::UnicodeUTF8));
        checkBox_Ordered_Select->setText(QApplication::translate("DepthParaWidget", "Auto", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("DepthParaWidget", "Mean Shift Segmentation", 0, QApplication::UnicodeUTF8));
        lb_width_2->setText(QApplication::translate("DepthParaWidget", "Spatial", 0, QApplication::UnicodeUTF8));
        lb_width_4->setText(QApplication::translate("DepthParaWidget", "Color", 0, QApplication::UnicodeUTF8));
        lb_width_5->setText(QApplication::translate("DepthParaWidget", "MinSize", 0, QApplication::UnicodeUTF8));
        lb_width_6->setText(QApplication::translate("DepthParaWidget", "Plan Fitting Size >=", 0, QApplication::UnicodeUTF8));
        lb_width_7->setText(QApplication::translate("DepthParaWidget", "Acceptable Threshold", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("DepthParaWidget", "Disparity Configuration", 0, QApplication::UnicodeUTF8));
        lb_width_10->setText(QApplication::translate("DepthParaWidget", "Max.Dsp", 0, QApplication::UnicodeUTF8));
        checkBox_AutoDspMax->setText(QApplication::translate("DepthParaWidget", "Auto", 0, QApplication::UnicodeUTF8));
        lb_width_8->setText(QApplication::translate("DepthParaWidget", "Min.Dsp", 0, QApplication::UnicodeUTF8));
        lb_width_9->setText(QApplication::translate("DepthParaWidget", "Color Sigma", 0, QApplication::UnicodeUTF8));
        lb_width_11->setText(QApplication::translate("DepthParaWidget", "Level Num", 0, QApplication::UnicodeUTF8));
        lb_width_12->setText(QApplication::translate("DepthParaWidget", "Dsp.Sigma", 0, QApplication::UnicodeUTF8));
        lable_InitCount_2->setText(QApplication::translate("DepthParaWidget", "Projection Sigma", 0, QApplication::UnicodeUTF8));
        groupBox_Block_2->setTitle(QApplication::translate("DepthParaWidget", "Depth Expansion", 0, QApplication::UnicodeUTF8));
        lb_overlap_2->setText(QApplication::translate("DepthParaWidget", "Sub-Levels", 0, QApplication::UnicodeUTF8));
        lb_Resample_2->setText(QApplication::translate("DepthParaWidget", "Sub-Iterator", 0, QApplication::UnicodeUTF8));
        groupBox_Block_3->setTitle(QApplication::translate("DepthParaWidget", "Weight", 0, QApplication::UnicodeUTF8));
        lb_overlap_3->setText(QApplication::translate("DepthParaWidget", "Smoothness Truncated", 0, QApplication::UnicodeUTF8));
        lb_Resample_3->setText(QApplication::translate("DepthParaWidget", "DataCost Weight", 0, QApplication::UnicodeUTF8));
        applyButton->setText(QApplication::translate("DepthParaWidget", "Apply", 0, QApplication::UnicodeUTF8));
        lb_Resample_4->setText(QApplication::translate("DepthParaWidget", "Threads Count", 0, QApplication::UnicodeUTF8));
        groupBox_Block_4->setTitle(QApplication::translate("DepthParaWidget", "Weight", 0, QApplication::UnicodeUTF8));
        radioButton_ImageSpace->setText(QApplication::translate("DepthParaWidget", "Image Space", 0, QApplication::UnicodeUTF8));
        radioButton_DspSpace->setText(QApplication::translate("DepthParaWidget", "Disparity Space", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DepthParaWidget: public Ui_DepthParaWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEPTHPARAWIDGET_H
