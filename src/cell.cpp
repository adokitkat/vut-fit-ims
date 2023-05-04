#pragma once

#include "etc.cpp"

enum struct Status 
{
  NONE,
  HEALTHY,
  INFECTED,
  DEAD
};

enum struct CellType 
{
  Empty,
  NonVaccinated,
  PartiallyVaccinated,
  Vaccinated
};

class Cell {
private:
  /* data */
public:
  Cell();
  Cell(int64_t, int64_t);
  ~Cell();

  int64_t x = 0;
  int64_t y = 0;
  bool active = false;
  bool infectious = false;
  int64_t infected_counter = 0; // ticks

  Status status = Status::HEALTHY;
  CellType type = CellType::NonVaccinated;

  char cellStatusChar();

  void infectCellRadius(std::vector<std::vector<Cell>>&, int64_t, int64_t, int64_t);
};

Cell::~Cell() {}

Cell::Cell(int64_t x, 
          int64_t y)
{
  this->x = x;
  this->y = y;
}

char Cell::cellStatusChar() {
  switch (this->type)
  {
    case CellType::Vaccinated:
      if (this->status == Status::HEALTHY) { return 'V';}
      else if (this->status == Status::INFECTED) { return 'X';}
      else if (this->status == Status::DEAD) { return 'A';}
      break;
    
    case CellType::PartiallyVaccinated:
      if (this->status == Status::HEALTHY) { return 'P';}
      else if (this->status == Status::INFECTED) { return 'Y';}
      else if (this->status == Status::DEAD) { return 'B';}
      break;

    case CellType::NonVaccinated:
      if (this->status == Status::HEALTHY) { return 'N';}
      else if (this->status == Status::INFECTED) { return 'Z';}
      else if (this->status == Status::DEAD) { return 'C';}
      break;

    case CellType::Empty:
      return '.';
      break;
    
    default:
      return 'E';
      break;
  }
  return 'E';
}
