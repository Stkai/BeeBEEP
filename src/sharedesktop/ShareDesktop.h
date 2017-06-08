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
class ShareDesktopJob;


class ShareDesktop : public QObject
{
  Q_OBJECT

public:
  explicit ShareDesktop( QObject* parent = 0 );

  void start();
  void stop();
  inline bool isActive() const;

  inline const QList<VNumber>& chatIdList() const;

  bool addChat( VNumber );
  inline bool hasChat( VNumber ) const;
  inline bool removeChat( VNumber );
  inline void clearChats();

signals:
  void shareDesktopDataReady( const QByteArray& );

protected slots:
  void onJobCompleted();
  void onImageDataAvailable( const QByteArray& );

private:
  QList<VNumber> m_chatIdList;
  ShareDesktopJob* mp_job;

};

// Inline Functions
inline bool ShareDesktop::isActive() const { return mp_job; }
inline bool ShareDesktop::hasChat( VNumber chat_id ) const { return m_chatIdList.contains( chat_id ); }
inline const QList<VNumber>& ShareDesktop::chatIdList() const { return m_chatIdList; }
inline bool ShareDesktop::removeChat( VNumber chat_id ) { return m_chatIdList.removeOne( chat_id ); }
inline void ShareDesktop::clearChats() { m_chatIdList.clear(); }

#endif // BEEBEEP_SHAREDESKTOP_H
