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
// $Id: GuiChat.h 113 2011-09-26 18:01:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "GuiChatGraphicsItem.h"
#include "GuiChatGraphicsScene.h"
#include "GuiChatGraphicsView.h"


GuiChatGraphicsScene::GuiChatGraphicsScene( QObject* parent )
  : QGraphicsScene( parent ), m_verticalPosForNewMessage( 0 ), m_verticalSpacing( 5 ),
    m_boxStartLength( 2 ), m_items()
{
}

void GuiChatGraphicsScene::addMessage( const ChatMessage& cm )
{
  GuiChatGraphicsItem* item = new GuiChatGraphicsItem();
  m_items.append( item );
  item->setBoxStartLength( m_boxStartLength );
  item->setViewWidth( 350 );
  item->setChatMessage( cm );
  item->setPos( 0, m_verticalPosForNewMessage );
  int height = item->boundingRect().height();
  m_verticalPosForNewMessage = m_verticalPosForNewMessage + height + m_verticalSpacing;
  addItem(item);

  QRectF rect = sceneRect();
  rect.setHeight( m_verticalPosForNewMessage) ;
  setSceneRect( rect );
}

void GuiChatGraphicsScene::setWidthResize(int newWidth, int oldWidth)
{
//    verticalReposition();
}

void GuiChatGraphicsScene::verticalReposition()
{
    m_verticalPosForNewMessage = 0;

    GuiChatGraphicsItem* item = 0;
    for(int i = 0; i < m_items.size(); ++i)
    {
        item = m_items.at(i);
        item->setViewWidth(views().at(0)->size().width());
        item->setPos(0, m_verticalPosForNewMessage);
        int height = item->boundingRect().height();
        m_verticalPosForNewMessage = m_verticalPosForNewMessage + height + m_verticalSpacing;
    }

    QRectF rect = sceneRect();
    if(item)
    {
        rect.setHeight(m_verticalPosForNewMessage);
        rect.setWidth(item->maxWidth() + item->boxStartLength() - 4);
        setSceneRect(rect);
    }
}


