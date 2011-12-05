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

#include "GuiChatGraphicsScene.h"
#include "GuiChatGraphicsView.h"


GuiChatGraphicsView::GuiChatGraphicsView( QWidget* parent )
  : QGraphicsView( parent )
{
  setAlignment( Qt::AlignHCenter | Qt::AlignTop );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setFrameStyle( QFrame::NoFrame );
}

void GuiChatGraphicsView::setChatGraphicsScene( GuiChatGraphicsScene* scene )
{
  m_scene = scene;
  setScene( m_scene );
}

void GuiChatGraphicsView::addMessage( const ChatMessage& cm )
{
  if( m_scene )
    m_scene->addMessage( cm );

  QRectF rect = scene()->sceneRect();
  rect.adjust(-4, -4, 4, 4);
  setSceneRect( rect );

  rect = sceneRect();
  rect.setTop( sceneRect().height() - 20 );
  rect.setWidth( 20 );
  ensureVisible( rect, 50, 50 );
}

void GuiChatGraphicsView::resizeEvent(QResizeEvent *event)
{
// pass this to scene
  m_scene->setWidthResize( event->size().width(), event->oldSize().width() );
  QGraphicsView::resizeEvent( event );

  QRectF rect = scene()->sceneRect();
  rect.adjust(-4, -4, 4, 4);
  setSceneRect( rect );
}
