//== 3D calculations ==

float ratio;
u8g_uint_t hInset;
u8g_uint_t shift;
u8g_uint_t x0, y0;
u8g_uint_t zoom = 0;

bool blocked;

//== Animation ==
unsigned long timeToMove;
u8g_uint_t zoomSpeed = 1;

//== Screen ==
u8g_uint_t screenWidth, screenHeight;
u8g_uint_t screenHalfWidth, screenHalfHeight;

void XToCorners(u8g_uint_t x, Point* points) {
  u8g_uint_t y = (u8g_uint_t)(1.0 * x * ratio);
  points[0].Set(x0 - x, y0 - y); // 0 = top left
  points[1].Set(x0 + x, y0 - y); // 1 = top right
  points[2].Set(x0 + x, y0 + y); // 2 = bottom right
  points[3].Set(x0 - x, y0 + y); // 3 = bottom left
}

void DrawFrontLeftWall(Point* outs, Point* ins)
{
  u8g.drawLine(outs[0].X, outs[0].Y, ins[0].X, ins[0].Y);
  u8g.drawLine(ins[0].X, ins[0].Y, ins[3].X, ins[3].Y);
  u8g.drawLine(ins[3].X, ins[3].Y, outs[3].X, outs[3].Y);
}

void DrawFrontRightWall(Point* outs, Point* ins)
{
  u8g.drawLine(outs[1].X, outs[1].Y, ins[1].X, ins[1].Y);
  u8g.drawLine(ins[1].X, ins[1].Y, ins[2].X, ins[2].Y);
  u8g.drawLine(ins[2].X, ins[2].Y, outs[2].X, outs[2].Y);
}

void DrawBackLeftWall(Point* outs, Point* ins)
{
  u8g.drawLine(outs[0].X, ins[0].Y, ins[0].X, ins[0].Y);
  u8g.drawLine(ins[0].X, ins[0].Y, ins[3].X, ins[3].Y);
  u8g.drawLine(ins[3].X, ins[3].Y, outs[3].X, ins[3].Y);
}

void DrawBackRightWall(Point* outs, Point* ins)
{
  u8g.drawLine(outs[1].X, ins[1].Y, ins[1].X, ins[1].Y);
  u8g.drawLine(ins[1].X, ins[1].Y, ins[2].X, ins[2].Y);
  u8g.drawLine(ins[2].X, ins[2].Y, outs[2].X, ins[2].Y);
}

void DrawFrontWall(Point* outs, Point* ins)
{
  u8g.drawLine(outs[0].X, outs[0].Y, outs[1].X, outs[1].Y);
  u8g.drawLine(outs[1].X, outs[1].Y, outs[2].X, outs[2].Y);
  u8g.drawLine(outs[2].X, outs[2].Y, outs[3].X, outs[3].Y);
  u8g.drawLine(outs[3].X, outs[3].Y, outs[0].X, outs[0].Y);
}

void DrawBackWall(Point* outs, Point* ins)
{
  u8g.drawLine(ins[0].X, ins[0].Y, ins[1].X, ins[1].Y);
  u8g.drawLine(ins[1].X, ins[1].Y, ins[2].X, ins[2].Y);
  u8g.drawLine(ins[2].X, ins[2].Y, ins[3].X, ins[3].Y);
  u8g.drawLine(ins[3].X, ins[3].Y, ins[0].X, ins[0].Y);
}

void DebugWalls()
{
  if (millis() > timeToDebug)
  {
    Serial.println("--------------------------");
    Debug("hasFrontLeftWall",hasFrontLeftWall);
    Debug("hasFrontWall",hasFrontWall);
    Debug("hasFrontRightWall",hasFrontRightWall);
    Debug("hasBackLeftWall",hasBackLeftWall);
    Debug("hasBackWall",hasBackWall);
    Debug("hasBackRightWall",hasBackRightWall);
    timeToDebug = millis() + 2000;
  }
}
void DrawWalls(byte depth, byte col, byte row)
{
  Point outs[4], ins[4];
  XToCorners(screenHalfWidth - hInset * depth       + (depth == 0 ? 0 : zoom), outs);
  XToCorners(screenHalfWidth - hInset * (depth + 1) + zoom, ins);

  if (youDir == 0) LookNorth(row-depth, col);
  if (youDir == 1) LookEast(row, col+depth);
  if (youDir == 2) LookSouth(row+depth, col);
  if (youDir == 3) LookWest(row, col-depth);
  DebugWalls();

  if (hasFrontWall)
    DrawFrontWall(outs, ins);
  else
  {
    if (hasBackWall) DrawBackWall(outs, ins);

    if (hasFrontLeftWall) DrawFrontLeftWall(outs, ins);
    else if (hasBackLeftWall) DrawBackLeftWall(outs, ins);

    if (hasFrontRightWall) DrawFrontRightWall(outs, ins);
    else if (hasBackRightWall) DrawBackRightWall(outs, ins);
  }
}

void DrawMaze()
{
  u8g.drawFrame(0, 0, screenWidth, screenHeight);

  blocked = false; // Assume there's nothing in front of us
  for (byte depth = 0; depth < 3; depth++)
    if (!blocked) {
      DrawWalls(depth, youCol, youRow);
      blocked = hasFrontWall || hasBackWall;
    }
}

