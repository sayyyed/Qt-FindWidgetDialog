#include "findwidgetdialog.h"
#include "ui_findwidgetdialog.h"
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QCompleter>
#include <QTimer>
#include <QDebug>
#include <QToolBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>

FindWidgetDialog::FindWidgetDialog(QWidget *targetWidget,
                                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindWidgetDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(Qt::WindowMaximized);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(Qt::gray);
    shadow->setOffset(0);
    ui->innerFrame->setGraphicsEffect(shadow);

    QList<QWidget *> childWidgets = targetWidget->findChildren<QWidget*>();

    QStringList widgetsTexts;
    QHash<QWidget*, QList<QWidget*>> widgetParents;

    foreach (QWidget *childWidget, childWidgets)
    {
        if(childWidget->inherits("QLabel"))
            widgetsTexts << qobject_cast<QLabel*>(childWidget)->text();
        else if(childWidget->inherits("QCheckBox"))
            widgetsTexts << qobject_cast<QCheckBox*>(childWidget)->text();
        else if(childWidget->inherits("QLineEdit"))
        {
            QLineEdit *childLineEdit = qobject_cast<QLineEdit*>(childWidget);
            if(childLineEdit && !childLineEdit->placeholderText().isEmpty())
                widgetsTexts << childLineEdit->placeholderText();
        }
        else if(childWidget->inherits("QGroupBox"))
        {
            QGroupBox *groupBox = qobject_cast<QGroupBox*>(childWidget);
            if(groupBox && !groupBox->title().isEmpty())
                widgetsTexts << groupBox->title();
        }

        if(childWidget->parentWidget())
        {
            QWidget *p = childWidget;
            while (p != targetWidget)
            {
                p = p->parentWidget();
                widgetParents[childWidget] << p;
//                qDebug() << label->text() << p->objectName();

            }
        }
    }

    if(!widgetsTexts.isEmpty())
    {
        auto find = [=]()
        {
            QString findText = ui->lineEdit->text();
            if(findText.isEmpty())
                return;

            foreach (QWidget *childWidget, childWidgets)
            {
                QString text;
                if(childWidget->inherits("QLabel"))
                    text = qobject_cast<QLabel*>(childWidget)->text();
                else if(childWidget->inherits("QCheckBox"))
                    text = qobject_cast<QCheckBox*>(childWidget)->text();
                else if(childWidget->inherits("QLineEdit"))
                {
                    QLineEdit *childLineEdit = qobject_cast<QLineEdit*>(childWidget);
                    if(childLineEdit && !childLineEdit->placeholderText().isEmpty())
                        text = childLineEdit->placeholderText();
                }
                else if(childWidget->inherits("QGroupBox"))
                {
                    QGroupBox *groupBox = qobject_cast<QGroupBox*>(childWidget);
                    if(groupBox && !groupBox->title().isEmpty())
                        text = groupBox->title();
                }

                if(text == findText)
                {
                    if(!childWidget->isVisible())//Inside QTabWidget or QToolBox and its tab is not current index
                    {
                        QList<QWidget*> parents = widgetParents[childWidget];
                        for (int i = 0; i < parents.size(); ++i)
                        {
                            QWidget *p = parents.at(i);

                            if(p->inherits("QTabWidget"))
                            {
                                QTabWidget *tabWidget = qobject_cast<QTabWidget*>(p);
                                if(tabWidget)
                                {
                                    QWidget *w = parents.at(i-2);
                                    int index = tabWidget->indexOf(w);
                                    if(index != -1)
                                        tabWidget->setCurrentIndex(index);
                                }
                            }

                            if(p->inherits("QToolBox"))
                            {
                                QToolBox *toolBox = qobject_cast<QToolBox*>(p);
                                if(toolBox)
                                {
                                    QWidget *w = parents.at(i-3);
                                    int index = toolBox->indexOf(w);
                                    if(index != -1)
                                        toolBox->setCurrentIndex(index);
                                }
                            }
                        }
                    }

                    //Inside Scroll Area ----> scroll to widget
                    if(widgetParents.contains(childWidget) && widgetParents[childWidget].size() > 3)
                    {
                        QScrollArea *scrollArea = qobject_cast<QScrollArea*>(widgetParents[childWidget].at(2));
                        if(scrollArea)
                            scrollArea->ensureWidgetVisible(childWidget);
                     }

                    if(childWidget->inherits("QLabel"))
                    {
                        QLabel *label = qobject_cast<QLabel*>(childWidget);
                        if(label)
                        {
                            if(label->buddy())
                                label->buddy()->setFocus();
                        }
                        else
                            label->setFocus();
                    }
                    else
                        childWidget->setFocus();

//                    QString lableStyleSheet = label->styleSheet();
                    childWidget->setStyleSheet(QString(R"(
                                                       #%0{
                                                       background-color: rgba(255,0,0,50)
                                                       }
                                                       )").arg(childWidget->objectName()));

                    QTimer::singleShot(1000, [=]()
                    {
                        childWidget->setStyleSheet("");
                    });
                }
            }
        };

        //Completer
        QCompleter *completer = new QCompleter(widgetsTexts, this);
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        QAbstractItemView *popup = completer->popup();
        popup->setStyleSheet(R"(
                             QScrollBar:horizontal {
                             height: 12px;
                             margin: 2px 0px;
                             border: 1px transparent #2A2929;
                             border-radius: 2px;
                             background-color: rgb(220, 220, 220);
                             }

                             QScrollBar::handle:horizontal{
                             background-color: rgb(86, 86, 86);
                             min-height: 12px;
                             height: 12px;
                             border-radius: 3px;
                             }

                             QScrollBar::handle:horizontal:hover
                             {
                             background-color: rgb(120, 120, 120);
                             }

                             QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal
                             {
                             background: none;
                             }


                             QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
                             {
                             background: none;
                             }

                             QScrollBar::add-line:horizontal
                             {
                             width: 0px;
                             background: none;
                             border: none;
                             }

                             QScrollBar::sub-line:horizontal
                             {
                             width: 0px;
                             background: none;
                             border: none;
                             }

                             QScrollBar:vertical
                             {
                             width: 12px;
                             margin: 0px 2px;
                             border: 1px #2A2929;
                             border-radius: 3px;
                             background-color: rgb(220, 220, 220);
                             }

                             QScrollBar::handle:vertical
                             {
                             background-color: rgb(86, 86, 86);
                             min-width: 12px;
                             width: 12px;
                             border-radius: 3px;
                             }

                             QScrollBar::handle:vertical:hover
                             {
                             background-color: rgb(120, 120, 120);
                             }

                             QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical
                             {
                             background: none;
                             }


                             QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
                             {
                             background: none;
                             }

                             QScrollBar::add-line:vertical
                             {
                             width: 0px;
                             background: none;
                             border: none;
                             }

                             QScrollBar::sub-line:vertical
                             {
                             width: 0px;
                             background: none;
                             border: none;
                             }
                             )");

        ui->lineEdit->setCompleter(completer);

        connect(ui->lineEdit, &QLineEdit::editingFinished, [=]()
        {
            find();
        });

        connect(completer, QOverload<const QString &>::of(&QCompleter::activated), [=]()
        {
            find();
        });
    }
}

FindWidgetDialog::~FindWidgetDialog()
{
    delete ui;
}


void FindWidgetDialog::mousePressEvent(QMouseEvent *)
{
    if(!ui->innerFrame->underMouse())
        close();
}
