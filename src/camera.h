//  $Id$
//
//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Matthias Braun <matze@braunis.de
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include <vector>
#include "vector.h"
#include "game_object.h"
#include "serializable.h"
#include <cassert>

class LispReader;
class Player;
class Level;

class Camera : public GameObject, public Serializable
{
public:
  Camera(Player* player = 0, Level* level = 0);
  virtual ~Camera();

  /** transforms a coordinate in world space to screen space.
   * Basically you have to apply this function to each coordinate that you want
   * to display on screen.
   */
  Vector world2screen(const Vector& worldpos) const
  {
    return worldpos - translation;                   
  }                                                  

  /// parse camera mode from lisp file
  void read(LispReader& reader);
  /// write camera mode to a lisp file
  virtual void write(LispWriter& writer);

  /** returns the current translation (=scroll) vector of the viewport */
  const Vector& get_translation() const
  { return translation; }
  /** set the curren translation vector of the viewport */
  void set_translation(const Vector& translation);

  virtual void action(float elapsed_time);

  enum CameraMode
  {
    NORMAL, AUTOSCROLL, MANUAL
  };
  CameraMode mode;

private:
  void scroll_normal(float elapsed_time);
  void scroll_autoscroll(float elapsed_time);
  void keep_in_bounds();

  enum LeftRightScrollChange
  {
    NONE, LEFT, RIGHT
  };
    
  Vector translation;

  Player* player;
  Level* level;

  // normal mode
  bool do_backscrolling;
  LeftRightScrollChange scrollchange;

  // autoscroll mode
  class ScrollPoint {
  public:
    Vector position;
    float speed;
  };
  std::vector<ScrollPoint> scrollpoints;
  size_t auto_idx;
  float auto_t;
  Vector current_dir;
};

#endif

