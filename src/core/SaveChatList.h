//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUISAVECHATLIST_H
#define BEEBEEP_GUISAVECHATLIST_H

#include "Config.h"


class SaveChatList : public QObject
{
  Q_OBJECT

public:
  explicit SaveChatList( QObject* parent = Q_NULLPTR );

  static bool canBeSaved();

signals:
  void operationCompleted();

public slots:
  bool save();
  bool autoSave();

protected:
  bool saveToFile( const QString&, bool silent_mode );
  bool saveChats( QDataStream*, bool silent_mode );

};

#endif // BEEBEEP_GUISAVECHATLIST_H
