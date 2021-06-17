#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlTableModel>
#include <QTableView>
#include "tabviewdelegate.h"
#include "edit.h"
#include <QDateTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("多功能书籍管理系统"));
    model = new QSqlTableModel(this);
    model->setTable("books");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model2 = new QSqlTableModel(this);
    model2->setTable("users");
    model2->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model2->select();

    setTitle();
    ReadOnlyDelegate * readOnlyDelegate = new ReadOnlyDelegate();

    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setItemDelegateForColumn(0, readOnlyDelegate);
    ui->tableView->setColumnWidth(4,110);
    ui->tableView->setColumnWidth(6,150);

    ui->tableView_2->setModel(model2);
    ui->tableView_2->verticalHeader()->setVisible(false);
    ui->tableView_2->setItemDelegateForColumn(0, readOnlyDelegate);
    ui->tableView_2->setColumnWidth(1,100);

    tabviewDelegate *textedit = new tabviewDelegate(this);
    //textedit->type = "introduction";
    ui->tableView->setItemDelegateForColumn(6,textedit);

    tabviewDelegate *cmb = new tabviewDelegate(this);
    cmb->type = "sex";
    ui->tableView_2->setItemDelegateForColumn(4,cmb);

    tabviewDelegate *cmb2 = new tabviewDelegate(this);
    cmb2->type = "status";
    ui->tableView->setItemDelegateForColumn(7,cmb2);

    tabviewDelegate *cmb3 = new tabviewDelegate(this);
    cmb3->type = "warning";
    ui->tableView_2->setItemDelegateForColumn(7,cmb3);

    tabviewDelegate *cmb4 = new tabviewDelegate(this);//选择学院
    cmb4->type = "department";
    ui->tableView_2->setItemDelegateForColumn(5,cmb4);

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_addButton_clicked()
{
    int rowNum = model->rowCount();
    model->insertRows(rowNum,1);
}
void MainWindow::on_deleteButton_clicked()
{
    int curRow = ui->tableView->currentIndex().row();
    QString status=model->data(model->index(curRow,7)).toString();
    if(status=="未借出"){
        model->removeRow(curRow);
        int ok = QMessageBox::warning(this,tr("删除当前行!"),
                                      tr("你确定删除当前行吗？ "),QMessageBox::Yes, QMessageBox::No);
        if(ok == QMessageBox::No)
        {
            model->revertAll();
        } else {
            if(model->submitAll()){
                QMessageBox::information(this, "删除成功","删除成功");
            }
            else{
                QMessageBox::warning(this, tr("数据库错误"),tr("数据库错误!"));
            }
        }
    }
    else{
        QMessageBox::warning(NULL,"删除失败","该书籍已被借出，不能删除！");
    }
}
void MainWindow::on_updateButton_clicked()
{
    model->database().transaction();
    if(model->submitAll()){
        model->database().commit();
        QMessageBox::information(NULL,"修改成功","修改成功！");
    }
    else{
        model->database().rollback();
        QMessageBox::warning(this, tr("数据库错误"),
                             tr("数据库错误!"));
    }
}
void MainWindow::on_updateButton2_clicked()
{
    model2->database().transaction();
    int curRow = ui->tableView_2->currentIndex().row();
    QSqlQuery query;
    int id=model2->data(model2->index(curRow,0)).toInt();
    QString number= model2->data(model2->index(curRow,1)).toString();
    QString password= model2->data(model2->index(curRow,2)).toString();
    QString sql=QString("select number,password from users where id=%0").arg(id);
    QString sql_number,sql_password;
    query.exec(sql);
    while(query.next())
    {
        sql_number=query.value(0).toString();
        sql_password=query.value(1).toString();
    }
    if(number==sql_number&&password==sql_password){
        if(model2->submitAll()){
            model2->database().commit();
            QMessageBox::information(NULL,"修改成功","修改成功！");
        }
        else{
            model2->database().rollback();
            QMessageBox::warning(this, tr("数据库错误"),
                                 tr("数据库错误！"));
        }
    }
    else{
        QMessageBox::warning(NULL,"修改失败","没有修改用户名和密码的权限！");
    }
}
void MainWindow::on_addButton2_clicked()
{
    int rowNum = model2->rowCount();
    model2->insertRows(rowNum,1);
}
void MainWindow::on_deleteButton2_clicked()
{
    QSqlQuery query;
    int curRow = ui->tableView_2->currentIndex().row();
    QString number=model2->data(model2->index(curRow,1)).toString(),number2;
    QString sql=QString("select number from userbooks where number='%1'").arg(number);
    query.exec(sql);
    while(query.next())
    {
        number2=query.value(0).toString();
    }
    if(number=="2017141461000"){
        QMessageBox::warning(NULL,"删除失败","不能删除管理员！");
    }
    else{
        if(number2.length()==0){
            model2->removeRow(curRow);
            int ok = QMessageBox::warning(this,tr("删除当前行!"),
                                          tr("你确定删除当前行吗？ "),QMessageBox::Yes, QMessageBox::No);
            if(ok == QMessageBox::No)
            {
                model2->revertAll();
            } else {
                model2->submitAll();
            }
        }
        else{
            QMessageBox::warning(NULL,"删除失败","该用户还有未还书籍，不能删除！");
        }
    }
}
void MainWindow::on_searshButton_clicked()
{
    QString name = ui->lineEdit_7->text();
    QString author=ui->lineEdit_8->text();
    QSqlQuery query;
    int count=0,count2=0;
    if(name.length()!=0&&author.length()==0){
        model->setFilter(QString("name like '%%1%'").arg(name));
        model->select();
        QString sql=QString("select * from books where name like '%%1%'").arg(name);
        query.exec(sql);
        while(query.next())
        {
            count++;
            if(query.value(7).toString()=="未借出"){
                count2++;
            }

        }
        //这里要进行类型的转换，把int转化为QString，因为label\lineEdit这种都默认显示QString类型的数据
        QString str_count=QString::number(count);
        QString str_count2=QString::number(count2);
        ui->label_4->setText(str_count);
        ui->label_5->setText(str_count2);
    }else if(author.length()!=0&&name.length()==0){
        model->setFilter(QString("author like '%%2%'").arg(author));
        model->select();
        QString str_count=QString::number(count);
        QString str_count2=QString::number(count2);
        ui->label_4->setText(str_count);
        ui->label_5->setText(str_count2);
    }else if(name.length()!=0&&author.length()!=0){
        model->setFilter(QString("name like '%%1%' && author like '%%2%'").arg(name).arg(author));
        model->select();
        QString sql=QString("select * from books where name like '%%1%'&& author like '%%2%'").arg(name).arg(author);
        query.exec(sql);
        while(query.next())
        {
            count++;
            if(query.value(7).toString()=="未借出"){
                count2++;
            }

        }
        QString str_count=QString::number(count);
        QString str_count2=QString::number(count2);
        ui->label_4->setText(str_count);
        ui->label_5->setText(str_count2);
    }
    else{
        QMessageBox::warning(this, tr("Warning"),tr("不能两者都为空") );
    }
}
void MainWindow::on_searshButton_2_clicked()
{
    QString department = ui->lineEdit_9->text();
    QString grade=ui->lineEdit_10->text();
    if(department.length()!=0&&grade.length()==0){
        model2->setFilter(QString("department= '%5'").arg(department));
        model2->select();
    }else if(grade.length()!=0&&department.length()==0){
        model2->setFilter(QString("grade='%6'").arg(grade));
        model2->select();
    }else if(department.length()!=0&&grade.length()!=0){
        model2->setFilter(QString("department = '%5' && grade='%6'").arg(department).arg(grade));
        model2->select();
    }
    else{
        QMessageBox::warning(this, tr("Warning"),tr("不能两者都为空") );
    }
}
void MainWindow::on_showallButton_clicked()
{
    model->setTable("books");
    setTitle();
    model->select();
    ui->tableView->setColumnWidth(4,110);
    ui->tableView->setColumnWidth(6,150);
}
void MainWindow::on_showallButton_2_clicked()
{
    model2->setTable("users");
    setTitle();
    model2->select();
    ui->tableView_2->setColumnWidth(1,100);
}
void MainWindow::on_searchButton_3_clicked()
{
    model2->setFilter("warning='yes'");
    model2->select();
}
void MainWindow::on_checkaddButton_clicked()
{
    int rowNum = model->rowCount();
    model->setData(model->index(rowNum,0),rowNum+1);
    if(model->submitAll()){
        model->database().commit();
        QMessageBox::information(NULL,"添加成功","添加成功！");
    }
    else{
        model->database().rollback();
        QMessageBox::warning(this, tr("数据库错误"),
                             tr("数据库错误！"));
    }
}
void MainWindow::on_checkaddButton_2_clicked()
{
    int rowNum = model2->rowCount();
    model2->setData(model2->index(rowNum,0),rowNum+1);
    int curRow = ui->tableView_2->currentIndex().row();
    QString number= model2->data(model2->index(curRow,1)).toString();
    QString password= model2->data(model2->index(curRow,2)).toString();
    if(!edit::iscard(number)||!edit::isonly(number)){
        QMessageBox::warning(NULL,"添加失败","用户证号格式错误或该证号已被注册！");
    }
    else{
        if(!edit::ispassword(password)){
            QMessageBox::warning(NULL,"添加失败","密码格式错误！");
        }
        else{
            if(model2->submitAll()){
                QMessageBox::information(NULL,"添加成功","添加成功！");
            }
            else{
                QMessageBox::warning(this, tr("数据库错误"),
                                     tr("数据库错误!"));
            }
        }
    }
}
void MainWindow::setTitle()
{
    model->setHeaderData(0,Qt::Horizontal, "序号");
    model->setHeaderData(1,Qt::Horizontal, "书籍名称");
    model->setHeaderData(2,Qt::Horizontal, "书籍作者");
    model->setHeaderData(3,Qt::Horizontal, "书籍类别");
    model->setHeaderData(4,Qt::Horizontal, "书籍出版时间");
    model->setHeaderData(5,Qt::Horizontal, "书籍出版社");
    model->setHeaderData(6,Qt::Horizontal, "书籍摘要");
    model->setHeaderData(7,Qt::Horizontal, "书籍借还状态");
    model2->setHeaderData(0,Qt::Horizontal, "序号");
    model2->setHeaderData(1,Qt::Horizontal, "用户证号");
    model2->setHeaderData(2,Qt::Horizontal, "用户密码");
    model2->setHeaderData(3,Qt::Horizontal, "用户姓名");
    model2->setHeaderData(4,Qt::Horizontal, "用户性别");
    model2->setHeaderData(5,Qt::Horizontal, "用户学院");
    model2->setHeaderData(6,Qt::Horizontal, "用户年级");
    model2->setHeaderData(7,Qt::Horizontal, "用户警告状态");
}
void MainWindow::on_quitButton_clicked()
{
    if (!(QMessageBox::information(this,tr("退出"),tr("确定退出吗?"),tr("Yes"),tr("No"))))
    {
        this->close();
    }
}

