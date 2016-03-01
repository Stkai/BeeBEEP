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
  setObjectName( "GAnalytics" );
  mp_manager = new QNetworkAccessManager( this );
  connect( mp_manager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( onReplyFinished( QNetworkReply* ) ) );
}

void GAnalytics::doPost()
{
  QUrl ga_url = QUrl( Settings::instance().gaUrl() );
  QList<QNetworkProxy> proxy_list = QNetworkProxyFactory::systemProxyForQuery( QNetworkProxyQuery( ga_url ) );
  if( !proxy_list.isEmpty() )
  {
    QNetworkProxy np = proxy_list.takeFirst();
    if( np.type() != QNetworkProxy::NoProxy )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << qPrintable( objectName() ) << "uses network proxy" << qPrintable( np.hostName() ) << ":" << np.port();
#endif
      mp_manager->setProxy( np );
    }
  }

  QNetworkRequest req( ga_url );
  req.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );

#if QT_VERSION >= 0x050000
  QUrlQuery query;
#else
  QUrl query;
#endif
  query.addQueryItem("v", Settings::instance().gaEventVersion() );
  query.addQueryItem("tid", Settings::instance().gaTrackingId() );
  query.addQueryItem("cid", Settings::instance().applicationUuid() );
  query.addQueryItem( "t", "event" );
  query.addQueryItem( "ec", Settings::instance().programName() );
  query.addQueryItem( "ea", "usage" );
  query.addQueryItem( "el", QString( "%1-%2" ).arg( Settings::instance().operatingSystem( false ).toLower() )
                                              .arg( Settings::instance().version( false ) ) );

  qint64 days_used = qMax( (qint64)0, Settings::instance().applicationUuidCreationDate().daysTo( QDate::currentDate() ) ) + 1;
  query.addQueryItem( "ev", QString::number( days_used ) );

#if QT_VERSION >= 0x050000
  QByteArray query_data = query.query().toLatin1();
#else
  QByteArray query_data = query.encodedQuery();
#endif

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "posts query:" << qPrintable( query_data );
#endif

  mp_manager->post( req, query_data );
}

void GAnalytics::onReplyFinished( QNetworkReply *reply )
{
#ifdef BEEBEEP_DEBUG
  QUrl url = reply->url();
  if( reply->error() != QNetworkReply::NoError )
    qWarning() << qPrintable( objectName() ) << "has error:" << reply->errorString();
  else
    qDebug() << qPrintable( objectName() ) << "has finished";
#endif
  reply->deleteLater();
  emit jobFinished();
}
