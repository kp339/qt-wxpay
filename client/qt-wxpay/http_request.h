#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QEventLoop>
#include <QByteArray>
#include <QVariant>


//Post http request
/*
  Example:
  Post("http://google.com/","header1=one&header2=two");
*/
QString Post(QString uri, QString header)
{
    QString result;
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    QUrl url(uri);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkReply *reply = manager.post(request, header.toUtf8());
    eventLoop.exec();
    // qDebug() << reply->readAll();
    result = reply->readAll();
    return result;
}

void Get(QString uri)
{
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QUrl url(uri);
    QNetworkRequest req(url);
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();
    qDebug() << "\n";
    qDebug() << reply->readAll();

}

#endif // HTTP_REQUEST_H
