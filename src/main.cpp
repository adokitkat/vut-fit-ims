#include <GL/glut.h>
//#include "cell_blob.hpp"
#include "map.cpp"

//TODO: nahodnejsia zapalnost

using namespace std::chrono_literals;

int updateCell(const std::vector<std::vector<Cell>>& map, 
              std::vector<std::vector<Cell>>& newMap, 
              int64_t i,
              int64_t j, 
              Wind wDirection = Wind::None, 
              int64_t radius = 2, 
              int64_t k = 1, 
              int64_t big_wind = 3)
{
  int64_t h = map.size(),
          w = map[0].size();
  
  int64_t a {-1*radius}, b {1*radius},
          c {-1*radius}, d {1*radius};

  switch(wDirection) {
    case Wind::N:
      //if (k != 1)
      //  { a += 1; } // 1*a-(k-a)
      a += 1; b += 1*k;
      c += 0; d += 0;
      if (k >= big_wind) { a = radius; }
      break;

    case Wind::NW:
      a += 1;  b += 1*k;
      c += 1;  d += 1*k;
      if (k >= big_wind) { a = radius; c = radius; }
      break;

    case Wind::W:
      a += 0;  b += 0;
      c += 1;  d += 1*k;
      if (k >= big_wind) { c = radius; }
      break;

    case Wind::SW:
      a += -1*k; b += -1;
      c += 1;    d += 1*k;
      if (k >= big_wind) { b = -radius; c = radius; }
      break;
    
    case Wind::S:
      a += -1*k; b += -1;
      c += 0;    d += 0;
      if (k >= big_wind) { b = -radius; }
      break;

    case Wind::SE:
      a += -1*k; b += -1;
      c += -1*k; d += -1;
      if (k >= big_wind) { b = -radius; d = -radius; }
      break;

    case Wind::E:
      a += 0;    b += 0;
      c += -1*k; d += -1;
      if (k >= big_wind) { d = -radius; }
      break;

    case Wind::NE:
      a += 1;    b += 1*k;
      c += -1*k; d += -1;
      if (k >= big_wind) { a = radius; d = -radius; }
      break;
    
    default:
      break;
  }

  int64_t distance {1};
  int64_t abs_x {0},
          abs_y {0};

  for (auto x = (i + a); x <= (i + b); ++x) 
  {
    for (auto y = (j + c); y <= (j + d); ++y)
    {
      if (x == i and y == j) { continue; }

      if (x >= 0 and x < h and y >= 0 and y < w)
      {
        if (map[i][j].active)
        {
          distance = 1;
          abs_x = abs(i-x);
          abs_y = abs(j-y);
          if (abs_x > radius/2 or abs_y > radius/2) {
            if (abs_x > abs_y) {
              distance = abs_x;
            } else {
              distance = abs_y;
            }
          }

          switch (map[x][y].status) // Neighbor status
          {
            /*
            case Status::HEALTHY:
              //map[i][j] ; // Current Cell
              break;
            
            case Status::INFECTED:
              
              switch(map[i][j].type) { // My type
                case CellType::Tree:  // My type == Tree
              
                  if (newMap[i][j].status == Status::NOT_BURNING) // My status
                  {
                    switch (map[x][y].type) // Neighbor type
                    {
                      case CellType::Tree:
                        newMap[i][j].flammability += std::pow(0.1, distance); // TODO: range
                        break;
                      
                      case CellType::Brush:
                        newMap[i][j].flammability += std::pow(0.08, distance); // TODO: range
                        break;
                      
                      default:
                        break;
                    }
                  } 
                  break;

                case CellType::Brush: // My type == Brush
                  if (newMap[i][j].status == Status::NOT_BURNING) // My status
                  {
                    switch (map[x][y].type) // Neighbor type
                    {
                      case CellType::Tree:
                        newMap[i][j].flammability += std::pow(0.1, distance); // TODO: range
                        break;
                        
                      case CellType::Brush:
                        newMap[i][j].flammability += std::pow(0.12, distance); // TODO: range
                        break;
                        
                      default:
                        break;
                    }
                  }
                  break;

                  default:
                    break;
              }
              
              // 

              break;

            case Status::DEAD:
              break;
            */
            default:
              break;
          }
        }
        //if(newMap[x][y].flammability > 0.3) { std::cout << map[x][y].flammability << " " << newMap[x][y].flammability << NEWLINE; }
      }
    }
  }
  return 0;
}

void updateCellAll(std::vector<std::vector<Cell>>&map, std::vector<std::vector<Cell>>& newMap)
{
  //#pragma omp parallel for
  for (int64_t i = 0; i < h; ++i) {
    for (int64_t j = 0; j < w; ++j) {
      updateCell(map, newMap, i, j, wind, radius_global, k_global, big_wind_global);
    }
  }
}

void display()
{
  GLfloat minSize = 60.0f/map.size();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 60, 60, 0.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, MAX_SIZE, MAX_SIZE);

  // Mouse interaction
  if (leftMouseButtonDown) {
    Map::igniteCellRadius(map, mouseXPos, mouseYPos, 3);
  }

  // Update data of every cell on map
  updateCellAll(map, newMap);
  // Update status of every cell
  Map::updateMapStatus(newMap);
  map = newMap;

  for (auto [i, line] : enumerate(map))
  {
    for (auto [j, c]: enumerate(line))
    {
      switch(c.status) {

        case Status::NONE:
          glColor3f(1.0f, 1.0f, 1.0f);
          break;
        
        case Status::HEALTHY:
          if (c.type == CellType::Vaccinated) {
            // light green
            glColor3f(0.0f, 0.8f, 0.0f);
          }
          else if (c.type == CellType::PartiallyVaccinated) {
            // darker green 
            glColor3f(0.2f, 0.8f, 0.2f);
          }
          else if (c.type == CellType::NonVaccinated) {
            // dark green 
            glColor3f(0.4f, 0.8f, 0.4f);
          }
          break;

        case Status::INFECTED:
          if (c.type == CellType::Vaccinated) {
            // light 
            glColor3f(0.8f, 0.0f, 0.0f);
          }
          else if (c.type == CellType::PartiallyVaccinated) {
            // darker
            glColor3f(0.8f, 0.2f, 0.2f);
          }
          else if (c.type == CellType::NonVaccinated) {
            // dark
            glColor3f(0.8f, 0.4f, 0.4f);
          }
          break;

        case Status::RETRIEVED:
          if (c.type == CellType::Vaccinated) {
            // light 
            glColor3f(0.0f, 0.0f, 0.8f);
          }
          else if (c.type == CellType::PartiallyVaccinated) {
            // darker
            glColor3f(0.2f, 0.2f, 0.8f);
          }
          else if (c.type == CellType::NonVaccinated) {
            // dark
            glColor3f(0.4f, 0.4f, 0.8f);
          }
          break;

        case Status::DEAD:
          // gray
          glColor3f(0.1f, 0.1f, 0.1f);
          break;

        default:
          break;
      }

      glBegin(GL_QUADS); // 2x2 pixels
        glVertex2f(0.0f+minSize*j,     0.0f+minSize*i);
        glVertex2f(0.0f+minSize*(j+1), 0.0f+minSize*i);
        glVertex2f(0.0f+minSize*(j+1), 0.0f+minSize*(i+1));
        glVertex2f(0.0f+minSize*j,     0.0f+minSize*(i+1));
      glEnd();
    }
  }

  glutSwapBuffers();
  glutPostRedisplay();
  //std::this_thread::sleep_for(1ms);
}

int main (int argc, char *argv[])
{
  // Variables init
  bool  gui {false},
        show_help {false},
        load_map {false};
  int64_t wildfire_start_x {-1},
          wildfire_start_y {-1};
  std::string wind_dir = "";
  
  // Parses arguments, fills variables with values
  parseArgs(argc, argv, gui, show_help, load_map, wildfire_start_x, wildfire_start_y, wind_dir);

  parseWindDir(wind_dir);

  if (show_help) { showHelp(); exit(EXIT_SUCCESS); }

  srand(time(NULL)); // initialize random function
  std::vector<std::string> mapChars;
  mapChars = seed4(mapChars, 479, 'N', 47000.0f, 11025); 
  //mapChars = seed4(mapChars, 958 , 'N', 38000.0f, 22050); 
  //mapChars = seed4(mapChars, MAX_SIZE, 'N', 46000.0f, 500000); 
  //TODO: sem hodit po generovani nejake ohnisko v danom bode and watch the world burn

  //if (wildfire_start_x == -1) { wildfire_start_x = MAX_SIZE/2; }
  //if (wildfire_start_y == -1) { wildfire_start_y = MAX_SIZE/2; }

  h = mapChars.size();
  w = mapChars[0].size();
  
  // Start wildfire
  /*
  for (auto x = (wildfire_start_x - 2); x <= (wildfire_start_x + 2); ++x) 
  {
    for (auto y = (wildfire_start_y - 2); y <= (wildfire_start_y + 2); ++y)
    {
      if (x >= 0 and x < h and y >= 0 and y < w) {
        mapChars[x][y] = 'X';
      }
      else 
      {
        std::cerr << "Starting coorinates out of bounds! Select numbers in range 0-999." << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  */

  
  if (logging)
  {
    map_state_log = initLog("output.txt");
    std::cout << "Log message every " << log_period << " ticks." << std::endl;  
  }

  // Load map from chars to cells
  Map::loadMap(map, mapChars, h, w);
  newMap = map;

  if (gui) {
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(MAX_SIZE, MAX_SIZE);
    glutCreateWindow("IMS - Wildfire simulation - xmlkvy00, xmudry01");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
  }
  else {
    //bool running {false};
    while (true)
    {
      // Print the map
      Map::printMap(map);
      // Update data of every cell on map
      updateCellAll(map, newMap);
      // Update status of every cell
      Map::updateMapStatus(newMap);
      map = newMap;
      //std::this_thread::sleep_for(1s);
    }
  }
  exit(EXIT_SUCCESS);
}