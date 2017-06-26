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

#ifndef BEEBEEP_SHAREDESKTOP_H
#define BEEBEEP_SHAREDESKTOP_H

#include "ShareDesktopData.h"
class Chat;
class ShareDesktopJob;


class ShareDesktop : public QObject
{
  Q_OBJECT

public:
  explicit ShareDesktop( QObject* parent = 0 );

  bool start();
  void stop();
  inline bool isActive() const;

  bool addUserId( VNumber );
  inline bool removeUserId( VNumber );
  inline bool hasUsers() const;
  inline const QList<VNumber>& userIdList() const;
  inline const ShareDesktopData& lastImageData() const;
  void setUserReadImage( VNumber );
  void resetUserReadImage( VNumber );
  inline bool hasUserReadImage( VNumber ) const;

signals:
  void imageDataAvailable( const ShareDesktopData& );
  void imageAvailable( const QImage& );

protected slots:
  void onImageDataAvailable( const QByteArray&, const QString& image_type, bool use_compression, unsigned int diff_color );
  void makeScreenshot();

private:
  QList<VNumber> m_userIdList;
  QTimer m_timer;
  ShareDesktopJob* mp_job;
  ShareDesktopData m_lastImageData;
  QList<VNumber> m_userIdReadList;

};

// Inline Functions
inline bool ShareDesktop::removeUserId( VNumber user_id ) { return m_userIdList.removeOne( user_id ); }
inline bool ShareDesktop::hasUsers() const { return !m_userIdList.isEmpty(); }
inline const QList<VNumber>& ShareDesktop::userIdList() const { return m_userIdList; }
inline bool ShareDesktop::isActive() const { return m_timer.isActive(); }
inline const ShareDesktopData& ShareDesktop::lastImageData() const { return m_lastImageData; }
inline bool ShareDesktop::hasUserReadImage( VNumber user_id ) const { return m_userIdReadList.contains( user_id ); }

#endif // BEEBEEP_SHAREDESKTOP_H
