#ifndef FINDWIDGETINSIDEDIALOG_H
#define FINDWIDGETINSIDEDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>

namespace Ui {
class FindWidgetDialog;
}

class FindWidgetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindWidgetDialog(QWidget *targetWidget,
                              const QString &findLabelText = tr("Find:"),
                              QWidget *parent = 0);
    ~FindWidgetDialog();

private:
    Ui::FindWidgetDialog *ui;
    QPropertyAnimation *m_closeAnimation = nullptr;

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // FINDWIDGETINSIDEDIALOG_H
