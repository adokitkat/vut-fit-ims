#include <GL/glut.h>
#include "map.cpp"


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

              if (map[x][y].infectious == false) { break; }

              switch(map[i][j].type) { // Current cell type
                
                case CellType::Vaccinated:  // Current cell type
                  if (newMap[i][j].status == Status::HEALTHY) // Current cell status
                  {
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
  // Move cells on the map
  Map::moveCells(newMap, sleep_for_ticks);
  map = newMap;

  for (auto [i, line] : enumerate(map))
  {
    for (auto [j, c]: enumerate(line))
    {
      switch(c.status) {

        case Status::NONE:
        //background
          glColor3f(0.2f, 0.2f, 0.2f);
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
            // dark
            glColor3f(0.6f, 0.6f, 1.0f);
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
            glColor3f(0.2f, 0.2f, 1.0f);
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

  int64_t rand_seed;
  if (r_seed != 0)
  {
    rand_seed = r_seed; 
  }
  else
  {
    rand_seed = time(NULL); 
  }
  srand(rand_seed); // initialize random function

  std::vector<std::string> mapChars;
  mapChars = seed4(mapChars, map_size, 40000.0f, population, vaccinated_start_percent, 0, infected_start_percent);

  h = mapChars.size();
  w = mapChars[0].size();
  
  if (logging)
  {
    map_state_log = initLog("output.csv");
    std::cout << "Log message every " << log_period << " ticks." << std::endl;  
  }

  // Load map from chars to cells
  Map::loadMap(map, mapChars, h, w);
  newMap = map;

  if (gui) {
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(MAX_SIZE, MAX_SIZE);
    glutCreateWindow("IMS - Covid epidemic vaccination simulator - xmlkvy00, xmudry01");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
  }
  else {
    while (true)
    {
      // Update data of every cell on map
      updateCellAll(map, newMap);
      // Update status of every cell
      Map::updateMapStatus(newMap);
      // Move cells on the map
      Map::moveCells(newMap, sleep_for_ticks);
      map = newMap;
    }
  }
  exit(EXIT_SUCCESS);
}