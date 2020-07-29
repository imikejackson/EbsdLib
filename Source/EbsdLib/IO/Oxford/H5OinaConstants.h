/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <string>

namespace EbsdLib
{
namespace H5Oina
{
// General Constants in the file
const std::string OxfordInstruments("Oxford Instruments");
const std::string Manufacturer("Manufacturer");
const std::string Version("Software Version");
const std::string H5FileExt("h5oina");
const std::string Isometric("isometric");
const std::string EBSD("EBSD");
const std::string Header("Header");
const std::string Phases("Phases");
const std::string Data("Data");
const std::string Index("Index");

// Header Section for EBSD Data
std::string AcquisitionDate("Acquisition Date");
std::string AcquisitionSpeed("Acquisition Speed");
std::string AcquisitionTime("Acquisition Time");
std::string BeamVoltage("Beam Voltage");
std::string DetectorOrientationEuler("Detector Orientation Euler");
std::string Magnification("Magnification");
std::string ProjectFile("Project File");
std::string ProjectLabel("Project Label");
std::string ProjectNotes("Project Notes");
std::string ScanningRotationAngle("Scanning Rotation Angle");
std::string SiteNotes("Site Notes");
std::string SpecimenOrientationEuler("Specimen Orientation Euler");
std::string TiltAngle("Tilt Angle");
std::string TiltAxis("Tilt Axis");
std::string XCells("X Cells");
std::string XStep("X Step");
std::string YCells("Y Cells");
std::string YStep("Y Step");

// Phase Headers
std::string Color("Color");
std::string LatticeAngles("Lattice Angles");
std::string LatticeDimensions("Lattice Dimensions");
std::string LaueGroup("Laue Group");
std::string NumberReflectors("Number Reflectors");
std::string PhaseName("Phase Name");
std::string Reference("Reference");
std::string SpaceGroup("Space Group");

// Data Labels
std::string BandContrast("Band Contrast");
std::string BandSlope("Band Slope");
std::string Bands("Bands");
std::string Error("Error");
std::string Euler("Euler");
std::string MeanAngularDeviation("Mean Angular Deviation");
std::string Phase("Phase");
std::string X("X");
std::string Y("Y");

using BandContrast_t = uint8_t;
using BandSlope_t = uint8_t;
using Bands_t = uint8_t;
using Error_t = uint8_t;
using Euler_t = float;
using MeanAngularDeviation_t = float;
using Phase_t = uint8_t;
using X_t = float;
using Y_t = float;

} // namespace H5Oina
} // namespace EbsdLib
