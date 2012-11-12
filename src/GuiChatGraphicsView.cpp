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

#include "GuiChatGraphicsView.h"
#include "GuiChatGraphicsScene.h"
#include <QResizeEvent>


GuiChatGraphicsView::GuiChatGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
}

void GuiChatGraphicsView::setChatGraphicsScene( GuiChatGraphicsScene* scene )
{
  mp_scene = scene;
  setScene( mp_scene );
}

void GuiChatGraphicsView::addChatMessage( const ChatMessage& chat_message )
{
  if( mp_scene )
    mp_scene->addChatMessage( chat_message );

  QRectF rect = scene()->sceneRect();
  rect.adjust(-4, -4, 4, 4);
  setSceneRect(rect);
  rect = sceneRect();
  rect.setTop(sceneRect().height() - 20);
  rect.setWidth(20);
  ensureVisible(rect, 50, 50);
}

void GuiChatGraphicsView::resizeEvent(QResizeEvent *event)
{
//  pass this to scene
    mp_scene->setWidthResize(event->size().width(), event->oldSize().width());
    QGraphicsView::resizeEvent(event);

    QRectF rect = scene()->sceneRect();
    rect.adjust(-4, -4, 4, 4);
    setSceneRect(rect);
}
