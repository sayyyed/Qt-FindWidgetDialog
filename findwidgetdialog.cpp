#include "findwidgetdialog.h"
#include "ui_findwidgetdialog.h"
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QCompleter>
#include <QTimer>
#include <QDebug>
#include <QToolBox>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QDesktopWidget>

FindWidgetDialog::FindWidgetDialog(QWidget *targetWidget,
                                   const QString &findLabelText,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindWidgetDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    resize(QApplication::desktop()->availableGeometry().width(),
           QApplication::desktop()->availableGeometry().height());

    ui->findLabel->setText(findLabelText.endsWith(":") ? findLabelText : (findLabelText+":"));

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 0.5);
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
        else if(childWidget->inherits("QTextEdit"))
        {
            QTextEdit *childTextEdit = qobject_cast<QTextEdit*>(childWidget);
            if(childTextEdit && !childTextEdit->placeholderText().isEmpty())
                widgetsTexts << childTextEdit->placeholderText();
        }
        else if(childWidget->inherits("QTextEdit"))
        {
            QTextEdit *childTextEdit = qobject_cast<QTextEdit*>(childWidget);
            if(childTextEdit && !childTextEdit->placeholderText().isEmpty())
                widgetsTexts << childTextEdit->placeholderText();
        }
        else if(childWidget->inherits("QPlainTextEdit"))
        {
            QPlainTextEdit *childPlainTextEdit = qobject_cast<QPlainTextEdit*>(childWidget);
            if(childPlainTextEdit && !childPlainTextEdit->placeholderText().isEmpty())
                widgetsTexts << childPlainTextEdit->placeholderText();
        }

        if(childWidget->parentWidget())
        {
            QWidget *p = childWidget;
            while (p != targetWidget)
            {
                p = p->parentWidget();
                widgetParents[childWidget] << p;
            }
        }
    }

    if(!widgetsTexts.isEmpty())
    {
        //Completer
        QCompleter *completer = new QCompleter(widgetsTexts, this);
        //completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);
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
                else if(childWidget->inherits("QTextEdit"))
                {
                    QTextEdit *childTextEdit = qobject_cast<QTextEdit*>(childWidget);
                    if(childTextEdit && !childTextEdit->placeholderText().isEmpty())
                        text = childTextEdit->placeholderText();
                }
                else if(childWidget->inherits("QPlainTextEdit"))
                {
                    QPlainTextEdit *childPlainTextEdit = qobject_cast<QPlainTextEdit*>(childWidget);
                    if(childPlainTextEdit && !childPlainTextEdit->placeholderText().isEmpty())
                        text = childPlainTextEdit->placeholderText();
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
                        int scrollAreaParentIndex = 2;
                        if(childWidget->inherits("QPlainTextEdit") || childWidget->inherits("QTextEdit"))
                            scrollAreaParentIndex = 3;

                        QScrollArea *scrollArea = qobject_cast<QScrollArea*>(widgetParents[childWidget].at(scrollAreaParentIndex));
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

                    break;
                }
            }
        };

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

    //Animations
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(400);
    animation->setEasingCurve(QEasingCurve::InOutBack);
    animation->setStartValue(QRect(0, -QApplication::desktop()->availableGeometry().height(),
                                   QApplication::desktop()->availableGeometry().width(),
                                   QApplication::desktop()->availableGeometry().height()));
    animation->setEndValue(QRect(0, 0,
                                 QApplication::desktop()->availableGeometry().width(),
                                 QApplication::desktop()->availableGeometry().height()));

    animation->start(QAbstractAnimation::DeleteWhenStopped);

    //Close animation
    m_closeAnimation = new QPropertyAnimation(this, "geometry", this);
    m_closeAnimation->setDuration(400);
    m_closeAnimation->setEasingCurve(QEasingCurve::InOutBack);
    m_closeAnimation->setStartValue(QRect(0, 0,
                                 QApplication::desktop()->availableGeometry().width(),
                                 QApplication::desktop()->availableGeometry().height()));
    m_closeAnimation->setEndValue(QRect(0, -QApplication::desktop()->availableGeometry().height(),
                                   QApplication::desktop()->availableGeometry().width(),
                                   QApplication::desktop()->availableGeometry().height()));

    connect(m_closeAnimation, &QPropertyAnimation::finished, this, &FindWidgetDialog::close);
}

FindWidgetDialog::~FindWidgetDialog()
{
    delete ui;
}

void FindWidgetDialog::mousePressEvent(QMouseEvent *)
{
    if(!ui->innerFrame->underMouse())
        m_closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
