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

#include "EmoticonManager.h"
#include "GuiConfig.h"
#include "GuiEmoticons.h"


GuiEmoticons::GuiEmoticons( QWidget* parent )
  : QTabWidget( parent )
{
  setObjectName( "GuiEmoticons" );
  setTabPosition( QTabWidget::South );
}

QSize GuiEmoticons::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH + 4, 40 );
}

void GuiEmoticons::initEmoticons()
{
  QList<Emoticon> emoticon_list = EmoticonManager::instance().emoticons( true );
  emoticon_list.append( EmoticonManager::instance().emoticonsByGroup( Emoticon::People ) );
  addEmoticonTab( emoticon_list, QIcon( ":images/emoticon.png"), tr( "Smiley" ) );
  emoticon_list = EmoticonManager::instance().emoticonsByGroup( Emoticon::Objects );
  addEmoticonTab( emoticon_list, Emoticon::groupIcon( Emoticon::Objects ), tr( "Objects" ) );
  emoticon_list = EmoticonManager::instance().emoticonsByGroup( Emoticon::Nature );
  addEmoticonTab( emoticon_list, Emoticon::groupIcon( Emoticon::Nature ), tr( "Nature" ) );
  emoticon_list = EmoticonManager::instance().emoticonsByGroup( Emoticon::Places );
  addEmoticonTab( emoticon_list, Emoticon::groupIcon( Emoticon::Places ), tr( "Places" ) );
  emoticon_list = EmoticonManager::instance().emoticonsByGroup( Emoticon::Symbols );
  addEmoticonTab( emoticon_list, Emoticon::groupIcon( Emoticon::Symbols ), tr( "Symbols" ) );
}

void GuiEmoticons::addEmoticonTab( const QList<Emoticon>& emoticon_list, const QIcon& group_icon, const QString& group_name )
{
  GuiEmoticonWidget* emoticon_widget = new GuiEmoticonWidget( this );
  QScrollArea* scroll_area = new QScrollArea( this );
  scroll_area->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  scroll_area->setBackgroundRole( QPalette::Light );
  scroll_area->setWidgetResizable( true );

  QList<QPushButton*> button_list;
  QPushButton* emoticon_button = 0;

  foreach( Emoticon e, emoticon_list )
  {
    emoticon_button = new QPushButton( emoticon_widget );
    emoticon_button->setIconSize( QSize( 24, 24 ) );
    emoticon_button->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    emoticon_button->setFixedSize( QSize( 26, 26 ) );
    emoticon_button->setIcon( e.pixmap() );
    emoticon_button->setStyleSheet( "QPushButton:hover{ background-color: #ffcf04; }");
    emoticon_button->setObjectName( QString( "GuiEmoticonCode%1" ).arg( e.textToMatch() ) );
    connect( emoticon_button, SIGNAL( clicked() ), this, SLOT( emoticonClicked() ) );
    button_list.append( emoticon_button );
  }

  emoticon_widget->setEmoticonButtons( button_list );
  emoticon_widget->setObjectName( emoticon_widget->objectName() + group_name );
  scroll_area->setWidget( emoticon_widget );
  int tab_id = addTab( scroll_area, group_icon, "" );
  setTabToolTip( tab_id, group_name );
}

void GuiEmoticons::emoticonClicked()
{
  QObject* emoticon_object = sender();
  if( !emoticon_object )
    return;

  QString emoticon_code = emoticon_object->objectName();
  emoticon_code.remove( "GuiEmoticonCode" );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Emoticon clicked is" << emoticon_code;
#endif
  Emoticon e = EmoticonManager::instance().emoticon( emoticon_code );
  if( e.isValid() )
    emit( emoticonSelected( e ) );
}


/******************************
 * GuiEmoticonWidget
 *
 */

GuiEmoticonWidget::GuiEmoticonWidget( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiEmoticonWidget" );

  mp_layout = new QGridLayout( this );
  mp_layout->setHorizontalSpacing( 1 );
  mp_layout->setVerticalSpacing( 1 );
  mp_layout->setContentsMargins( 1, 1, 1, 1 );

  m_hasPainted = false;
}

void GuiEmoticonWidget::resizeEvent( QResizeEvent* e )
{
  QWidget::resizeEvent( e );
  clearLayout();
  if( !m_hasPainted )
    paintEmoticonButtons( e->size().width() );
}

void GuiEmoticonWidget::clearLayout()
{
  foreach( QWidget* w, m_buttons )
    mp_layout->removeWidget( w );
}

void GuiEmoticonWidget::paintEmoticonButtons( int box_width )
{
  int x = 0;
  int y = 0;
  int max_icons_per_row = qMax( 2, static_cast<int>((box_width-2) / 26) ) - 1;

  foreach( QWidget* w, m_buttons )
  {
    mp_layout->addWidget( w, y, x );
    x++;
    if( x > max_icons_per_row )
    {
      y++;
      x = 0;
    }
  }

  // tells to scroll area the size of the widget
  setMinimumSize( QSize( (max_icons_per_row+1) * 26 + 1, (y+1) * 26 + 1) );
}

