#pragma once

bool PointToCircleCollision(float x, float y, float xc, float yc, float radius);
bool PointToEllipseCollision(float x, float y, float xc, float yc, float radius, float a, float b);
bool CircleToCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2);