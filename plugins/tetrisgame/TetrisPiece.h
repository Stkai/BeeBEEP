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
// $Id: TetrisPiece.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEE_TETRISPIECE_H
#define BEE_TETRISPIECE_H


class TetrisPiece
{
public:
  enum Shape { NoShape, ZShape, SShape, LineShape, TShape, SquareShape,
               LShape, MirroredLShape, NumShapes };

  TetrisPiece();

  void setRandomShape();
  void setShape( Shape );

  inline Shape shape() const;
  inline int x( int ) const;
  inline int y( int ) const;
  int minX() const;
  int maxX() const;
  int minY() const;
  int maxY() const;
  TetrisPiece rotatedLeft() const;
  TetrisPiece rotatedRight() const;

private:
  inline void setX( int, int );
  inline void setY( int, int );

  Shape m_pieceShape;
  int m_coords[4][2];

};


// Inline Functions
inline TetrisPiece::Shape TetrisPiece::shape() const { return m_pieceShape; }
inline int TetrisPiece::x( int index ) const { return m_coords[ index ][ 0 ]; }
inline int TetrisPiece::y( int index ) const { return m_coords[ index ][ 1 ]; }
inline void TetrisPiece::setX( int index, int x ) { m_coords[ index ][ 0 ] = x; }
inline void TetrisPiece::setY( int index, int y ) { m_coords[ index ][ 1 ] = y; }

#endif  // BEE_TETRISPIECE_H
