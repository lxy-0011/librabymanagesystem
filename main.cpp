#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
void checkwarning1();

int main(int argc, char *argv[])
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QSqlQuery query;
    query = QSqlQuery(db);
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("librarymanagesystem");//数据库名称
    db.setUserName("root");//用户名
    db.setPassword("root");//密码
    db.open();//连接数据库
    db.exec("SET NAMES 'GBK'");
    checkwarning1();
    QApplication a(argc, argv);
    login w;
    w.show();
    return a.exec();
}
void checkwarning1(){
    //在程序启动时检查，随着时间的推移，用户是否有超期记录，如果有，则将warning字段设置为yes
    QSqlQuery query;
    QString sql=QString("select user_id from userbooks where is_return='否' and return_date-now()<0");//未归还且超期了
    query.exec(sql);
    if(query.size()!=0){//存在超期的用户
        while(query.next()){
            QString sql1=QString("update users set warning='yes' where id=%0").arg(query.value(0).toInt());//warning改为yes
            query.exec(sql1);
        }
    }

}

