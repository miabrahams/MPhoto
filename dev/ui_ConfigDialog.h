/********************************************************************************
** Form generated from reading UI file 'ConfigDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLineEdit *le_start_path;
    QPushButton *b_browse;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *cb_show_name;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QComboBox *combo_file_name_position;
    QWidget *widget_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_3;
    QDoubleSpinBox *spin_file_name_size;
    QWidget *widget_7;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_4;
    QComboBox *combo_ui_size;
    QLabel *label_5;
    QCheckBox *cb_show_scroll;
    QCheckBox *cb_showfolders;
    QCheckBox *cb_extra_buttons;
    QCheckBox *cb_smooth_images;
    QCheckBox *cb_disable_animations;
    QCheckBox *cb_allow_rotation;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QComboBox *combo_slide_speed;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *cb_thumbnail_square;
    QCheckBox *cb_thumbnail_spacing;
    QCheckBox *cb_thumbnail_crop;
    QCheckBox *cb_thumbnails_show_name;
    QCheckBox *cb_thumbnail_text_under_image;
    QSpacerItem *verticalSpacer;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_6;
    QLabel *lb_color_preview;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *b_background;
    QPushButton *b_text;
    QSpacerItem *verticalSpacer_2;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *b_defaults;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QStringLiteral("ConfigDialog"));
        ConfigDialog->resize(659, 837);
        ConfigDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_3 = new QVBoxLayout(ConfigDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        tabWidget = new QTabWidget(ConfigDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_4 = new QVBoxLayout(tab);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        radioButton = new QRadioButton(groupBox);
        radioButton->setObjectName(QStringLiteral("radioButton"));

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(groupBox);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        radioButton_3 = new QRadioButton(groupBox);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        verticalLayout->addWidget(radioButton_3);

        widget = new QWidget(groupBox);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        le_start_path = new QLineEdit(widget);
        le_start_path->setObjectName(QStringLiteral("le_start_path"));

        horizontalLayout->addWidget(le_start_path);

        b_browse = new QPushButton(widget);
        b_browse->setObjectName(QStringLiteral("b_browse"));

        horizontalLayout->addWidget(b_browse);


        verticalLayout->addWidget(widget);


        verticalLayout_4->addWidget(groupBox);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        cb_show_name = new QCheckBox(groupBox_2);
        cb_show_name->setObjectName(QStringLiteral("cb_show_name"));

        verticalLayout_2->addWidget(cb_show_name);

        widget_4 = new QWidget(groupBox_2);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_4->addWidget(label_2);

        combo_file_name_position = new QComboBox(widget_4);
        combo_file_name_position->setObjectName(QStringLiteral("combo_file_name_position"));

        horizontalLayout_4->addWidget(combo_file_name_position);


        verticalLayout_2->addWidget(widget_4);

        widget_6 = new QWidget(groupBox_2);
        widget_6->setObjectName(QStringLiteral("widget_6"));
        horizontalLayout_6 = new QHBoxLayout(widget_6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(widget_6);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_6->addWidget(label_3);

        spin_file_name_size = new QDoubleSpinBox(widget_6);
        spin_file_name_size->setObjectName(QStringLiteral("spin_file_name_size"));
        spin_file_name_size->setMinimum(0.2);
        spin_file_name_size->setMaximum(40);
        spin_file_name_size->setSingleStep(0.5);
        spin_file_name_size->setValue(1);

        horizontalLayout_6->addWidget(spin_file_name_size);


        verticalLayout_2->addWidget(widget_6);

        widget_7 = new QWidget(groupBox_2);
        widget_7->setObjectName(QStringLiteral("widget_7"));
        horizontalLayout_7 = new QHBoxLayout(widget_7);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(widget_7);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_7->addWidget(label_4);

        combo_ui_size = new QComboBox(widget_7);
        combo_ui_size->setObjectName(QStringLiteral("combo_ui_size"));

        horizontalLayout_7->addWidget(combo_ui_size);


        verticalLayout_2->addWidget(widget_7);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_2->addWidget(label_5);

        cb_show_scroll = new QCheckBox(groupBox_2);
        cb_show_scroll->setObjectName(QStringLiteral("cb_show_scroll"));

        verticalLayout_2->addWidget(cb_show_scroll);

        cb_showfolders = new QCheckBox(groupBox_2);
        cb_showfolders->setObjectName(QStringLiteral("cb_showfolders"));

        verticalLayout_2->addWidget(cb_showfolders);

        cb_extra_buttons = new QCheckBox(groupBox_2);
        cb_extra_buttons->setObjectName(QStringLiteral("cb_extra_buttons"));

        verticalLayout_2->addWidget(cb_extra_buttons);

        cb_smooth_images = new QCheckBox(groupBox_2);
        cb_smooth_images->setObjectName(QStringLiteral("cb_smooth_images"));

        verticalLayout_2->addWidget(cb_smooth_images);

        cb_disable_animations = new QCheckBox(groupBox_2);
        cb_disable_animations->setObjectName(QStringLiteral("cb_disable_animations"));

        verticalLayout_2->addWidget(cb_disable_animations);

        cb_allow_rotation = new QCheckBox(groupBox_2);
        cb_allow_rotation->setObjectName(QStringLiteral("cb_allow_rotation"));

        verticalLayout_2->addWidget(cb_allow_rotation);

        widget_3 = new QWidget(groupBox_2);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget_3);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_3->addWidget(label);

        combo_slide_speed = new QComboBox(widget_3);
        combo_slide_speed->setObjectName(QStringLiteral("combo_slide_speed"));

        horizontalLayout_3->addWidget(combo_slide_speed);


        verticalLayout_2->addWidget(widget_3);


        verticalLayout_4->addWidget(groupBox_2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_5 = new QVBoxLayout(tab_2);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        cb_thumbnail_square = new QCheckBox(tab_2);
        cb_thumbnail_square->setObjectName(QStringLiteral("cb_thumbnail_square"));

        verticalLayout_5->addWidget(cb_thumbnail_square);

        cb_thumbnail_spacing = new QCheckBox(tab_2);
        cb_thumbnail_spacing->setObjectName(QStringLiteral("cb_thumbnail_spacing"));

        verticalLayout_5->addWidget(cb_thumbnail_spacing);

        cb_thumbnail_crop = new QCheckBox(tab_2);
        cb_thumbnail_crop->setObjectName(QStringLiteral("cb_thumbnail_crop"));

        verticalLayout_5->addWidget(cb_thumbnail_crop);

        cb_thumbnails_show_name = new QCheckBox(tab_2);
        cb_thumbnails_show_name->setObjectName(QStringLiteral("cb_thumbnails_show_name"));

        verticalLayout_5->addWidget(cb_thumbnails_show_name);

        cb_thumbnail_text_under_image = new QCheckBox(tab_2);
        cb_thumbnail_text_under_image->setObjectName(QStringLiteral("cb_thumbnail_text_under_image"));

        verticalLayout_5->addWidget(cb_thumbnail_text_under_image);

        verticalSpacer = new QSpacerItem(20, 317, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_6 = new QVBoxLayout(tab_3);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        lb_color_preview = new QLabel(tab_3);
        lb_color_preview->setObjectName(QStringLiteral("lb_color_preview"));
        lb_color_preview->setMinimumSize(QSize(0, 64));
        lb_color_preview->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(lb_color_preview);

        widget_2 = new QWidget(tab_3);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        b_background = new QPushButton(widget_2);
        b_background->setObjectName(QStringLiteral("b_background"));

        horizontalLayout_2->addWidget(b_background);

        b_text = new QPushButton(widget_2);
        b_text->setObjectName(QStringLiteral("b_text"));

        horizontalLayout_2->addWidget(b_text);


        verticalLayout_6->addWidget(widget_2);

        verticalSpacer_2 = new QSpacerItem(20, 227, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        tabWidget->addTab(tab_3, QString());

        verticalLayout_3->addWidget(tabWidget);

        widget_5 = new QWidget(ConfigDialog);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        horizontalLayout_5 = new QHBoxLayout(widget_5);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        b_defaults = new QPushButton(widget_5);
        b_defaults->setObjectName(QStringLiteral("b_defaults"));

        horizontalLayout_5->addWidget(b_defaults);

        horizontalSpacer = new QSpacerItem(35, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(widget_5);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_5->addWidget(buttonBox);


        verticalLayout_3->addWidget(widget_5);


        retranslateUi(ConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfigDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Settings", 0));
        groupBox->setTitle(QApplication::translate("ConfigDialog", "Start directory", 0));
        radioButton->setText(QApplication::translate("ConfigDialog", "Empt&y screen", 0));
        radioButton_2->setText(QApplication::translate("ConfigDialog", "&Last folder", 0));
        radioButton_3->setText(QApplication::translate("ConfigDialog", "C&ustom folder", 0));
        b_browse->setText(QApplication::translate("ConfigDialog", "Browse ...", 0));
        groupBox_2->setTitle(QApplication::translate("ConfigDialog", "Parameters", 0));
        cb_show_name->setText(QApplication::translate("ConfigDialog", "Show file name", 0));
        label_2->setText(QApplication::translate("ConfigDialog", "File name position", 0));
        label_3->setText(QApplication::translate("ConfigDialog", "File name size", 0));
        label_4->setText(QApplication::translate("ConfigDialog", "UI size", 0));
        label_5->setText(QApplication::translate("ConfigDialog", "(the ui size will be applied after restarting the application)", 0));
        cb_show_scroll->setText(QApplication::translate("ConfigDialog", "Always show scroll indicator", 0));
        cb_showfolders->setText(QApplication::translate("ConfigDialog", "Show folders", 0));
        cb_extra_buttons->setText(QApplication::translate("ConfigDialog", "Show extra buttons", 0));
        cb_smooth_images->setText(QApplication::translate("ConfigDialog", "Smooth images", 0));
        cb_disable_animations->setText(QApplication::translate("ConfigDialog", "Disable animations", 0));
        cb_allow_rotation->setText(QApplication::translate("ConfigDialog", "Enable touch rotation", 0));
        label->setText(QApplication::translate("ConfigDialog", "Image slide speed:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ConfigDialog", "General", 0));
#ifndef QT_NO_TOOLTIP
        cb_thumbnail_square->setToolTip(QApplication::translate("ConfigDialog", "Thumbnail shape is a square instead of a rectangle", 0));
#endif // QT_NO_TOOLTIP
        cb_thumbnail_square->setText(QApplication::translate("ConfigDialog", "Square thumbnails", 0));
#ifndef QT_NO_TOOLTIP
        cb_thumbnail_spacing->setToolTip(QApplication::translate("ConfigDialog", "Thumbnails have spacing between them", 0));
#endif // QT_NO_TOOLTIP
        cb_thumbnail_spacing->setText(QApplication::translate("ConfigDialog", "Spacing between thumbnails", 0));
#ifndef QT_NO_TOOLTIP
        cb_thumbnail_crop->setToolTip(QApplication::translate("ConfigDialog", "The image is cropped to fit he thumbnail istead of being resized", 0));
#endif // QT_NO_TOOLTIP
        cb_thumbnail_crop->setText(QApplication::translate("ConfigDialog", "Crop thumbnails", 0));
        cb_thumbnails_show_name->setText(QApplication::translate("ConfigDialog", "Show image names", 0));
        cb_thumbnail_text_under_image->setText(QApplication::translate("ConfigDialog", "Thumbnail text under the image", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ConfigDialog", "Thumbnails", 0));
        lb_color_preview->setText(QApplication::translate("ConfigDialog", "Color Preview Area", 0));
        b_background->setText(QApplication::translate("ConfigDialog", "Background color", 0));
        b_text->setText(QApplication::translate("ConfigDialog", "Text color", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("ConfigDialog", "Colors", 0));
        b_defaults->setText(QApplication::translate("ConfigDialog", "Reset to default", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
