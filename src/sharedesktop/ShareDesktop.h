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

#include "Config.h"
class Chat;
class ShareDesktopJob;


class ShareDesktop : public QObject
{
  Q_OBJECT

public:
  explicit ShareDesktop( QObject* parent = 0 );

  bool start();
  void stop();
  bool isActive() const;

  bool addUserId( VNumber );
  inline bool removeUserId( VNumber );
  inline bool hasUsers() const;
  inline const QList<VNumber>& userIdList() const;

signals:
  void shareDesktopDataReady( const QByteArray& );

protected slots:
  void onJobCompleted();
  void onImageDataAvailable( const QByteArray& );

private:
  QList<VNumber> m_userIdList;
  ShareDesktopJob* mp_job;

};

// Inline Functions
inline bool ShareDesktop::removeUserId( VNumber user_id ) { return m_userIdList.removeOne( user_id ); }
inline bool ShareDesktop::hasUsers() const { return !m_userIdList.isEmpty(); }
inline const QList<VNumber>& ShareDesktop::userIdList() const { return m_userIdList; }

#endif // BEEBEEP_SHAREDESKTOP_H
