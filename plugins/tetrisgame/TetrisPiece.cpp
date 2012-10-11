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

#include <QtGlobal>
#include "TetrisPiece.h"
#include "Random.h"


TetrisPiece::TetrisPiece()
{
  setShape( NoShape );
}

void TetrisPiece::setRandomShape()
{
  setShape( (Shape)Random::number( 1, NumShapes-1 ) );
}

void TetrisPiece::setShape( Shape s )
{
  static const int coords_table[8][4][2] =
  {
    { { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 } },
    { { 0, -1 },  { 0, 0 },   { -1, 0 },  { -1, 1 } },
    { { 0, -1 },  { 0, 0 },   { 1, 0 },   { 1, 1 } },
    { { 0, -1 },  { 0, 0 },   { 0, 1 },   { 0, 2 } },
    { { -1, 0 },  { 0, 0 },   { 1, 0 },   { 0, 1 } },
    { { 0, 0 },   { 1, 0 },   { 0, 1 },   { 1, 1 } },
    { { -1, -1 }, { 0, -1 },  { 0, 0 },   { 0, 1 } },
    { { 1, -1 },  { 0, -1 },  { 0, 0 },   { 0, 1 } }
  };

  for( int i = 0; i < 4 ; i++ )
  {
    for( int j = 0; j < 2; j++ )
      m_coords[i][j] = coords_table[s][i][j];
  }

  m_pieceShape = s;
}

int TetrisPiece::minX() const
{
  int min = m_coords[0][0];
  for( int i = 1; i < 4; i++ )
    min = qMin( min, m_coords[i][0] );
  return min;
}

int TetrisPiece::maxX() const
{
  int max = m_coords[0][0];
  for( int i = 1; i < 4; i++ )
    max = qMax( max, m_coords[i][0] );
  return max;
}

int TetrisPiece::minY() const
{
  int min = m_coords[0][1];
  for( int i = 1; i < 4; i++ )
    min = qMin( min, m_coords[i][1] );
  return min;
}

int TetrisPiece::maxY() const
{
  int max = m_coords[0][1];
  for( int i = 1; i < 4; i++ )
    max = qMax( max, m_coords[i][1] );
  return max;
}

TetrisPiece TetrisPiece::rotatedLeft() const
{
  if( m_pieceShape == SquareShape )
    return *this;

  TetrisPiece result;
  result.m_pieceShape = m_pieceShape;
  for( int i = 0; i < 4; i++ )
  {
    result.setX( i, y( i ) );
    result.setY( i, -x( i ) );
  }
  return result;
}

TetrisPiece TetrisPiece::rotatedRight() const
{
  if( m_pieceShape == SquareShape )
    return *this;

  TetrisPiece result;
  result.m_pieceShape = m_pieceShape;
  for( int i = 0; i < 4; i++ )
  {
    result.setX( i, -y( i ) );
    result.setY( i, x( i ) );
  }
  return result;
}
