#include "math/polygon.hpp"
#include "util/log.hpp"

void Polygon::add_vertice(Vector v)
{
  vertices.push_back(v);
  if(vertices.size() > 1)
  {
    edges.push_back(v-vertices[vertices.size()-2]);
    // Add a normal for this edge
    normals.push_back(edges[edges.size()-1].perp());
  }

}

void Polygon::process_neighbor(Polygon& b)
{
  // Loop over all edges (and vertices in order to find 'wrong' edges)
  for(size_t i = 0;i < vertices.size()-1; i++)
  {
    auto& v = vertices[i];
    auto& v2 = vertices[i+1];
    for(size_t j = 0; j < b.vertices.size(); j++)
    {
      auto& w = b.vertices[j];
      if(std::abs(v.x-w.x)+std::abs(v.y-w.y) <= 10)
      {
        bool ok = false;
        if(j > 0)
        {
          auto& t = b.vertices[j-1];
          log_debug << "abs is " << std::abs(v2.x-t.x)+std::abs(v2.y-t.y) << std::endl;
          ok |= std::abs(v2.x-t.x)+std::abs(v2.y-t.y) <= 10;
        }
        if(j != b.vertices.size()-1)
        {
          auto& t = b.vertices[j+1];
          log_debug << "abs is " << std::abs(v2.x-t.x)+std::abs(v2.y-t.y) << std::endl;
          ok |= (std::abs(v2.x-t.x)+std::abs(v2.y-t.y)) <= 10;
        }
        log_debug << "Try" << std::endl;
        if(ok)
        {
          log_debug << "Disabled" << std::endl;
          normal_enabled[i] = false;
        }
      }
    }
  }
}

void Polygon::process_octile_neighbour(int dir, Polygon& b)
{
  // Check if the polygons have an edge at direction in common
}

void Polygon::handle_collision(Polygon& b, Manifold& m)
{
  // First: Check if there is a collision

  // Check if any of this Polygons axes seperates
  double d_inf = std::numeric_limits<double>::infinity();
  double minOverlap = d_inf;
  Vector minAxis;
  for(size_t i = 0; i < edges.size(); i++)
  {
    auto axis = edges[i];
    double overlap;
    if((overlap = is_seperating_axis(b, normals[i])) == 0.0f)
      return;
    if((std::abs(overlap) < std::abs(minOverlap) || minOverlap == d_inf) && normal_enabled[i])
    {
      minAxis = normals[i];
      minOverlap = overlap;
    }
  }
  // Check if any of b's axes seperates
  for(size_t i = 0; i < b.edges.size(); i++)
  {
    auto axis = b.edges[i];
    double overlap;
    if((overlap = is_seperating_axis(b, b.normals[i])) == 0.0f)
    {
        return;
    }
    if(std::abs(overlap) < std::abs(minOverlap) && b.normal_enabled[i])
    {
      minAxis = b.normals[i];
      minOverlap = overlap;
    }
  }
  //log_debug << "Polygonial collision occured" << std::endl;
  //log_debug << minOverlap << std::endl;
  // To resolve the collison use overlap as depth
  // and the axis normal as normal
  m.normal = minAxis.unit();
  log_debug << "Axis is " << m.normal.x << " " << m.normal.y << " " << m.depth << std::endl;
  m.depth = minOverlap;
  // TODO Ignore ignormals

}

double Polygon::is_seperating_axis(Polygon& b,const Vector& axis)
{
  Vector a_proj = this->project(axis);
  Vector b_proj = b.project(axis);
  double aRight = b_proj.y - a_proj.x;
  double aLeft  = a_proj.y - b_proj.x;
  if( aLeft < 0 || aRight < 0)
    return 0.0f;
  // Is a seperating axis iff ranges do not overlap
  if(aRight < aLeft)
    return aRight;
  return -aLeft;
  //return std::min(a_proj.y, b_proj.y)-std::max(a_proj.x, b_proj.x);
}

Vector Polygon::project(Vector axis)
{
  //assert(vertices.size() > 0);
  double minimum =  vertices[0]*axis.unit(), maximum = vertices[0]*axis.unit();
  for(const auto& vertice : vertices)
  {
    double proj = vertice*axis.unit();
    minimum = std::min(proj, minimum);
    maximum = std::max(proj, maximum);
  }
  return Vector(minimum, maximum);
}

void Polygon::disable_normal(const Vector& n)
{
  for(int i = 0;i<normals.size();i++)
  {
    if(!normal_enabled[i])
    {
      // Check if they are colinear
      if(normals[i].is_colinear(n))
      {
        normal_enabled[i] = false;
      }
    }
  }
}
void Polygon::debug()
{
  log_debug << "Poly debug" << std::endl;
  log_debug << "Vertices: " << vertices.size() << std::endl;
  for(const auto& v: vertices)
    log_debug << v.x << " " << v.y << "  , ";
  log_debug << std::endl;
}
void Polygon::setup()
{
  edges.push_back(vertices[vertices.size()-1]-vertices[0]);
  normals.push_back(edges[edges.size()-1].perp());
  normal_enabled.resize( normals.size() );
  for(int i = 0;i < normals.size();i++)
    normal_enabled[i] = true;
}