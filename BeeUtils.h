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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id: BeeUtils.h 27 2010-06-14 17:02:33Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIUTILS_H
#define BEEBEEP_GUIUTILS_H

#include "Config.h"
class Chat;
class ChatMessage;


namespace Bee
{

  QString chatMessagesToText( const Chat& );
  QString formatMessage( const ChatMessage& );
  QString formatSystemMessage( const ChatMessage& );

  namespace Private
  {
    QString formatHtmlText( QString );
  }

}

#endif // BEEBEEP_GUIUTILS_H
