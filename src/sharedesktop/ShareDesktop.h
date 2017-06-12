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

  bool start( const Chat& );
  void stop();
  bool isActive() const;

  inline bool hasChat() const;
  inline VNumber chatId() const;

  bool addUserId( VNumber );
  inline bool removeUserId( VNumber );
  inline const QList<VNumber>& userIdList() const;

signals:
  void shareDesktopDataReady( const QByteArray& );

protected slots:
  void onJobCompleted();
  void onImageDataAvailable( const QByteArray& );

protected:
  bool setChat( const Chat& );

private:
  VNumber m_chatId;
  QList<VNumber> m_userIdList;
  ShareDesktopJob* mp_job;

};

// Inline Functions
inline bool ShareDesktop::hasChat() const { return m_chatId != ID_INVALID; }
inline VNumber ShareDesktop::chatId() const { return m_chatId; }
inline bool ShareDesktop::removeUserId( VNumber user_id ) { return m_userIdList.removeOne( user_id ); }
inline const QList<VNumber>& ShareDesktop::userIdList() const { return m_userIdList; }

#endif // BEEBEEP_SHAREDESKTOP_H
