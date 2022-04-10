#ifndef FINDWIDGETINSIDEDIALOG_H
#define FINDWIDGETINSIDEDIALOG_H

#include <QDialog>

namespace Ui {
class FindWidgetDialog;
}

class FindWidgetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindWidgetDialog(QWidget *targetWidget,
                                    QWidget *parent = 0);
    ~FindWidgetDialog();

private:
    Ui::FindWidgetDialog *ui;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // FINDWIDGETINSIDEDIALOG_H
