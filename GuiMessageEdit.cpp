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
// $Id: GuiMessageEdit.cpp 37 2010-07-20 17:57:07Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "GuiMessageEdit.h"
#include "Settings.h"


GuiMessageEdit::GuiMessageEdit( QWidget* parent )
  : QTextEdit( parent )
{
}

void GuiMessageEdit::keyPressEvent( QKeyEvent* e )
{
  bool reset_font_color = false;
  if( e->key() == Qt::Key_Return )
  {
    Qt::KeyboardModifiers mods = e->modifiers();
    if( mods & Qt::ControlModifier || mods & Qt::ShiftModifier )
    {
      insertPlainText( "\n" );
      e->accept();
      return;
    }
    else
    {
      emit returnPressed();
      e->accept();
      return;
    }
  }

  if( e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete || e->key() == Qt::Key_Cancel )
    reset_font_color = true;

  QTextEdit::keyPressEvent( e );

  // Fixed: when the text is fully cancelled the message box loose the color... patched with the line below
  if( reset_font_color )
    setTextColor( QColor( Settings::instance().chatFontColor() ) );
}
