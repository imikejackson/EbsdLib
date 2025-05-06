#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/Core/Quaternion.hpp"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/LaueOps.h"
#include "EbsdLib/OrientationMath/OrientationConverter.hpp"
#include "EbsdLib/Utilities/EbsdStringUtils.hpp"

#include "Test/TestPrintFunctions.h"

bool quaternionsAreClose(const QuatD& q1, const QuatD& q2, double tolerance = 1e-6)
{
  return fabs(q1.w() - q2.w()) < tolerance && fabs(q1.x() - q2.x()) < tolerance && fabs(q1.y() - q2.y()) < tolerance && fabs(q1.z() - q2.z()) < tolerance;
}

int v1(QuatD q1, QuatD q2)
{
  q1 = q1.unitQuaternion();
  q2 = q2.unitQuaternion();
  const QuatD q2Negate = -q2;

  LaueOps::Pointer cubicOps = LaueOps::GetAllOrientationOps()[1];

  int numSymOps = cubicOps->getNumSymOps();
  for(int s1op = 0; s1op < numSymOps; ++s1op)
  {
    const QuatD s1 = cubicOps->getQuatSymOp(s1op);

    for(int s2op = 0; s2op < numSymOps; ++s2op)
    {
      const QuatD s2 = cubicOps->getQuatSymOp(s2op);
      const QuatD s2Inverse = s2.inverse();
      QuatD qPrime = s1 * q1 * s2Inverse;
      if(quaternionsAreClose(qPrime, q2) || quaternionsAreClose(qPrime, q2Negate))
      {
        std::cout << "V1: Quats are same" << std::endl;
        // std::cout << "q1: ";
        // OrientationPrinters::Print_QU(q1);
        // std::cout << "q2: ";
        // OrientationPrinters::Print_QU(q2);
        // std::cout << "s1: ";
        // OrientationPrinters::Print_QU(s1);
        // std::cout << "s2: ";
        // OrientationPrinters::Print_QU(s2);
        return 1;
      }
    }
  }
  std::cout << "V1: Quats are not the same\n";
  return 0;
}

int v2(QuatD q1, QuatD q2)
{
  double tolerance = 1e-6;
  q1 = q1.unitQuaternion();
  q2 = q2.unitQuaternion();

  LaueOps::Pointer cubicOps = LaueOps::GetAllOrientationOps()[1];
  double minDiff = 1.0;
  int numSymOps = cubicOps->getNumSymOps();
  for(int s1op = 0; s1op < numSymOps; ++s1op)
  {
    const QuatD s1 = cubicOps->getQuatSymOp(s1op);

    for(int s2op = 0; s2op < numSymOps; ++s2op)
    {
      const QuatD s2Con = cubicOps->getQuatSymOp(s2op).conjugate();
      QuatD qPrime = s1 * q1 * s2Con;

      double dot = qPrime.dotProduct(q2);
      double diff = std::abs(1.0 - dot);
      minDiff = std::min(minDiff, diff);
      if(diff < tolerance)
      {
        std::cout << "V2: Quats are same. minDiff = " << minDiff << "\n";
        // std::cout << "q1: ";
        // OrientationPrinters::Print_QU(q1);
        // std::cout << "q2: ";
        // OrientationPrinters::Print_QU(q2);
        // std::cout << "s1: ";
        // OrientationPrinters::Print_QU(s1);
        // std::cout << "s2: ";
        // OrientationPrinters::Print_QU(cubicOps->getQuatSymOp(s2op));
        return 1;
      }
    }
  }
  std::cout << "V2: Quats are not the same. minDiff = " << minDiff << "\n";
  return 0;
}

std::vector<double> ReadDoublesFromCSV(const std::string& filePath)
{
  std::vector<double> values;
  std::ifstream file(filePath);

  if(!file.is_open())
  {
    throw std::runtime_error("Unable to open file: " + filePath);
  }

  std::string line;
  while(std::getline(file, line))
  {
    std::stringstream ss(line);
    std::string token;
    while(std::getline(ss, token, ','))
    {
      try
      {
        double value = std::stod(token);
        values.push_back(value);
      } catch(const std::invalid_argument& e)
      {
        std::cerr << "Warning: Skipping invalid token '" << token << "'\n";
      }
    }
  }

  return values;
}

// -----------------------------------------------------------------------------
int main2(int argc, char* argv[])
{
  std::vector<double> q = ReadDoublesFromCSV(argv[1]);
  for(int i = 0; i < q.size(); i=i+8)
  {
    std::cout << (i/8) << ": ";
    v2({q[i], q[i+1], q[i+2], q[i+3]}, {q[i+4], q[i+5], q[i+6], q[i+7]});
  }
  return 0;
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Quat for Grain ID:
  // 1   -0.24675526	0.30400401	-0.13882975	0.90962613
  // 30  -0.13404281	-0.35873663	-0.022394493	0.92349279
  // 142 -0.13404283	-0.35873666	-0.022394495	0.92349285

  double rad2Deg = 180.0 / 3.1415926535897932384626433832795;
  double axisToleranceRad = 3.0;
  double AngleTolerance = 3.0;

  double LD[3] = {0.0, 0.0, 1.0};
  CubicOps ops;
  {
    std::cout << "#============================================================\n";
    QuatD q1(0, 0, 0, 1);
    QuatD q2(0.28867513, 0.28867513, 0.28867513, 0.8660253999999999);

    OrientationD axisAngle = ops.calculateMisorientation(q1, q2);
    double w = axisAngle[3] * rad2Deg;

    double axisDiff111 = std::acos(std::fabs(axisAngle[0]) * 0.57735f + std::fabs(axisAngle[1]) * 0.57735f + fabs(axisAngle[2]) * 0.57735f);
    double angDiff60 = std::fabs(w - 60.0f);
    if(axisDiff111 < axisToleranceRad && angDiff60 < AngleTolerance)
    {
      std::cout << "Quat Pair is a Twin: \n";
    }
    else
    {
      std::cout << "Quat Pair is NOT a Twin: \n";
      std::cout << "   axisDiff111: " << axisDiff111 << std::endl;
      std::cout << "     angDiff60: " << angDiff60 << std::endl;
    }
    double lusterMorris = ops.getmPrime(q1, q2, LD);
    std::cout << "LM: " << lusterMorris << std::endl;
  }

  {
    std::cout << "#============================================================\n";
    QuatD q1(-0.24675526, 0.30400401, -0.13882975, 0.90962613);
    QuatD q2(-0.13404281, -0.35873663, -0.022394493, 0.92349279);
    OrientationD axisAngle = ops.calculateMisorientation(q1, q2);
    double w = axisAngle[3] * rad2Deg;

    double axisDiff111 = std::acos(std::fabs(axisAngle[0]) * 0.57735f + std::fabs(axisAngle[1]) * 0.57735f + fabs(axisAngle[2]) * 0.57735f);
    double angDiff60 = std::fabs(w - 60.0f);
    if(axisDiff111 < axisToleranceRad && angDiff60 < AngleTolerance)
    {
      std::cout << "Quat Pair is a Twin: \n";
    }
    else
    {
      std::cout << "Quat Pair is NOT a Twin: \n";
    }
    double lusterMorris = ops.getmPrime(q1, q2, LD);
    std::cout << "LM: " << lusterMorris << std::endl;
  }

  {
    std::cout << "#============================================================\n";
    QuatD q1(0.23, -0.34, 0.12, 0.9);
    QuatD q2(-0.13, 0.14, 0.15, 0.97);
    OrientationD axisAngle = ops.calculateMisorientation(q1, q2);
    double w = axisAngle[3] * rad2Deg;

    double axisDiff111 = std::acos(std::fabs(axisAngle[0]) * 0.57735f + std::fabs(axisAngle[1]) * 0.57735f + fabs(axisAngle[2]) * 0.57735f);
    double angDiff60 = std::fabs(w - 60.0f);
    std::cout << "axisDiff111: " << axisDiff111 << std::endl;
    std::cout << "angDiff60: " << angDiff60 << std::endl;
    if(axisDiff111 < axisToleranceRad && angDiff60 < AngleTolerance)
    {
      std::cout << "Quat Pair is a Twin: \n";
    }
    else
    {
      std::cout << "Quat Pair is NOT a Twin: \n";
    }
    double lusterMorris = ops.getmPrime(q1, q2, LD);
    std::cout << "EbsdLib v1.0.38 Original MPrime: " << lusterMorris << std::endl;
  }

  {
    std::cout << "#============================================================\n";
    QuatD q1(-0.24, -0.23, -0.35, 0.88);
    QuatD q2(0.02, 0.09, -0.04, 1);
    OrientationD axisAngle = ops.calculateMisorientation(q1, q2);
    double w = axisAngle[3] * rad2Deg;

    double axisDiff111 = std::acos(std::fabs(axisAngle[0]) * 0.57735f + std::fabs(axisAngle[1]) * 0.57735f + fabs(axisAngle[2]) * 0.57735f);
    double angDiff60 = std::fabs(w - 60.0f);
    std::cout << "axisDiff111: " << axisDiff111 << std::endl;
    std::cout << "angDiff60: " << angDiff60 << std::endl;
    if(axisDiff111 < axisToleranceRad && angDiff60 < AngleTolerance)
    {
      std::cout << "Quat Pair is a Twin: \n";
    }
    else
    {
      std::cout << "Quat Pair is NOT a Twin: \n";
    }
    double lusterMorris = ops.getmPrime(q1, q2, LD);
    std::cout << "EbsdLib v1.0.38 Original MPrime: " << lusterMorris << std::endl;
  }
}
