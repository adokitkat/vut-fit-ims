#pragma once

#include "etc.cpp"

#define WATER -7.0
#define DIRT 0.0
#define GRASS 0.5
#define TREE 0.9

enum struct Status 
{
  NONE,
  HEALTHY,
  INFECTED,
  RETRIEVED,
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
  Cell(int64_t, int64_t, bool, double, int64_t, int64_t, Status, CellType);
  ~Cell();

  int64_t x = 0;
  int64_t y = 0;
  bool    active       = false; // empty space or filled with fuel
  double  flammability = 0.0;

  int64_t fuel_amount  = 0;
  int64_t burning_tick = 0;

  Status status = Status::HEALTHY;
  CellType type = CellType::NonVaccinated;

  char cellStatusChar();

  void igniteCellRadius(std::vector<std::vector<Cell>>&, int64_t, int64_t, int64_t);
};

Cell::~Cell() {}

Cell::Cell(int64_t x, 
          int64_t y)
{
  this->x = x;
  this->y = y;
}

Cell::Cell(int64_t x, 
          int64_t y, 
          bool active, 
          double flammability, 
          int64_t fuel_amount, 
          int64_t burning_tick, 
          Status status, 
          CellType type)
{
  this->x = x;
  this->y = y;
  this->active = active;
  this->flammability = flammability;
  this->fuel_amount = fuel_amount;
  this->burning_tick = burning_tick;
  this->status = status;
  this->type = type;
}

char Cell::cellStatusChar() {
  switch (this->type)
  {
    case CellType::Vaccinated:
      if (this->status == Status::HEALTHY 
          or this->status == Status::RETRIEVED) { return 'V';}
      else if (this->status == Status::INFECTED) { return 'X';}
      else if (this->status == Status::DEAD) { return 'A';}
      break;
    
    case CellType::PartiallyVaccinated:
      if (this->status == Status::HEALTHY 
          or this->status == Status::RETRIEVED) { return 'P';}
      else if (this->status == Status::INFECTED) { return 'Y';}
      else if (this->status == Status::DEAD) { return 'B';}
      break;

    case CellType::NonVaccinated:
      if (this->status == Status::HEALTHY 
          or this->status == Status::RETRIEVED) { return 'N';}
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
