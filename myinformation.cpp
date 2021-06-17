#include "login.h"
#include "myinformation.h"
#include "ui_myinformation.h"
#include <QMEssageBox>
#include <qDebug>
#include <QSqlQuery>
#include <QDateTime>
#include "main.cpp"
#include "edit.h"
#include "updatepassword.h"
myinformation::myinformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myinformation)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("多功能书籍管理系统"));
    BG=new QButtonGroup(this);
    model = new QSqlTableModel(this);
    model->setTable("mybooks");
    setTitle();
    model->select();
    model->removeColumn(10);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setColumnWidth(6,110);
    ui->tableView->setColumnWidth(8,180);
    ui->tableView->setColumnWidth(9,110);
    ui->tableView->setColumnWidth(10,110);
    QSqlQuery query;
    QString sql=QString("select * from users where number='%1'").arg(login::number),sex,warning;
    query.exec(sql);
    while(query.next())
    {
        ui->lineEdit_3->setText(query.value(1).toString());
        ui->lineEdit_4->setText(query.value(2).toString());
        ui->lineEdit_5->setText(query.value(3).toString());
        sex=query.value(4).toString();
        if(sex=="女")
            ui->radioButton->setChecked(true);
        else if(sex=="男")
            ui->radioButton_2->setChecked(true);
        ui->lineEdit_7->setText(query.value(5).toString());
        ui->lineEdit_8->setText(query.value(6).toString());
        warning=query.value(7).toString();
        if(warning=="yes")
            ui->label_10->setText("您有超期未归还的书籍，请及时归还！");
        else
            ui->label_10->setText("您暂无书籍超期未归还记录~");
    }
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
}
myinformation::~myinformation()
{
    delete ui;
}
void myinformation::on_borrowButton_clicked()
{
    ui->label_9->clear();
    QString id=ui->lineEdit->text();
    int int_id=id.toInt();
    QSqlQuery query;
    QString status;
    QString sql0=QString("select * from books where id=%0").arg(int_id);
    query.exec(sql0);
    while(query.next())
    {
        status=query.value(7).toString();
    }
    if(isid(int_id)){
        if(status=="未借出"){
            QString sql=QString("select warning from users where number='%1'").arg(login::number);
            query.exec(sql);
            while(query.next())
            {
                if(query.value(0).toString()!="yes"){
                    QString sql1=QString("insert into userbooks(user_id,book_id,borrow_date,return_date,is_return"
                                         ") values(%1,%2,now(),date_add(now(),interval 1 month),'否')").arg(login::id).arg(int_id);
                    QString sql2=QString("update books set status='已借出' where id=%0").arg(int_id);
                    if(query.exec(sql1)&&query.exec(sql2)){
                        model->submitAll();
                        model->setTable("mybooks");
                        setTitle();
                        model->select();
                        model->removeColumn(10);
                        QMessageBox::information(NULL,"借书成功","借书成功！");
                    }
                    else{
                        QMessageBox::warning(NULL,"数据库出错","数据库出错！");
                    }
                }
                else{
                    QMessageBox::warning(NULL,"借书失败","您有超期的图书还未归还，不能借书！");
                }
            }
        }
        else{
            QMessageBox::warning(NULL,"借书失败","该图书已被借出！");
        }
    }
    else{
        QMessageBox::warning(NULL,"借书失败","填写的id不符合规定！");
    }
}
void myinformation::on_refreshButton_clicked()
{
    model->setTable("mybooks");
    setTitle();
    model->select();
    ui->label_9->clear();
}
void myinformation::on_returnButton_clicked()
{
    QString id=ui->lineEdit_2->text();
    int int_id=id.toInt();
    QSqlQuery query;
    QString sql=QString("update userbooks set is_return='是' where book_id=%2").arg(int_id);
    QString sql2=QString("update books set status='未借出' where id=%0").arg(int_id);
    if(isid2(int_id)){
        QDateTime current_date_time =QDateTime::currentDateTime();
        int timeT = current_date_time.toTime_t();
        int ndaysec = 24*60*60;
        QString sql3=QString("select * from userbooks where book_id=%2 and return_date-now()<0 and is_return='否'").arg(int_id);
        query.exec(sql3);
        while(query.next())
        {
            if(query.size()==1){
                QDateTime return_date=query.value(4).toDateTime();
                int timeT2=return_date.toTime_t();
                int day=(timeT-timeT2)/(ndaysec)+((timeT-timeT2)%(ndaysec)+(ndaysec-1))/(ndaysec)-1;
                QString str_day=QString::number(day);
                ui->label_9->setText("本书归还时间已超期"+str_day+"天！");
                break;
            }
        }
        if(query.exec(sql)&&query.exec(sql2)){
            checkwarning2();
            model->submitAll();
            model->setTable("mybooks");
            setTitle();
            model->select();
            model->removeColumn(10);
            QMessageBox::information(NULL,"还书成功","还书成功！");
        }
        else
            QMessageBox::warning(NULL,"数据库出错","数据库出错！");
    }
    else{
        QMessageBox::warning(NULL,"还书失败","所填写的id不符合规定！");
    }
}
void myinformation::on_update_infor_Button_clicked()
{
    QString password=ui->lineEdit_4->text();
    QString name=ui->lineEdit_5->text();
    QString sex;
    QString department= ui->lineEdit_7->text();
    QString grade=ui->lineEdit_8->text();
    QSqlQuery query;
    BG->addButton(ui->radioButton,0);
    BG->addButton(ui->radioButton_2,1);
    switch (BG->checkedId()) {
    case 0:
        sex="女";
        break;
    case 1:
        sex="男";
        break;
    }
    if(name!=NULL&&sex!=NULL&&department!=NULL&&grade!=NULL)
    {
        QString sql=QString("update users set password='%2',name='%3',sex='%4',department='%5',grade='%6' where number='%1'").arg(login::number).arg(password).arg(name).arg(sex).arg(department).arg(grade);
        qDebug()<<"sql:"<<sql;
        if(query.exec(sql)){
            QMessageBox::information(this, "修改成功", "修改成功！");
        }
        else{
            QMessageBox::warning(this, "修改失败", "修改失败！");
        }
    }
    else
    {
        QMessageBox::warning(NULL,"修改失败","信息不能为空！");
    }
}
void myinformation::on_tableView_doubleClicked(const QModelIndex &index)
{
    int curRow=ui->tableView->currentIndex().row();
    QAbstractItemModel *modessl = ui->tableView->model();
    QModelIndex indextemp;
    QVariant data;
    QString infor[10];
    QDateTime datetime;
    QString datetime_str1,datetime_str2,datetime_str3;
    for(int i=0;i<10;i++){
        indextemp=modessl->index(curRow,i);
        data=modessl->data(indextemp);
        if(i==4){
            datetime=data.toDateTime();
            datetime_str1=datetime.toString("yyyy-MM-dd hh:mm:ss");
        }
        else if(i==7){
            datetime=data.toDateTime();
            datetime_str2=datetime.toString("yyyy-MM-dd hh:mm:ss");
        }
        else if(i==8){
            datetime=data.toDateTime();
            datetime_str3=datetime.toString("yyyy-MM-dd hh:mm:ss");
        }
        else{
            infor[i]=data.toString();
        }
    }
    QString show=QString("书籍序号："+infor[0]+"\n名称："+infor[1]+"\n作者："+infor[2]+"\n类别："+infor[3]+"\n出版日期："+datetime_str1+"\n出版社："+infor[5]+"\n摘要："+infor[6]+"\n借出时间："+datetime_str2+"\n应归时间："+datetime_str3+"\n是否归还："+infor[9]);
    QMessageBox::about(NULL,"详情",show);
}
void myinformation::on_change_password_Button_clicked()
{
    updatepassword* w=new updatepassword();
    w->show();
}
bool myinformation::isid(int id)
{
    bool res=false;
    QString sql=QString("select id from books");
    QSqlQuery query;
    query.exec(sql);
    while(query.next())
    {
        if(id==query.value(0).toInt()){
            res= true;
            break;
        }
    }
    return res;
}
bool myinformation::isid2(int id)
{
    bool res=false;
    QString sql=QString("select book_id from userbooks where user_id=%1 and is_return='否'").arg(login::id);
    QSqlQuery query;
    query.exec(sql);
    while(query.next()){
        if(id==query.value(0).toInt()){
            res= true;
            break;
        }
    }
    return res;
}
void myinformation::setTitle()
{
    model->setHeaderData(0,Qt::Horizontal, "书籍序号");
    model->setHeaderData(1,Qt::Horizontal, "书籍名称");
    model->setHeaderData(2,Qt::Horizontal, "书籍作者");
    model->setHeaderData(3,Qt::Horizontal, "书籍类别");
    model->setHeaderData(4,Qt::Horizontal, "书籍出版时间");
    model->setHeaderData(5,Qt::Horizontal, "书籍出版社");
    model->setHeaderData(6,Qt::Horizontal, "书籍摘要");
    model->setHeaderData(7,Qt::Horizontal, "借出时间");
    model->setHeaderData(8,Qt::Horizontal, "应归时间");
    model->setHeaderData(9,Qt::Horizontal, "是否归还");
}
void myinformation::checkwarning2(){
    //在用户还书完成后检查，看此时，用户的warning是否将改动为no
    QSqlQuery query;
    QString sql=QString("select * from userbooks where user_id=%1 and is_return='否' and return_date-now()<0").arg(login::id);
    query.exec(sql);
    if(query.size()==0){
        QString sql1=QString("update users set warning ='no' where id=%0").arg(login::id);
        query.exec(sql1);
    }
}

void myinformation::on_quitButton_clicked()
{
    if (!(QMessageBox::information(this,tr("退出"),tr("确定退出吗?"),tr("Yes"),tr("No"))))
    {
        this->close();
    }
}


