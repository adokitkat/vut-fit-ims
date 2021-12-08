#include <GL/glut.h>
//#include "cell_blob.hpp"
#include "map.cpp"

//TODO: nahodnejsia zapalnost

using namespace std::chrono_literals;

int updateCell(const std::vector<std::vector<Cell>>& map, 
              std::vector<std::vector<Cell>>& newMap, 
              int64_t i,
              int64_t j,
              int64_t radius = 2)
{
  int64_t h = map.size(),
          w = map[0].size();
  
  int64_t a {-1*radius}, b {1*radius},
          c {-1*radius}, d {1*radius};

  int64_t distance {1};
  int64_t abs_x {0},
          abs_y {0};
  int64_t chance {0};

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
            case Status::HEALTHY: // Neighbor status
            case Status::DEAD: 
            case Status::NONE:
              break;

            case Status::INFECTED: // Neighbor status

              //if (!map[x][y].infectious) { continue; } // TODO: navysuj infekcnost v map.cpp

              switch(map[i][j].type) { // Current cell type
                
                case CellType::Vaccinated:  // Current cell type
                  // Neighbor type = map[x][y].type
                  if (newMap[i][j].status == Status::HEALTHY) // Current cell status
                  {
                    //newMap[i][j].flammability += std::pow(0.1, distance); 
                    chance = static_cast<int>(std::pow(0.02, distance)*100);
                    if(chance > rand() % 100)
                    {
                      newMap[i][j].status = Status::INFECTED;
                    }

                  }

                case CellType::PartiallyVaccinated:
                  if (newMap[i][j].status == Status::HEALTHY) // Current cell status
                  {
                    chance = static_cast<int>(std::pow(0.1, distance)*100);
                    if(chance > rand() % 100)
                    {
                      newMap[i][j].status = Status::INFECTED;
                    }

                  }
                  break;

                case CellType::NonVaccinated:
                  if (newMap[i][j].status == Status::HEALTHY) // Current cell status
                  {
                    chance = static_cast<int>(std::pow(0.3, distance)*100);
                    if(chance > rand() % 100)
                    {
                      newMap[i][j].status = Status::INFECTED;
                    }

                  }
                  break;

                default:
                  break;
            }
            default:
              break;
          }
        }

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
      updateCell(map, newMap, i, j, radius_global);
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
    Map::infectCellRadius(map, mouseXPos, mouseYPos, 10);
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
  int64_t infection_start_x {-1},
          infection_start_y {-1};
  
  // Parses arguments, fills variables with values
  parseArgs(argc, argv, gui, show_help, load_map, infection_start_x, infection_start_y);

  if (show_help) { showHelp(); exit(EXIT_SUCCESS); }

  srand(time(NULL)); // initialize random function
  std::vector<std::string> mapChars;
  mapChars = seed4(mapChars, 1000, 45000.0f, population, 75, 0);
  //mapChars = seed4(mapChars, 479, 'N', 47000.0f, 11025); 
  //mapChars = seed4(mapChars, 958 , 'N', 38000.0f, 22050); 
  //mapChars = seed4(mapChars, MAX_SIZE, 'N', 46000.0f, 500000); 
  //TODO: sem hodit po generovani nejake ohnisko v danom bode and watch the world burn

  //if (infection_start_x == -1) { infection_start_x = MAX_SIZE/2; }
  //if (infection_start_y == -1) { infection_start_y = MAX_SIZE/2; }

  h = mapChars.size();
  w = mapChars[0].size();
  
  // Start infection
  /*
  for (auto x = (infection_start_x - 2); x <= (infection_start_x + 2); ++x) 
  {
    for (auto y = (infection_start_y - 2); y <= (infection_start_y + 2); ++y)
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
    glutCreateWindow("IMS - Covid epidemic vaccination simulator - xmlkvy00, xmudry01");
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
    }
  }
  exit(EXIT_SUCCESS);
}