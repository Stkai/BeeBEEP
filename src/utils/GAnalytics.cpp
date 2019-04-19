//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "GAnalytics.h"
#include "Settings.h"


GAnalytics::GAnalytics( QObject* parent )
 : QObject( parent )
{
  setObjectName( "BeeBEEP Analytics" );
  mp_manager = new QNetworkAccessManager( this );
  connect( mp_manager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( onReplyFinished( QNetworkReply* ) ) );
}

void GAnalytics::doPost()
{
  QUrl ga_url = QUrl( Settings::instance().gaUrl() );
  QNetworkProxyQuery npq = QNetworkProxyQuery( ga_url );
  QNetworkProxy np = Settings::instance().systemNetworkProxy( npq );
  if( np.type() != QNetworkProxy::NoProxy )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "uses network proxy" << qPrintable( np.hostName() ) << ":" << np.port();
#endif
    mp_manager->setProxy( np );
  }

  QNetworkRequest req( ga_url );
  req.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );

#if QT_VERSION >= 0x050000
  QUrlQuery query;
#else
  QUrl query;
#endif

  // Please refers to: https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters
  query.addQueryItem("v", Settings::instance().gaEventVersion() );
  query.addQueryItem("tid", Settings::instance().gaTrackingId() );
  query.addQueryItem("cid", Settings::instance().applicationUuid() );
  query.addQueryItem("aip", "1" ); // anonymized IP
  query.addQueryItem("ds", "app" );
  query.addQueryItem("an", Settings::instance().programName().toLower() );
  query.addQueryItem("av", Settings::instance().version( false, false ) );
  QString user_language = QLocale::system().name().toLower();
  if( !user_language.isEmpty() )
    query.addQueryItem( "ul", user_language );
  query.addQueryItem( "t", "event" );
  query.addQueryItem( "ec", Settings::instance().programName() );
  query.addQueryItem( "ea", "usage" );
#ifdef BEEBEEP_DEBUG
  query.addQueryItem( "el", QString( "%1-%2" ).arg( "zz_test" )
                                              .arg( Settings::instance().version( false, false ) ) );
#else
  query.addQueryItem( "el", QString( "%1-%2" ).arg( Settings::instance().operatingSystem( false ).toLower() )
                                              .arg( Settings::instance().version( false, false ) ) );
#endif

  qint64 days_used = qMax( static_cast<qint64>(0), static_cast<qint64>(Settings::instance().settingsCreationDate().daysTo( QDate::currentDate() ) ) + 1 );
  query.addQueryItem( "ev", QString::number( days_used ) );

#if QT_VERSION >= 0x050000
  QByteArray query_data = query.query().toLatin1();
#else
  QByteArray query_data = query.encodedQuery();
#endif

  qDebug() << "Posting anonymous data to BeeBEEP statistics:" << query_data;
  QNetworkReply* reply = mp_manager->post( req, query_data );
#ifndef QT_NO_SSL
  connect( reply, SIGNAL( sslErrors( const QList<QSslError>& ) ), this, SLOT( onSslErrors( const QList<QSslError>& ) ) );
#endif
}

void GAnalytics::onReplyFinished( QNetworkReply *reply )
{
#ifdef BEEBEEP_DEBUG
  if( reply->error() != QNetworkReply::NoError )
    qWarning() << qPrintable( objectName() ) << "has error:" << reply->errorString();
  else
    qDebug() << qPrintable( objectName() ) << "has finished";
#endif

  if( reply->error() == QNetworkReply::NoError )
    Settings::instance().setStatsPostDate( QDate::currentDate() );

  reply->deleteLater();
  emit jobFinished();
}

void GAnalytics::onSslErrors( const QList<QSslError>& ssl_errors )
{
#ifndef QT_NO_SSL
  foreach( QSslError ssl_error, ssl_errors )
    qWarning() << qPrintable( objectName() ) << "has found SSL error:" << qPrintable( ssl_error.errorString() );
#else
  Q_UNUSED( ssl_errors );
#endif
}
