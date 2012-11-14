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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUICHATGRAPHICSSCENE_H
#define BEEBEEP_GUICHATGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QList>
class ChatMessage;
class GuiChatGraphicsItem;


class GuiChatGraphicsScene : public QGraphicsScene
{
public:
  GuiChatGraphicsScene( QObject* parent = 0 );

  void addChatMessage( const ChatMessage& );

  void setWidthResize( int new_width, int old_width );
  void verticalReposition();
  inline void setBoxStartLength( int );

private:
  int m_verticalPosForNewMessage;
  int m_verticalSpacing;
  int m_boxStartLength;
  QList<GuiChatGraphicsItem*> m_items;

};


// Inline Functions
inline void GuiChatGraphicsScene::setBoxStartLength( int value ) { m_boxStartLength = value; }


#endif // BEEBEEP_GUICHATGRAPHICSSCENE_H

