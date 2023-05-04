#pragma once

#include "cell.cpp"

// Map global variables
std::vector<std::vector<Cell>>  map,
                                newMap;

float pseudo_rand()
{
  return (float)rand()/13376.9f;  
}

int rand_mod_100()
{
  return static_cast<int>((double)rand()/4857.0f) % 100;
}

int rand_mod(int mod)
{
  return static_cast<int>((double)rand()/4857.0f) % mod;
}

void PerlinNoise2D(int nWidth, 
                  int nHeight, 
                  float* fSeed, 
                  int nOctaves, 
                  float fBias, 
                  float* fOutput)
{
	// Used 1D Perlin Noise
	for (int x = 0; x < nWidth; x++)
		for (int y = 0; y < nHeight; y++)
		{				
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;
					
				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;					
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[y * nWidth + x] = fNoise / fScaleAcc;
		}
	}

std::vector<std::string> seed4(std::vector<std::string> mapChars, 
                              int64_t size = MAX_SIZE, 
                              float _seed = 40000.0f, 
                              int people = 1000000,
                              float vaxd_percent = 40,
                              float par_vaxd_percent = 20,
                              float infected_percent = 0.4f) 
{

  int vaxd = static_cast<int>((people/100)*vaxd_percent);
  int par_vaxd = static_cast<int>((people/100)*par_vaxd_percent);
  int infected = static_cast<int>((people/100)*infected_percent);

  int max = size * size;
  std::string map1D(max, '.');
  int x;

  float *noise_seed = new float[size*size];
  float *out_seed   = new float[size*size];

  for (int i = 0; i < size * size; i++)
    { noise_seed[i] = pseudo_rand()*1.25f; }

  PerlinNoise2D(size, size, noise_seed, 9, 1.3f, out_seed);

  for (int i = 0; i < vaxd;)
  {
    x = rand() % max;
    if (map1D[x] != '.')
    {
      continue;
    } else {
      if( ((int)(out_seed[x] * 0.5f) > _seed && pseudo_rand() > _seed - 10000.0f))
      {
        map1D[x] = 'V';
        ++i;
      }
    }
  }

  for (int i = 0; i < par_vaxd;)
  {
    x = rand() % max;
    if (map1D[x] != '.')
    {
      continue;
    } else {
      
      if( ((int)(out_seed[x] * 0.5f) > _seed && pseudo_rand() > _seed - 10000.0f))
      {
        map1D[x] = 'P';
        ++i;
      }
    }
  }

  for (int i = 0; i < infected;)
  {
    x = rand() % max;
    if (map1D[x] != '.')
    {
      continue;
    } else {
      
      if( ((int)(out_seed[x] * 0.5f) > _seed && pseudo_rand() > _seed - 10000.0f))
      {
        map1D[x] = 'Z';
        ++i;
      }
    }
  }

  for (int i = 0; i < (people - vaxd - par_vaxd - infected);)
  {
    x = rand() % max;
    if (map1D[x] != '.')
    {
      continue;
    } else {
      if( ((int)(out_seed[x] * 0.5f) > _seed && pseudo_rand() > _seed - 10000.0f))
      {
        map1D[x] = 'N';
        ++i;
      }
      
    }

  }

  for (int i = 0; i < size; i++)
  {
    mapChars.push_back(map1D.substr(i*size, size));
  }

  delete [] noise_seed;
  delete [] out_seed;

  return mapChars;
}

namespace Map {

  void loadMap(std::vector<std::vector<Cell>>& map, 
              const std::vector<std::string>& mapChars, 
              int64_t h, 
              int64_t w)
  {
    for (int64_t i = 0; i < h; ++i){
      std::vector<Cell> vec {};
      for (int64_t j = 0; j < w; ++j) {
        vec.push_back(Cell(i, j));
        switch (mapChars[i][j])
        {
          // Vaccinated
          case 'V':
            vec[j].status = Status::HEALTHY;
            vec[j].type = CellType::Vaccinated;
            vec[j].active = true;
            break;

          case 'X':
            vec[j].status = Status::INFECTED;
            vec[j].type = CellType::Vaccinated; 
            vec[j].active = true;
            break;

          case 'A':
            vec[j].status = Status::DEAD;
            vec[j].type = CellType::Vaccinated;
            vec[j].active = false;
            break;

          // Partially vaccinated
          case 'P':
            vec[j].status = Status::HEALTHY;
            vec[j].type = CellType::PartiallyVaccinated;
            vec[j].active = true;
            break;

          case 'Y':
            vec[j].status = Status::INFECTED;
            vec[j].type = CellType::PartiallyVaccinated; 
            vec[j].active = true;
            break;
          
          case 'B':
            vec[j].status = Status::DEAD;
            vec[j].type = CellType::PartiallyVaccinated;
            vec[j].active = false;
            break;

          // Non-vaccinated
          case 'N':
            vec[j].status = Status::HEALTHY;
            vec[j].type = CellType::NonVaccinated;
            vec[j].active = true;
            break;

          case 'Z':
            vec[j].status = Status::INFECTED;
            vec[j].type = CellType::NonVaccinated;
            vec[j].active = true;
            break;

          case 'C':
            vec[j].status = Status::DEAD;
            vec[j].type = CellType::NonVaccinated;
            vec[j].active = false;
            break; 
          
          // Etc
          case '.':
            vec[j].status = Status::NONE;
            vec[j].type = CellType::Empty;
            vec[j].active = false;
            break;

          default:
            break;
        }
      }
      map.push_back(vec);
    }
  }

  void printMap(std::vector<std::vector<Cell>>& map)
  {
    for (auto& line : map)
    {
      for (auto& c : line)
      {
        std::cout << c.cellStatusChar();
      }
      std::cout << NEWLINE; 
    }
    std::cout << std::endl;
  }

  int moveCells(std::vector<std::vector<Cell>>& map, int stop_for) 
  {
    int64_t h = map.size(),
            w = map[0].size();
    
    if (tick % day > stop_for)
    {
      return 0;
    }

    for (int64_t i = 0; i < h; ++i)
    {
      for (int64_t j = 0; j < w; ++j)
      {
        if (!map[i][j].active) { continue; }
        else if (map[i][j].status == Status::DEAD) { continue; }

        // Pohyb
        std::vector<std::tuple<int,int>> possible_moves;

          for (auto x = (i - 1); x <= (i + 1); ++x) 
          {
            for (auto y = (j - 1); y <= (j + 1); ++y)
            {
              if (x == i and y == j) { continue; }

              auto a = x;
              auto b = y;
              if (x < 0) { a = h + x; }
              if (x >= h) { a = x - h; }
              if (y < 0) { b = w + y; }
              if (y >= w) { b = y - w; }

                if (!map[a][b].active) {
                  possible_moves.push_back(std::make_tuple(a, b));
                }
             
            }
          }

          if (possible_moves.size() == 0) { continue; }

          int direction = rand() % possible_moves.size();
          int x = std::get<0>(possible_moves[direction]); // x
          int y = std::get<1>(possible_moves[direction]); // y
          
          std::swap(map[i][j], map[x][y]);
      }
    }
    return 0;
  }

  int updateMapStatus(std::vector<std::vector<Cell>>& map)
  {
    int64_t h = map.size(),
            w = map[0].size();
    
    for (int64_t i = 0; i < h; ++i)
    {
      for (int64_t j = 0; j < w; ++j)
      {
        
        switch(map[i][j].status) {
            case Status::HEALTHY:
              if (map[i][j].type == CellType::Vaccinated)
              {
                ++vaccinated;
                ++healthy;
                ++v_healthy;
              }
              else if (map[i][j].type == CellType::PartiallyVaccinated)
              {
                ++partially_vaccinated;
                ++healthy;
                ++p_healthy;
              }
              else if (map[i][j].type == CellType::NonVaccinated)
              {
                ++nonvaccinated;
                ++healthy;
                ++n_healthy;
              }
              break;

            case Status::INFECTED:

              ++map[i][j].infected_counter;
              
              if (map[i][j].infected_counter > 100) { // Doba, kedy sa stane infekcny
                map[i][j].infectious = true;
              } else {
                map[i][j].infectious = false; 
              }
              
              if (map[i][j].type == CellType::Vaccinated)
              {
                ++vaccinated;
                if (map[i][j].infected_counter >= 300) // Evaulacia po case
                {
                  map[i][j].status = Status::HEALTHY;
                  map[i][j].infected_counter = 0;
                  map[i][j].infectious = false;
                  ++healthy;
                  ++v_healthy;
                  ++retrieved;
                  ++v_retrieved;
                }
                else // Pokracovanie infikacie
                {
                  ++infected;
                  ++v_infected;
                }
              
              }
              else if (map[i][j].type == CellType::PartiallyVaccinated)
              {
                ++partially_vaccinated;

              }
              else if (map[i][j].type == CellType::NonVaccinated)
              {
                ++nonvaccinated;
                if (map[i][j].infected_counter >= 350) // Evaulacia po case
                {
                  auto r = rand_mod(10000);
                  if (1.9f*100 >= r) // Sanca na umrtie
                  {
                    map[i][j].status = Status::DEAD;
                    map[i][j].active = false;
                    map[i][j].infectious = false;
                     map[i][j].infected_counter = 0;
                    ++dead;
                    ++n_dead;
                  }
                  else // Prezitie nakazy
                  {
                    map[i][j].status = Status::HEALTHY;
                    map[i][j].infected_counter = 0;
                    map[i][j].infectious = false;
                    ++healthy;
                    ++n_healthy;
                    ++retrieved;
                    ++n_retrieved;
                  }
                }
                else // Pokracovanie infikacie
                {
                  ++infected;
                  ++n_infected;
                }
              }
              break;

            case Status::DEAD:
              ++dead;
              if (map[i][j].type == CellType::Vaccinated)
              {
                ++vaccinated;
                ++v_dead;
              }
              else if (map[i][j].type == CellType::PartiallyVaccinated)
              {
                ++partially_vaccinated;
                ++p_dead;
              }
              else if (map[i][j].type == CellType::NonVaccinated)
              {
                ++nonvaccinated;
                ++n_dead;
              }
              break;

            default:
              break;
        }
      }
    }
    
     
    if (tick % log_period == 0) 
    {
      
      std::cout << "Tick: " << tick << " | Map size: " << map_size << " | Population: " << population << " | Vacc: " << vaccinated << " NonVacc: " << nonvaccinated << " | Infected (start): " << infected_start_percent << "% | Days: " << day << " Sleep: " << sleep_for_ticks << NEWLINE // << "\tActive: " << healthy+infected
       
        << " Healthy (All):\t\t" << healthy << "\tInfected (All):\t\t" << infected << "\tRetrtieved (All): \t" << retrieved << "\tDead (All):\t" << dead << NEWLINE //"\tAffected (All):\t\t" << burning+burned << NEWLINE
        << " Healthy (Vacc):\t" << v_healthy << "\tInfected (Vacc):\t" << v_infected << "\tRetrtieved (Vacc): \t" << v_retrieved  << "\tDead (Vacc)\t" << v_dead << NEWLINE //"\tAffected (Tree):\t" << burning_tree+burned_tree << NEWLINE
        << " Healthy (NonVacc):\t" << n_healthy << "\tInfected (NonVacc):\t" << n_infected << "\tRetrtieved (NonVac): \t" << n_retrieved  << "\tDead (NonVacc)\t" << n_dead << NEWLINE //"\tAffected (Brush):\t" << burning_brush+burned_brush << NEWLINE
        << std::endl;
      
      if (logging)
      {
        map_state_log
        << tick << "," << map_size << "," << population << "," << vaccinated << "," << nonvaccinated << "," << infected_start_percent << "," << day << "," << sleep_for_ticks << ","
        << healthy << "," << infected << "," << retrieved << "," << dead  << ","
        << v_healthy << "," << v_infected << "," << v_retrieved  << ","<< v_dead << ","
        << n_healthy << "," << n_infected << "," << n_retrieved  << "," << n_dead << std::endl;
      }
      
    }
    ++tick;

    vaccinated = 0;
    partially_vaccinated = 0;
    nonvaccinated = 0;

    healthy = 0;
    infected = 0;
    dead = 0;

    v_healthy = 0;
    v_infected = 0;
    v_dead = 0;

    p_healthy = 0;
    p_infected = 0;
    p_dead = 0;

    n_healthy = 0;
    n_infected = 0;
    n_dead = 0;

    return 0;
  }

  void infectCellRadius(std::vector<std::vector<Cell>>& map, 
                        int64_t i, 
                        int64_t j, 
                        int64_t radius = 3)
  {  
    for (auto x = (i - radius); x <= (i + radius); ++x) 
    {
      for (auto y = (j - radius); y <= (j + radius); ++y)
      {
        if (x >= 0 and x < h and y >= 0 and y < w)
        {
          if (map[x][y].status == Status::HEALTHY)
          {
            map[x][y].status = Status::INFECTED;
          }
        }
      }
    }

  }

}
