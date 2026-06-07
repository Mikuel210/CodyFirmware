#pragma once
#include "Line.h"
#include "PursuitData.h"
#include <cmath>
#include <vector>

class Pursuit {
  public:
    static Vector3 findLookahead(Vector3 position, PursuitData* data, bool allowOverflow = false) {
      std::vector<Line> lines;
      std::vector<double> lookaheadTimes;

      // Get lookahead points
      for (int i = 1; i < data->points.size(); i++)
        lines.push_back(Line(data->points[i - 1], data->points[i]));

      for (Line line : lines)
        lookaheadTimes.push_back(findLookaheadTime(position, line, data->lookaheadDistance, allowOverflow));

      // Find next lookahead point
      for (int i = lookaheadTimes.size() - 1; i >= 0; i--) {
        if (i != data->lineIndex && i != data->lineIndex + 1) continue;

        double t = lookaheadTimes[i];
        if (t < 0) continue;

        // Get point from time
        Line line = lines[i];
        double x = line.start.x + (line.end.x - line.start.x) * t;
        double y = line.start.y + (line.end.y - line.start.y) * t;

        data->lineIndex = i;
        return Vector3(x, y);
      }

      return lines[0].start;
    }

    static double findLookaheadTime(Vector3 position, Line line, double r, bool allowOverflow = false) {
      // Get vectors
      double dx = line.end.x - line.start.x;
      double dy = line.end.y - line.start.y;
      double fx = line.start.x - position.x;
      double fy = line.start.y - position.y;

      // Quadratic coefficients
      double a = dx * dx + dy * dy;
      double b = 2 * (fx * dx + fy * dy);
      double c = (fx * fx + fy * fy) - r * r;
      double d = b * b - 4 * a * c;

      if (d < 0) return -1;
      double t1 = (-b + sqrt(d)) / (2 * a);
      double t2 = (-b - sqrt(d)) / (2 * a);

      if (!allowOverflow) {
        t1 = std::clamp(t1, 0.0, 1.0);
        t2 = std::clamp(t2, 0.0, 1.0);
      }

      return max(t1, t2);
    }

    static double getClosestTime(Line line, Vector3 P) {
      Vector3 A = line.start;
      Vector3 B = line.end;

      Vector3 AB(B.x - A.x, B.y - A.y);
      Vector3 AP(P.x - A.x, P.y - A.y);

      // Calculate the dot product and the squared magnitude of AB
      double dotProduct = AP.x * AB.x + AP.y * AB.y;
      double squaredMagnitude = AB.x * AB.x + AB.y * AB.y;

      // Calculate the projection parameter t
      double t = -1.0;
      if (squaredMagnitude != 0) t = dotProduct / squaredMagnitude;

      return t;
    }
};
