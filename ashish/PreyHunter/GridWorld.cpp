#include "GridWorld.h"
#include <cstdlib>
#include <iostream>

namespace PredatorPreyHunter {
  using std::cerr;
  using std::endl;
  using std::abs;
  GridWorld::GridWorld( const int& width, const int& height ) {
    this->width = width;
    this->height = height;
  }
  uint GridWorld::distance( const Position& p1, const Position& p2 ) const {
    int distX, distY;
    distX = distY = 0;
    distX = abs( p1.x - p2.x );
    if ( distX > ( width / 2 ) ) {
      distX = width - distX;
    }
    distY = abs( p1.y - p2.y );
    if ( distY > ( height / 2 ) ) {
      distY = height - distY;
    }
    if ( distX < 0 || distY < 0 ) {
      cerr << "GridWorld::distance distX or distY is less than 0" << endl;
      throw 1; // throw something meaningful later
    }
    return static_cast<uint> ( distX + distY ); 
  }
  uint GridWorld::distanceX( const Position& p1, const Position& p2 ) const {
    int distX;
    distX = abs( p1.x - p2.x ); 
    if ( distX > ( width / 2 ) ) {
      distX = width - distX;
    }
    if ( distX < 0 ) {
      cerr << "GridWorld::distanceX distX < 0!" << endl;
      throw 1; // throw something meaningful later
    }
    return static_cast<uint> ( distX );
  }
  uint GridWorld::distanceY( const Position& p1, const Position& p2 ) const {
    int distY;
    distY = abs( p1.y - p2.y ); 
    if ( distY > ( height / 2 ) ) {
      distY = height - distY;
    }
    if ( distY < 0 ) {
      cerr << "GridWorld::distanceY distY < 0!" << endl;
      throw 1; // throw something meaningful later
    }
    return static_cast<uint> ( distY );
  }
  Position GridWorld::move( const Position& pCurrent, const Action& action ) const {
    Position pNew = pCurrent;
    if ( NORTH == action ) {
      pNew.y = pNew.y + 1;
    } else if ( SOUTH == action ) {
      pNew.y = pNew.y - 1;
    } else if ( EAST == action ) {
      pNew.x = pNew.x + 1;
    } else if ( WEST == action ) {
      pNew.x = pNew.x - 1;
    } // else do nothing
    // check if map width and height have been exceeded
    if ( pNew.x >= width ) {
      pNew.x = 0; 
    }
    if ( pNew.x < 0 ) {
      pNew.x = width - 1;
    }
    if ( pNew.y < 0 ) {
      pNew.y = height - 1;
    }
    if ( pNew.y >= height ) {
      pNew.y = 0;
    }
    return pNew;
  }
}
